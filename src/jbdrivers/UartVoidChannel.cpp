/**
 * @file
 * @brief UART Void Channel class realization
 *
 *
 * @note
 * Copyright © 2021 Evgeniy Ivanov. Contacts: <strelok1290@gmail.com>
 * All rights reserved.
 * @note
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 * @note
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @note
 * This file is a part of JB_Lib.
 */

// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "jbkernel/jb_common.h"
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 1, 0))

#include "jbdrivers/UartVoidChannel.hpp"
#include <esp_pthread.h>
#include <thread>
#include <vector>
#include <soc/uart_reg.h>

namespace jblib
{
    namespace jbdrivers
    {
        using namespace ::jblib::jbkernel;

        UartVoidChannel::UartVoidChannel(Parameters_t& parameters) : VoidChannel(), parameters_(parameters)
        {
            #if !CONFIG_UART_CHANNEL_CONSOLE_ENABLE
            esp_log_level_set(logTag_, ESP_LOG_WARN);
            #endif
        }



        void UartVoidChannel::initialize()
        {
            if(!this->isInitialized_){
                esp_err_t result;
                uart_config_t uartConfig{};
                uartConfig.baud_rate = this->parameters_.baudRate;
                uartConfig.data_bits = this->parameters_.wordLength;
                uartConfig.parity = this->parameters_.parity;
                uartConfig.stop_bits = this->parameters_.stopBits;
                uartConfig.flow_ctrl = this->parameters_.hwFlowControl;
                uartConfig.source_clk = UART_SCLK_APB;

                if(uart_param_config(this->parameters_.portNumber, &uartConfig) != ESP_OK) {
                    ESP_LOGE(logTag_, "Initialize uart_param_config error");
                    return;
                }
                if(uart_set_pin(this->parameters_.portNumber, this->parameters_.txPin, this->parameters_.rxPin,
                                this->parameters_.rtsPin, this->parameters_.ctsPin) != ESP_OK) {
                    ESP_LOGE(logTag_, "Initialize uart_set_pin error");
                    return;
                }
                if (this->parameters_.swFlowControl) {
                    result = uart_set_sw_flow_ctrl(this->parameters_.portNumber, true,
                            8, UART_FIFO_LEN - 8);
                }
                else{
                    result = uart_set_hw_flow_ctrl(this->parameters_.portNumber,
                            this->parameters_.hwFlowControl, UART_FIFO_LEN - 8);
                }
                if(result != ESP_OK){
                    ESP_LOGE(logTag_, "Set flow control error");
                    return;
                }
                result = uart_driver_install(this->parameters_.portNumber, CONFIG_UART_CHANNEL_RX_BUFFER_SIZE,
                        CONFIG_UART_CHANNEL_TX_BUFFER_SIZE, CONFIG_UART_CHANNEL_EVENT_QUEUE_SIZE,
                        &(this->uartEventQueue_), this->parameters_.interruptAllocFlags);

                if(result != ESP_OK){
                    ESP_LOGE(logTag_, "Initialize uart_driver_install error");
                    return;
                }

                uart_intr_config_t uartIntrConfig{};
                uartIntrConfig.intr_enable_mask = UART_RXFIFO_FULL_INT_ENA_M
                        | UART_RXFIFO_TOUT_INT_ENA_M
                        | UART_FRM_ERR_INT_ENA_M
                        | UART_RXFIFO_OVF_INT_ENA_M
                        | UART_BRK_DET_INT_ENA_M
                        | UART_PARITY_ERR_INT_ENA_M;
                uartIntrConfig.rxfifo_full_thresh = CONFIG_UART_CHANNEL_RX_FIFO_FULL_TRESHOLD;
                uartIntrConfig.rx_timeout_thresh = CONFIG_UART_CHANNEL_RX_TIMEOUT_TRESHOLD;
                uartIntrConfig.txfifo_empty_intr_thresh = CONFIG_UART_CHANNEL_TX_FIFO_EMPTY_TRESHOLD;
                result = uart_intr_config(this->parameters_.portNumber, &uartIntrConfig);
                if(result != ESP_OK){
                    ESP_LOGE(logTag_, "Initialize uart_intr_config error");
                    uart_driver_delete(this->parameters_.portNumber);
                    return;
                }

                auto cfg = esp_pthread_get_default_config();
                std::function<void()> eventHandlerFunc = std::bind( &UartVoidChannel::eventHandler, this);
                cfg.thread_name = logTag_;
                cfg.stack_size = CONFIG_UART_CHANNEL_TASK_STACK_SIZE;
                cfg.prio = CONFIG_UART_CHANNEL_TASK_PRIORITY;
                esp_pthread_set_cfg(&cfg);
                std::thread handlerThread(eventHandlerFunc);
                handlerThread.detach();
                this->isInitialized_ = true;
            }
        }



