/**
 * @file
 * @brief Encoder class realization
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

// This is an independent project of an individual developer. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "jbdrivers/Encoder.hpp"
#include <esp_pthread.h>
#include <thread>

using namespace ::jblib::jbdrivers;

Encoder::Encoder(gpio_num_t e1Pin, gpio_num_t e2Pin) : e1Pin_(e1Pin), e2Pin_(e2Pin)
{
    gpio_pad_select_gpio(this->e1Pin_);
    gpio_set_direction(this->e1Pin_, GPIO_MODE_INPUT);
    gpio_set_pull_mode(this->e1Pin_, GPIO_PULLUP_ONLY);

    GpioInterrupt::Configuration configuration;
    configuration.pin = this->e2Pin_;
    configuration.pullUp = GPIO_PULLUP_ENABLE;
    configuration.pullDown = GPIO_PULLDOWN_DISABLE;
    configuration.edge = GPIO_INTR_NEGEDGE;
    this->gpioInterrupt_ = std::unique_ptr<GpioInterrupt>(new GpioInterrupt(configuration));
    this->gpioInterrupt_->addCallback([this](void*, void*){
        this->state_ = gpio_get_level(this->e1Pin_) ? STATE_LEFT : STATE_RIGHT;
        this->gpioInterrupt_->disable();
        BaseType_t isAwake = 0;
        xSemaphoreGiveFromISR(this->filterSem_,&isAwake);
        if(isAwake) {
            portYIELD_FROM_ISR();
        }
    });

    auto cfg = esp_pthread_get_default_config();
    cfg.thread_name = "[Encoder Filter]";
    cfg.stack_size = CONFIG_ENCODER_FILTER_TASK_STACK_SIZE;
    cfg.prio = CONFIG_ENCODER_FILTER_TASK_PRIORITY;
    esp_pthread_set_cfg(&cfg);
    std::thread defenceFilterThread([this](){
        while(true){
            xSemaphoreTake(this->filterSem_,portMAX_DELAY);
            vTaskDelay(pdMS_TO_TICKS(CONFIG_ENCODER_FILTER_DELAY_MS));
            this->gpioInterrupt_->enable();
        }
    });
    defenceFilterThread.detach();
    this->gpioInterrupt_->enable();
}



Encoder::State_t Encoder::getState()
{
    return this->state_;
}



void Encoder::resetState()
{
    this->state_ = STATE_IDLE;
}
