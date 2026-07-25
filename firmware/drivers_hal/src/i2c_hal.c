/**
 * @file I2C_hal.c
 * @author Federico Andriollo
 * @brief HAL driver template for ESP32-C6.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright Copyright (c) 2026
 *
 */
/**
 * @file i2c_hal.c
 * @brief Hardware Abstraction Layer for the I2C peripheral.
 *
 * This module provides a hardware-independent interface for I2C
 * communication with external devices.
 *
 * The implementation uses the ESP-IDF I2C master driver internally,
 * while exposing only HAL-level types and functions to upper layers.
 *
 * This is the only layer that directly accesses the ESP-IDF I2C driver.
 *
 * @author Federico Andriollo
 */
/*==================[inclusions]=============================================*/
#include "i2c_hal.h"
#include "driver/i2c_master.h"
#include "i2c_hal_config.h"
#include <stdbool.h>
#include <string.h>

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
/**
 * @brief Handle for the I2C master bus.
 */
static i2c_master_bus_handle_t bus_handle = NULL;

/**
 * @brief Handles for registered I2C devices.
 *
 * Each entry corresponds to a device registered through I2CHalAddDevice().
 */
static i2c_master_dev_handle_t dev_handles[MAX_I2C_DEVICES];

/**
 * @brief Number of devices currently registered on the I2C bus.
 */
static uint8_t device_count = 0;

/**
 * @brief Indicates whether the I2C HAL has been initialized.
 */
static bool initialized = false;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
/**
 * @brief Initializes the I2C master bus.
 *
 * Configures and initializes the I2C master bus using the hardware
 * parameters defined in the HAL configuration.
 *
 * If the bus is already initialized, the function does not perform
 * the initialization again and returns true.
 *
 * @return true if the I2C bus was initialized successfully.
 * @return false if the I2C bus initialization failed.
 */
bool I2CHalInit(void) 
{
    esp_err_t err;
    i2c_master_bus_config_t i2c_master_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = HAL_I2C_PORT,
        .scl_io_num = HAL_I2C_SCL_PIN,
        .sda_io_num = HAL_I2C_SDA_PIN,
        .glitch_ignore_cnt = HAL_I2C_GLITCH_IGNORE_CNT,
        .flags.enable_internal_pullup = true,
        };
    if (initialized)
        {
            return true;
        }
    err=i2c_new_master_bus(&i2c_master_config, &bus_handle);
    if (err != ESP_OK)
        {
            return false;
        }
    initialized = true;
    return true;
    }

/**
 * @brief Registers an I2C device on the master bus.
 *
 * Adds a new device to the initialized I2C bus and assigns it an
 * internal device identifier that can be used by the other HAL
 * communication functions.
 *
 * The device is configured using a 7-bit I2C address and the
 * specified bus speed.
 *
 * @param address 7-bit I2C device address.
 * @param speed_hz I2C communication speed in Hz.
 * @param device Pointer where the assigned device identifier is stored.
 *
 * @return true if the device was successfully registered.
 * @return false if the HAL is not initialized, the device pointer is NULL,
 *         the maximum number of devices has been reached, or the device
 *         registration failed.
 */
bool I2CHalAddDevice( uint8_t address, uint32_t speed_hz, i2c_hal_device_t *device)
{
    esp_err_t err;
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = address, 
        .scl_speed_hz = speed_hz, 
        };
    if (!initialized||device==NULL)
        {   
        return false;
        }
    if(device_count>=HAL_I2C_MAX_DEVICES)
        {
        return false;
        }
    err=i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handles[device_count]);
    if (err != ESP_OK)
        {
            return false;                                   
        }
    *device=device_count;
    device_count++;
    return true;
}

/**
 * @brief Reads data from an I2C device register.
 *
 * Writes the register address to the selected I2C device and then
 * reads the requested number of bytes.
 *
 * The register address can be configured as either 8-bit or 16-bit.
 * For 16-bit addresses, the most significant byte is transmitted first.
 *
 * @param device Internal HAL identifier of the I2C device.
 * @param register_address Register address to read from.
 * @param addr_size Register address size, either 8-bit or 16-bit.
 * @param data Pointer to the buffer where the received data is stored.
 * @param length Number of bytes to read.
 *
 * @return true if the read transaction completed successfully.
 * @return false if the parameters are invalid or the I2C transaction failed.
 */
bool I2CHalRead( i2c_hal_device_t device, uint16_t register_address, i2c_hal_reg_addr_size_t addr_size, uint8_t *data, uint16_t length)
{
    if (!initialized || data==NULL || device>=device_count)
        return false;
    if(length == 0)
        return false;
    if(length > HAL_I2C_MAX_TRANSFER)
        return false;
    uint8_t register_buffer[2];
    uint8_t register_length;
    esp_err_t err;
    switch (addr_size)
    {
        case I2C_HAL_REG_ADDR_8_BITS:
            register_buffer[0] = (uint8_t)register_address;
            register_length = 1;
            break;

        case I2C_HAL_REG_ADDR_16_BITS:
            register_buffer[0] = (uint8_t)(register_address >> 8);   // MSB
            register_buffer[1] = (uint8_t)(register_address);        // LSB
            register_length = 2;
            break;

        default:
        return false;
}
    err=i2c_master_transmit_receive( dev_handles[device], register_buffer, register_length, data, length, HAL_I2C_TIMEOUT_MS);
    if (err != ESP_OK)
        {
            return false;
        }
    return true;
}

/**
 * @brief Writes data to an I2C device register.
 *
 * Sends the register address followed by the data bytes to the
 * selected I2C device.
 *
 * The register address can be configured as either 8-bit or 16-bit.
 * For 16-bit addresses, the most significant byte is transmitted first.
 *
 * @param device Internal HAL identifier of the I2C device.
 * @param register_address Register address to write to.
 * @param addr_size Register address size, either 8-bit or 16-bit.
 * @param data Pointer to the data to be transmitted.
 * @param length Number of bytes to write.
 *
 * @return true if the write transaction completed successfully.
 * @return false if the parameters are invalid or the I2C transaction failed.
 */
bool I2CHalWrite( i2c_hal_device_t device, uint16_t register_address, i2c_hal_reg_addr_size_t addr_size, const uint8_t *data, uint16_t length)
{
    if(length > HAL_I2C_MAX_TRANSFER)
        {
        return false;
        }   
    if (!initialized || data==NULL || device>=device_count)  
        {
        return false;
        }
    if(length == 0)
        {
        return false;
        }
    uint8_t register_length = (addr_size == I2C_HAL_REG_ADDR_8_BITS) ? 1 : 2;
    uint8_t tx_buffer[length + register_length];    
    esp_err_t err;
    switch (addr_size)
    {
        case I2C_HAL_REG_ADDR_8_BITS:
            tx_buffer[0] = (uint8_t)register_address;
            memcpy(&tx_buffer[1], data, length);
            break;

        case I2C_HAL_REG_ADDR_16_BITS:
            tx_buffer[0] = (uint8_t)(register_address >> 8);
            tx_buffer[1] = (uint8_t)(register_address);
            memcpy(&tx_buffer[2], data, length);
            break;

        default:
            return false;
    }
    err = i2c_master_transmit( dev_handles[device], tx_buffer, length + register_length, HAL_I2C_TIMEOUT_MS);
    if (err != ESP_OK)
        {
            return false;
        }
    return true;
}
/*==================[end of file]============================================*/


