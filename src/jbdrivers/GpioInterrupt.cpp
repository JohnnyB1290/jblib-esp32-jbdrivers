/**
 * @file
 * @brief GPIO Interrupt class realization
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

// This is an independent project of an individual developer. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <stdexcept>
#include "jbdrivers/GpioInterrupt.hpp"

using namespace ::jblib::jbkernel;
using namespace ::jblib::jbdrivers;



void GpioInterrupt::globalEnableGpioInterrupt(uint32_t intrFlags)
{
    auto ret = gpio_install_isr_service(intrFlags);
    if(ret != ESP_OK){
        ESP_LOGE(logTag_, "Install isr service error %i", ret);
    }
}



void GpioInterrupt::globalDisableGpioInterrupt()
{
    gpio_uninstall_isr_service();
}



void IRAM_ATTR GpioInterrupt::isrHandler(void* arg)
{
    auto gpioInterrupt = reinterpret_cast<GpioInterrupt*>(arg);
    if(gpioInterrupt->callback_ != nullptr){
        gpioInterrupt->callback_(arg, gpioInterrupt->callbackParameter_);
    }
}



GpioInterrupt::GpioInterrupt(const Configuration& config) : CallbackCaller(), pin_(config.pin)
{
    gpio_config_t gpioConfig{};
    gpioConfig.pin_bit_mask = 1ULL << static_cast<uint32_t>(config.pin);
    gpioConfig.mode = GPIO_MODE_INPUT;
    gpioConfig.pull_up_en = config.pullUp;
    gpioConfig.pull_down_en = config.pullDown;
    gpioConfig.intr_type = config.edge;
    if(gpio_config(&gpioConfig) != ESP_OK){
        #if CONFIG_COMPILER_CXX_EXCEPTIONS
        throw std::logic_error("Gpio Config wrong parameters");
        #else
        ESP_LOGE(logTag_,"Gpio Config wrong parameters");
        return;
        #endif
    }
    auto ret = gpio_isr_handler_add(config.pin, isrHandler, this);
    if(ret == ESP_ERR_INVALID_STATE){
        globalEnableGpioInterrupt();
        ret = gpio_isr_handler_add(config.pin, isrHandler, this);
    }
    if(ret != ESP_OK){
        #if CONFIG_COMPILER_CXX_EXCEPTIONS
        throw std::logic_error("Add ISR handler error");
        #else
        ESP_LOGE(logTag_,"Add ISR handler error");
        #endif
    }
    this->disable();
}



GpioInterrupt::~GpioInterrupt()
{
    gpio_isr_handler_remove(this->pin_);
    gpio_reset_pin(this->pin_);
}



void GpioInterrupt::enable()
{
    gpio_intr_enable(this->pin_);
}



void GpioInterrupt::disable()
{
    gpio_intr_disable(this->pin_);
}
