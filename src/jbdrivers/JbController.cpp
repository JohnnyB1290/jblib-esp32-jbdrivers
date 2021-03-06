/**
 * @file
 * @brief JbController class realization
 *
 *
 * @note
 * Copyright © 2019 Evgeniy Ivanov. Contacts: <strelok1290@gmail.com>
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
#include "jbdrivers/JbController.hpp"

namespace jblib
{
namespace jbdrivers
{

using namespace jbkernel;

const BoardGpio_t JbController::boardGpios_[] = JBCONTROLLER_BOARD_GPIOS;
char* JbController::serial_ = nullptr;


void JbController::initialize()
{
	static bool isInitialized = false;
	if(!isInitialized) {
        for(auto & boardGpio : boardGpios_) {
            if(boardGpio.pin != GPIO_NUM_NC){
                gpio_pad_select_gpio(boardGpio.pin);
                gpio_set_direction(boardGpio.pin, boardGpio.direction);
                gpio_set_pull_mode(boardGpio.pin,	boardGpio.pullMode);
                if((boardGpio.direction != GPIO_MODE_INPUT) &&  (boardGpio.direction != GPIO_MODE_DISABLE)){
                    gpio_set_level(boardGpio.pin, 0);
                }
            }
        }
		isInitialized = true;
	}
}



void JbController::gpioOn(uint8_t number)
{
    if(boardGpios_[number].pin != GPIO_NUM_NC){
        gpio_set_level(boardGpios_[number].pin, 1);
    }
}



void JbController::gpioOff(uint8_t number)
{
    if(boardGpios_[number].pin != GPIO_NUM_NC){
        gpio_set_level(boardGpios_[number].pin, 0);
    }
}



void JbController::gpioTgl(uint8_t number)
{
    if(boardGpios_[number].pin == GPIO_NUM_NC){
        return;
    }

    bool isOn = false;
    if (boardGpios_[number].pin < 32) {
        isOn = (GPIO.out >> boardGpios_[number].pin) & 1U;
    } else {
        isOn = (GPIO.out1.data >> boardGpios_[number].pin) & 1U;
    }

    if(isOn){
        JbController::gpioOff(number);
    }
    else{
        JbController::gpioOn(number);
    }
}


bool JbController::getGpio(uint8_t number)
{
    return (boardGpios_[number].pin != GPIO_NUM_NC) ? gpio_get_level(boardGpios_[number].pin) : false;
}


char* JbController::getSerial()
{
    if(!serial_){
        serial_ = (char*)malloc_s(JBCONTROLLER_SERIAL_MAX_LENGTH);
        if(serial_){
            memset(serial_, 0, JBCONTROLLER_SERIAL_MAX_LENGTH);
            uint8_t mac[6];
            esp_efuse_mac_get_default(mac);
            snprintf(serial_, JBCONTROLLER_SERIAL_MAX_LENGTH,
                     "%02x%02x-%02x%02x-%02x%02x",
                     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        }
    }
    return serial_;
}

}
}
