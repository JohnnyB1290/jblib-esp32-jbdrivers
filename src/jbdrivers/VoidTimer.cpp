/**
 * @file
 * @brief Void Timer class realization
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

#include "jbdrivers/VoidTimer.hpp"
#include <iostream>

using namespace ::jblib::jbkernel;
using namespace ::jblib::jbdrivers;

VoidTimer& VoidTimer::getVoidTimer(TimerNum_t number, int intrFlags)
{
    static VoidTimer* voidTimers[TIMER_MAX]{};
    if(number >= TIMER_MAX){
        throw std::invalid_argument("Wrong timer number");
    }
    if(!voidTimers[number]){
        voidTimers[number] = new VoidTimer(number, intrFlags);
        if(!voidTimers[number]){
            throw std::bad_alloc();
        }
    }
    return *voidTimers[number];
}



VoidTimer::VoidTimer(TimerNum_t number, int intrFlags) : IVoidTimer(), intrFlags_(intrFlags)
{
    this->timerGroup_ = static_cast<timer_group_t>(static_cast<uint32_t>(number) & 2U);
    this->timerIdx_ = static_cast<timer_idx_t>(static_cast<uint32_t>(number) & 1U);
}



void VoidTimer::initialize(uint32_t periodUs)
{
    this->initializeTicks(TIMER_BASE_CLK / 2000000 * periodUs);
}



void VoidTimer::initializeTicks(uint32_t periodTicks)
{
    timer_config_t config{};
    config.divider = 2,
    config.counter_dir = TIMER_COUNT_UP,
    config.counter_en = TIMER_PAUSE,
    config.alarm_en = TIMER_ALARM_EN,
    config.auto_reload = TIMER_AUTORELOAD_EN,
    config.intr_type = TIMER_INTR_LEVEL,

    timer_init(this->timerGroup_, this->timerIdx_, &config);
    timer_set_counter_value(this->timerGroup_, this->timerIdx_, 0ULL);
    timer_set_alarm_value(this->timerGroup_, this->timerIdx_, periodTicks);
    timer_enable_intr(this->timerGroup_, this->timerIdx_);
    timer_isr_callback_add(this->timerGroup_, this->timerIdx_, VoidTimer::isrHandler,
            this, this->intrFlags_);
}



bool IRAM_ATTR VoidTimer::isrHandler(void* instance)
{
    auto timer = reinterpret_cast<VoidTimer*>(instance);
    timer->setCounter(0);
    if(timer->callback_){
        timer->callback_->voidCallback(timer, timer->callbackParameter_);
    }
    return true;
}



void VoidTimer::deinitialize()
{
    this->stop();
    timer_isr_callback_remove(this->timerGroup_, this->timerIdx_);
    timer_deinit(this->timerGroup_, this->timerIdx_);
}



void VoidTimer::start()
{
    timer_start(this->timerGroup_, this->timerIdx_);
}



void VoidTimer::stop()
{
    timer_pause(this->timerGroup_, this->timerIdx_);
}



void VoidTimer::reset()
{
    this->setCounter(0);
}



uint32_t VoidTimer::getCounter()
{
    uint64_t ret = 0;
    timer_get_counter_value(this->timerGroup_, this->timerIdx_, &ret);
    return ret;
}



uint32_t VoidTimer::getUsecCounter()
{
    return static_cast<uint64_t>(this->getCounter()) * 2000000 / TIMER_BASE_CLK;
}



void VoidTimer::setCounter(uint32_t ticks)
{
    timer_set_counter_value(this->timerGroup_, this->timerIdx_, ticks);
}



void VoidTimer::setUsecCounter(uint32_t us)
{
    this->setCounter(TIMER_BASE_CLK / 2000000 * us);
}



void VoidTimer::changePeriod(uint32_t periodUs)
{
    this->changePeriodTicks(TIMER_BASE_CLK / 2000000 * periodUs);
}



void VoidTimer::changePeriodTicks(uint32_t periodTicks)
{
    timer_set_alarm_value(this->timerGroup_, this->timerIdx_, periodTicks);
}



void VoidTimer::addCallback(::jblib::jbkernel::IVoidCallback* callback)
{
    this->callback_ = callback;
    this->callbackParameter_ = nullptr;
}



void VoidTimer::addCallback(::jblib::jbkernel::IVoidCallback* callback, void* parameter)
{
    this->callback_ = callback;
    this->callbackParameter_ = parameter;
}



void VoidTimer::deleteCallback()
{
    this->callback_ = nullptr;
    this->callbackParameter_ = nullptr;
}
