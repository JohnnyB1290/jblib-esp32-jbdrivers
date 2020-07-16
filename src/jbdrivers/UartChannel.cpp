/**
 * @file
 * @brief UART Void Channel class realization
 *
 *
 * @note
 * Copyright © 2020 Evgeniy Ivanov. Contacts: <strelok1290@gmail.com>
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

#include <cstring>
#include "jbdrivers/UartChannel.hpp"

namespace jblib
{
    namespace jbdrivers
    {
        using namespace ::jblib::jbkernel;

        UartChannel::UartChannel(Parameters_t* parameters) : IVoidChannel(), IVoidCallback()
        {
            memcpy(&this->parameters_, parameters, sizeof(this->parameters_));
        }



        UartChannel::~UartChannel()
        {
            this->deinitialize();
        }



        void UartChannel::initialize(void* (* const mallocFunc)(size_t),
                const uint16_t txBufferSize, ::jblib::jbkernel::IChannelCallback* const callback)
        {
            if(!this->isInitialized_){
                esp_err_t result = ESP_FAIL;
                uart_config_t uartConfig;
                memset(&uartConfig, 0, sizeof(uart_config_t));
                uartConfig.baud_rate = this->parameters_.baudRate;
                uartConfig.data_bits = this->parameters_.wordLength;
                uartConfig.parity = this->parameters_.parity;
                uartConfig.stop_bits = this->parameters_.stopBits;
                uartConfig.flow_ctrl = this->parameters_.hwFlowControl;

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
                        &(this->uartEventQueue_), 0);

                if(result != ESP_OK){
                    ESP_LOGE(logTag_, "Initialize uart_driver_install error");
                    return;
                }
                result = uart_enable_rx_intr(this->parameters_.portNumber);
                if(result != ESP_OK){
                    ESP_LOGE(logTag_, "Initialize uart_enable_rx_intr error");
                    uart_driver_delete(this->parameters_.portNumber);
                    return;
                }
                this->taskHandle_ = JbKernel::addMainProcedure(this, nullptr,
                        CONFIG_UART_CHANNEL_TASK_STACK_SIZE,
                        CONFIG_UART_CHANNEL_TASK_PRIORITY, logTag_);
                this->isInitialized_ = true;
            }
            this->callback_ = callback;
        }



        void UartChannel::deinitialize(void)
        {
            this->callback_ = nullptr;
            if(this-isInitialized_){
                vTaskDelete(this->taskHandle_->taskHandle);
                free_s(this->taskHandle_);
                uart_driver_delete(this->parameters_.portNumber);
                this->isInitialized_ = false;
            }
        }



        void UartChannel::tx(uint8_t* const buffer, const uint16_t size, void* parameter)
        {
            if(this-isInitialized_) {
                int length = uart_write_bytes(this->parameters_.portNumber, (char*)buffer, size);
                if(length < size){
                    ESP_LOGE(logTag_, "Tx failed: transmitted %i bytes, size %i", length, size);
                }
            }
        }



        void UartChannel::getParameter(const uint8_t number, void* const value)
        {

        }



        void UartChannel::setParameter(const uint8_t number, void* const value)
        {

        }



        void UartChannel::voidCallback(void* source, void* parameter)
        {
            uart_event_t event;
            if (xQueueReceive(this->uartEventQueue_, &event, portMAX_DELAY) == pdTRUE) {
                switch (event.type) {

                    case UART_DATA:
                    {
                        size_t length = 0;
                        uart_get_buffered_data_len(this->parameters_.portNumber, &length);
                        if(length){
                            auto buffer = (uint8_t*)malloc_s(length);
                            if(!buffer){
                                ESP_LOGE(logTag_, "Couldn't allocate memory for buffer %i, free heap: %i", length, JbKernel::getHeapFree());
                                break;
                            }
                            length = uart_read_bytes(this->parameters_.portNumber, buffer, length, portMAX_DELAY);
                            ESP_LOGI(logTag_, "Uart received %i bytes", length);
                            if(this->callback_){
                                this->callback_->channelCallback(buffer, length, this, nullptr);
                            }
                            free_s(buffer);
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

                    default:
                        ESP_LOGW(logTag_, "Unknown UART event type: %d", event.type);
                        break;
                }
            }
        }

    }
}
