/**
 * @file
 * @brief UART Void Channel class definition
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

#ifndef UARTCHANNEL_HPP
#define UARTCHANNEL_HPP

#include "jbkernel/jb_common.h"
#include "jbkernel/callback_interfaces.hpp"
#include "jbkernel/IVoidChannel.hpp"
#include "jbkernel/JbKernel.hpp"
#include "driver/uart.h"

namespace jblib
{
    namespace jbdrivers
    {

    class UartChannel : public ::jblib::jbkernel::IVoidChannel, ::jblib::jbkernel::IVoidCallback
        {
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
            }Parameters_t;

            explicit UartChannel(Parameters_t* parameters);
            ~UartChannel() override;
            void initialize(void* (* mallocFunc)(size_t),
                    uint16_t txBufferSize, ::jblib::jbkernel::IChannelCallback* callback) override;
            void deinitialize() override;
            void tx(uint8_t* buffer, uint16_t size, void* parameter) override;
            void getParameter(uint8_t number, void* value) override;
            void setParameter(uint8_t number, void* value) override;

        protected:
            void voidCallback(void* source, void* parameter) override;

            jblib::jbkernel::JbKernel::ProceduresListItem* taskHandle_ = nullptr;
            Parameters_t parameters_;

        private:
            static constexpr const char* logTag_ = "[ UART Channel ]";
            jblib::jbkernel::IChannelCallback* callback_ = nullptr;
            QueueHandle_t uartEventQueue_ = nullptr;
            bool isInitialized_ = false;

        };
    }
}


#endif //UARTCHANNEL_HPP
