/**
 * @file
 * @brief GPIO Interrupt class definition
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

#pragma once

#include "jbkernel/jb_common.h"
#include "jbkernel/callback_interfaces.hpp"
#include "jbdrivers/JbController.hpp"

namespace jblib {
    namespace jbdrivers {

        class GpioInterrupt
        {
        public:
            struct Configuration
            {
                gpio_num_t pin = GPIO_NUM_NC;
                gpio_pullup_t pullUp = GPIO_PULLUP_DISABLE;
                gpio_pulldown_t pullDown = GPIO_PULLDOWN_DISABLE;
                gpio_int_type_t edge = GPIO_INTR_ANYEDGE;
            };

            static void globalEnableGpioInterrupt(uint32_t intrFlags = ESP_INTR_FLAG_LOWMED);
            static void globalDisableGpioInterrupt();

            GpioInterrupt(const Configuration& config);
            virtual ~GpioInterrupt();
            void setCallback(::jblib::jbkernel::IVoidCallback* callback, void* args = nullptr);
            void resetCallback();
            void enable();
            void disable();

        private:
            static constexpr const char* logTag_ = "[ Gpio Intr ]";

            ::jblib::jbkernel::IVoidCallback* callback_ = nullptr;
            void* callbackArgs_ = nullptr;
            gpio_num_t pin_ = GPIO_NUM_NC;

            static void isrHandler(void* arg);
        };

    }
}
