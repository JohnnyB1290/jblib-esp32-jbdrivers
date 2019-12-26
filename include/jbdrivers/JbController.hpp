/**
 * @file
 * @brief JbController class definition
 *
 *
 * @note
 * Copyright � 2019 Evgeniy Ivanov. Contacts: <strelok1290@gmail.com>
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

#ifndef JB_CONTROLLER_HPP_
#define JB_CONTROLLER_HPP_

#include <stdint.h>
#include "jbkernel/jb_common.h"
#include "jbkernel/JbKernel.hpp"
#include "driver/gpio.h"

typedef struct{
    gpio_num_t pin;
    gpio_mode_t direction;
    gpio_pull_mode_t pullMode;
}BoardGpio_t;

namespace jblib
{
namespace jbdrivers
{

using namespace jbkernel;

class JbController : public JbKernel
{
public:
	static void initialize(void);
	static void gpioOn(uint8_t number);
	static void gpioOff(uint8_t number);
	static void gpioTgl(uint8_t number);
    static bool getGpio(uint8_t number);

private:
	static BoardGpio_t boardGpios_[];
};

}
}

#endif /*  JB_CONTROLLER_HPP_ */
