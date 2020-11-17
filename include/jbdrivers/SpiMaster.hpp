/**
 * @file
 * @brief SPI Master class definition
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
#include "driver/spi_master.h"
#include <forward_list>

namespace jblib {
    namespace jbdrivers {

        class SpiMaster
        {
        public:
            class Device;

        private:

            static spi_device_handle_t getDeviceHandle(const Device& device)
            {
                return device.handle;
            }

            static void setDeviceHandle(Device& device, spi_device_handle_t handle)
            {
                device.handle = handle;
            }

            static spi_device_interface_config_t getDeviceSpiConfiguration(const Device& device)
            {
                return device.deviceConfiguration_;
            }

        public:
            class Device
            {
            public:
                Device(const Device&) = delete;
                Device& operator=(const Device&) = delete;
                virtual spi_bus_config_t getBusConfiguration();
            protected:
                Device() = default;
                virtual ~Device() = default;
                spi_device_interface_config_t deviceConfiguration_{};
                spi_device_handle_t handle = nullptr;
            private:
                friend spi_device_handle_t SpiMaster::getDeviceHandle(const Device& device);
                friend void SpiMaster::setDeviceHandle(Device& device, spi_device_handle_t handle);
                friend spi_device_interface_config_t SpiMaster::getDeviceSpiConfiguration(const Device& device);
            };

            explicit SpiMaster(const spi_bus_config_t& configuration,
                    spi_host_device_t number = HSPI_HOST, bool useDma = false) noexcept(false);
            virtual ~SpiMaster();
            SpiMaster(const SpiMaster&) = delete;
            SpiMaster& operator=(const SpiMaster&) = delete;
            void addDevice(Device& device) noexcept(false);
            void removeDevice(const Device& device);

        private:
            static constexpr const char* logTag_ = "[ Spi Master ]";
            spi_host_device_t number_ = HSPI_HOST;
            std::forward_list<spi_device_handle_t> devicesList_;
        };
    }
}