        void UartVoidChannel::eventHandler()
        {
            uart_event_t event;
            while(true){
                if (xQueueReceive(this->uartEventQueue_, &event, portMAX_DELAY) == pdTRUE) {
                    switch (event.type) {

                        case UART_DATA:
                        {
                            size_t length = 0;
                            uart_get_buffered_data_len(this->parameters_.portNumber, &length);
                            if(length){
                                auto data = std::vector<uint8_t>(length);
                                length = uart_read_bytes(this->parameters_.portNumber, data.data(),
                                                         length, portMAX_DELAY);
                                ESP_LOGI(logTag_, "Uart received %i bytes", length);
                                this->invokeCallback(data.data(), length, this, nullptr);
                            }
                            else{
                                ESP_LOGE(logTag_, "Uart received 0 bytes");
                            }
                        }
                            break;

                        case UART_FIFO_OVF:
                        {
                            ESP_LOGW(logTag_, "UART FIFO Overflow");
                            uart_flush_input(this->parameters_.portNumber);
                            xQueueReset(this->uartEventQueue_);
                        }
                            break;

                        case UART_BUFFER_FULL:
                        {
                            ESP_LOGW(logTag_, "UART Ring buffer full");
                            uart_flush_input(this->parameters_.portNumber);
                            xQueueReset(this->uartEventQueue_);
                        }
                            break;

                        case UART_BREAK:
                            ESP_LOGW(logTag_, "UART Rx Break");
                            break;

                        case UART_PARITY_ERR:
                            ESP_LOGE(logTag_, "UART Parity Error");
                            break;

                        case UART_FRAME_ERR:
                            ESP_LOGE(logTag_, "UART Frame Error");
                            break;

                        case UART_EVENT_MAX:
                        {
                            std::unique_lock<std::mutex> lock(this->threadExitCvMutex_);
                            std::notify_all_at_thread_exit(this->threadExitCv_, std::move(lock));
                            ESP_LOGE(logTag_, "Exit thread");
                            return;
                        }

                        default:
                            ESP_LOGW(logTag_, "Unknown UART event type: %d", event.type);
                            break;
                    }
                }
            }
        }



        void UartVoidChannel::tx(uint8_t* data, uint16_t size, void* connectionParameter)
        {
            (void)connectionParameter;
            if(this->isInitialized_) {
                int length = uart_write_bytes(this->parameters_.portNumber, reinterpret_cast<char*>(data), size);
                if(length < size){
                    ESP_LOGE(logTag_, "Tx failed: transmitted %i bytes, size %i", length, size);
                }
            }
        }



        UartVoidChannel::~UartVoidChannel()
        {
            if(this-isInitialized_){
                std::unique_lock<std::mutex> lock(this->threadExitCvMutex_);
                uart_event_t event = {UART_EVENT_MAX, 0, false};
                xQueueSend(this->uartEventQueue_, &event, portMAX_DELAY);
                this->threadExitCv_.wait(lock);
                uart_driver_delete(this->parameters_.portNumber);
                this->isInitialized_ = false;
                ESP_LOGE(logTag_, "Destruct success");
            }
        }

    }
}

#endif