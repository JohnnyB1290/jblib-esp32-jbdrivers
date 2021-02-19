/**
 * @file
 * @brief UART Void Channel class definition
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

#pragma once

#include "jbkernel/jb_common.h"
#include "jbkernel/callback_interfaces.hpp"
#include "jbkernel/IVoidChannel.hpp"
#include "jbkernel/JbKernel.hpp"
#include "driver/uart.h"
#include <condition_variable>

namespace jblib
{
    namespace jbdrivers
    {

        class UartVoidChannel : public ::jblib::jbkernel::VoidChannel
        {
            static constexpr const char* logTag_ = "[ UART Void Channel ]";
            QueueHandle_t uartEventQueue_ = nullptr;
            bool isInitialized_ = false;
            std::mutex threadExitCvMutex_;
            std::condition_variable threadExitCv_;

            void eventHandler();

        public:

            typedef struct
            {
                int txPin = -1;
                int rxPin = -1;
                int rtsPin = -1;
                int ctsPin = -1;
                uart_port_t portNumber = UART_NUM_1;
                uart_word_length_t wordLength = UART_DATA_8_BITS;
                uart_stop_bits_t stopBits = UART_STOP_BITS_1;
                uart_parity_t parity = UART_PARITY_DISABLE;
                uart_hw_flowcontrol_t hwFlowControl = UART_HW_FLOWCTRL_DISABLE;
                bool swFlowControl = false;
                uint32_t baudRate = 115200;
                int interruptAllocFlags = ESP_INTR_FLAG_LOWMED;
            }Parameters_t;

            explicit UartVoidChannel(Parameters_t& parameters);
            ~UartVoidChannel() override;
            void initialize() override ;
            void tx(uint8_t* data, uint16_t size, void* connectionParameter) override ;

        protected:
            Parameters_t parameters_;
        };
    }
}
