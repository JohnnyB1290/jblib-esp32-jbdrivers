/**
 * @file
 * @brief SPI Master class realization
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

#include "jbdrivers/SpiMaster.hpp"
#include <stdexcept>
#include "esp_intr.h"

using namespace ::jblib::jbdrivers;

SpiMaster::SpiMaster(const spi_bus_config_t& configuration, spi_host_device_t number, bool useDma) : number_(number)
{
    auto ret = spi_bus_initialize(number, &configuration, useDma? number : 0);
    if(ret != ESP_OK){
        throw std::logic_error("Bus initialize error");
    }
}



spi_bus_config_t SpiMaster::Device::getBusConfiguration()
{
    spi_bus_config_t configuration{};
    configuration.mosi_io_num = 13;
    configuration.miso_io_num = 12;
    configuration.sclk_io_num = 14;
    configuration.quadwp_io_num = -1;
    configuration.quadhd_io_num = -1;
    configuration.max_transfer_sz = 0;
    configuration.flags = SPICOMMON_BUSFLAG_MASTER;
    configuration.intr_flags = ESP_INTR_FLAG_LOWMED;
    return configuration;
}



void SpiMaster::addDevice(Device& device)
{
    auto configuration = getDeviceSpiConfiguration(device);
    spi_device_handle_t handle;
    auto ret = spi_bus_add_device(this->number_, &configuration, &handle);
    if(ret != ESP_OK){
        throw std::logic_error("Add device to bus error");
    }
    setDeviceHandle(device, handle);
    this->devicesList_.push_front(handle);
}



void SpiMaster::removeDevice(const Device& device)
{
    spi_device_handle_t handle = getDeviceHandle(device);
    if(handle){
        this->devicesList_.remove(handle);
        spi_bus_remove_device(handle);
    }
}



SpiMaster::~SpiMaster()
{
    for(auto handle : this->devicesList_){
        spi_bus_remove_device(handle);
    }
    spi_bus_free(this->number_);
}
