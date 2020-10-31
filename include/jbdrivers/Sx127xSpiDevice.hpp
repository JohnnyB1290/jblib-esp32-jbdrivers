/**
 * @file
 * @brief SX127X Spi Device class definition
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
#include "jbdrivers/SpiMaster.hpp"
#include "esp_intr_alloc.h"

namespace jblib {
    namespace jbdrivers {

        static constexpr uint16_t MAX_WRITE_CHECKS = 256;
        class Sx127xSpiDevice : public SpiMaster::Device
        {
        public:
            struct Configuration
            {
                int misoPin = -1;
                int mosiPin = -1;
                int clkPin = -1;
                int csPin = -1;
                uint32_t intrFlags = ESP_INTR_FLAG_LOWMED; //priority
                int clockSpeedHz = SPI_MASTER_FREQ_8M;
                bool useInterruptMode  = false;
                uint16_t maxWriteChecks = MAX_WRITE_CHECKS;
            };

            explicit Sx127xSpiDevice(const Configuration& config);
            ~Sx127xSpiDevice() override = default;
            spi_bus_config_t getBusConfiguration() final;
            uint8_t read(uint8_t address);
            uint8_t write(uint8_t address, uint8_t data, bool waitUntilSet = false); //returns old value of register
            void write(uint8_t startAddress, const uint8_t* data, size_t size); //max value size is 4096 if using DMA, 64 if not

        private:
            static constexpr const char* logTag_ = "[ SX127x Spi Dev ]";
            Configuration config_;
            esp_err_t makeTransaction(spi_transaction_t& transaction);
        };

    }
}
