/**
 * @file
 * @brief Encoder class description
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
#include "jbdrivers/GpioInterrupt.hpp"
#include <memory>

namespace jblib {
    namespace jbdrivers {
        class Encoder
        {
        public:
            typedef enum{
                STATE_IDLE = 0,
                STATE_LEFT = 1,
                STATE_RIGHT = 2
            }State_t;
            Encoder(gpio_num_t e1Pin, gpio_num_t e2Pin);
            State_t getState();
            void resetState();
        private:
            static constexpr const char* logTag_ = "[ Encoder ]";
            gpio_num_t e1Pin_ = GPIO_NUM_NC;
            gpio_num_t e2Pin_ = GPIO_NUM_NC;
            std::unique_ptr<jblib::jbdrivers::GpioInterrupt> gpioInterrupt_;
            State_t state_ = STATE_IDLE;
            xQueueHandle filterSem_ = xSemaphoreCreateBinary();
        };
    }
}
