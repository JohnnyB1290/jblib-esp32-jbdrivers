/**
 * @file
 * @brief SX127X Spi Device class realization
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

#include "jbdrivers/Sx127xSpiDevice.hpp"

using namespace ::jblib::jbdrivers;

Sx127xSpiDevice::Sx127xSpiDevice(const Configuration& config) : SpiMaster::Device(), config_(config)
{
    this->deviceConfiguration_.command_bits = 8;
    this->deviceConfiguration_.address_bits = 0;
    this->deviceConfiguration_.dummy_bits = 0;
    this->deviceConfiguration_.mode = 0;        // CPOL = 0, CPHA = 0
    this->deviceConfiguration_.duty_cycle_pos = 0;
    this->deviceConfiguration_.cs_ena_pretrans = 0;
    this->deviceConfiguration_.cs_ena_posttrans = 0;
    this->deviceConfiguration_.clock_speed_hz = this->config_.clockSpeedHz;
    this->deviceConfiguration_.input_delay_ns = 0;
    this->deviceConfiguration_.spics_io_num = this->config_.csPin;
    this->deviceConfiguration_.flags = 0;
    this->deviceConfiguration_.queue_size = 1;
    this->deviceConfiguration_.pre_cb = nullptr;
    this->deviceConfiguration_.post_cb = nullptr;
}



spi_bus_config_t Sx127xSpiDevice::getBusConfiguration()
{
    auto configuration = SpiMaster::Device::getBusConfiguration();
    configuration.mosi_io_num = this->config_.mosiPin;
    configuration.miso_io_num = this->config_.misoPin;
    configuration.sclk_io_num = this->config_.clkPin;
    configuration.intr_flags = this->config_.intrFlags;
    return configuration;
}



esp_err_t Sx127xSpiDevice::makeTransaction(spi_transaction_t& transaction)
{
    return this->config_.useInterruptMode ?
           spi_device_transmit(this->handle, &transaction):
           spi_device_polling_transmit(this->handle, &transaction);
}



uint8_t Sx127xSpiDevice::read(uint8_t address)
{
    spi_transaction_t transaction{};
    transaction.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
    transaction.cmd = (address & static_cast<uint8_t>(0x7f));
    transaction.length = 8;  //< Total data length, in bits
    esp_err_t ret = this->makeTransaction(transaction);
    if(ret != ESP_OK){
        ESP_LOGE(logTag_, "Transaction error %i", ret);
    }
    return transaction.rx_data[0];
}



uint8_t Sx127xSpiDevice::write(uint8_t address, uint8_t data, bool waitUntilSet)
{
    spi_transaction_t transaction{};
    transaction.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
    transaction.cmd = static_cast<uint8_t>(128) | static_cast<uint8_t>((address & static_cast<uint8_t>(0x7f)));
    transaction.length = 8; //< Total data length, in bits
    transaction.tx_data[0] = data;
    esp_err_t ret = this->makeTransaction(transaction);
    if(ret != ESP_OK){
        ESP_LOGE(logTag_, "Transaction error %i", ret);
    }
    if(waitUntilSet){
        for(size_t i = 0; i < this->config_.maxWriteChecks; i++){
            if(this->read(address) == data){
                break;
            }
        }
    }
    return transaction.rx_data[0];
}



void Sx127xSpiDevice::write(uint8_t startAddress, const uint8_t* data, size_t size)
{
    spi_transaction_t transaction{};
    transaction.cmd = static_cast<uint8_t>(128) | static_cast<uint8_t>((startAddress & static_cast<uint8_t>(0x7f)));
    transaction.length = 8 * size;  //< Total data length, in bits
    transaction.tx_buffer = data;
    esp_err_t ret = this->makeTransaction(transaction);
    if(ret != ESP_OK){
        ESP_LOGE(logTag_, "Transaction error %i", ret);
    }
}
