/**
 * @file
 * @brief Void Timer class definition
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
#include "jbkernel/IVoidTimer.hpp"
#include "driver/timer.h"


namespace jblib{
    namespace jbdrivers{
        class VoidTimer : public ::jblib::jbkernel::IVoidTimer
        {
        public:
            typedef enum{
                TIMER0_GROUP0 = 0,
                TIMER1_GROUP0 = 1,
                TIMER0_GROUP1 = 2,
                TIMER1_GROUP1 = 3,
                TIMER_MAX,
            }TimerNum_t;

            static VoidTimer& getVoidTimer(TimerNum_t number, int intrFlags = ESP_INTR_FLAG_LOWMED) noexcept(false);
            VoidTimer(const VoidTimer&) = delete;
            VoidTimer& operator=(const VoidTimer&) = delete;
            void initialize(uint32_t periodUs) override;
            void initializeTicks(uint32_t periodTicks) override;
            void deinitialize() override;
            void start() override;
            void stop() override;
            void reset() override;
            uint32_t getCounter() override;
            uint32_t getUsecCounter() override;
            void setCounter(uint32_t ticks) override;
            void setUsecCounter(uint32_t us) override;
            void changePeriod(uint32_t periodUs) override;
            void changePeriodTicks(uint32_t periodTicks) override;

        private:
            static constexpr const char* logTag_ = "[ VoidTimer ]";
            static bool isrHandler(void* instance);
            explicit VoidTimer(TimerNum_t number, int intrFlags);
            timer_group_t timerGroup_ = TIMER_GROUP_0;
            timer_idx_t timerIdx_ = TIMER_0;
            int intrFlags_ = 0;
        };
    }
}
