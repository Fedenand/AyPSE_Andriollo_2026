#ifndef MPU6050_BSP_H
#define MPU6050_BSP_H

/** @file mpu6050_bsp.h
 *  @brief BSP driver for the MPU6050 inertial measurement unit.
 *
 * This driver provides a hardware abstraction for the MPU6050 sensor.
 * It allows initialization of the device, configuration of the
 * accelerometer and gyroscope measurement ranges, and acquisition of
 * acceleration, angular velocity and temperature data through the HAL
 * I2C interface.
 *
 * @note Hardware connection:
 *
 * | MPU6050 Pin | ESP32-C6 |
 * |:-----------:|:--------:|
 * | SDA         | GPIO 5   |
 * | SCL         | GPIO 6   |
 * | VCC         | 3.3 V    |
 * | GND         | GND      |
 *
 * @author Federico Andriollo
 */
 *
 * @section changelog
 *
 * |   Date     | Description                                            |
 * |:----------:|:-------------------------------------------------------|
 * | 20/07/2023 | Document creation                                      |
 *
 **/

/*==================[inclusions]=============================================*/
#include <stdbool.h>
#include <stdint.h>
#include "MPU6050_registers.h"
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
/**
 * @brief Available accelerometer full-scale ranges.
 */
typedef enum
{
    MPU6050_ACCEL_RANGE_2G = 0,
    MPU6050_ACCEL_RANGE_4G,
    MPU6050_ACCEL_RANGE_8G,
    MPU6050_ACCEL_RANGE_16G
} mpu6050_accel_range_t;

/**
 * @brief Available gyroscope full-scale ranges.
 */
typedef enum
{
    MPU6050_GYRO_RANGE_250DPS = 0,
    MPU6050_GYRO_RANGE_500DPS,
    MPU6050_GYRO_RANGE_1000DPS,
    MPU6050_GYRO_RANGE_2000DPS
} mpu6050_gyro_range_t;

/**
 * @brief Raw accelerometer measurement.
 *
 * Values are returned directly from the sensor registers.
 */
typedef struct
{
    int16_t x;      /**< X-axis acceleration. */
    int16_t y;      /**< Y-axis acceleration. */
    int16_t z;      /**< Z-axis acceleration. */
} mpu6050_accel_t;

/**
 * @brief Raw gyroscope measurement.
 *
 * Values are returned directly from the sensor registers.
 */
typedef struct
{
    int16_t x;      /**< X-axis angular velocity. */
    int16_t y;      /**< Y-axis angular velocity. */
    int16_t z;      /**< Z-axis angular velocity. */
} mpu6050_gyro_t;

/**
 * @brief Complete measurement returned by the MPU6050.
 */
typedef struct
{
    mpu6050_accel_t accel;  /**< Accelerometer measurement. */
    mpu6050_gyro_t gyro;    /**< Gyroscope measurement. */
    float temperature;      /**< Internal temperature in degrees Celsius. */
} mpu6050_data_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief Initializes the MPU6050 sensor.
 *
 * Initializes the I2C interface, verifies that the device is present,
 * wakes it up from sleep mode and configures the default measurement
 * ranges for the accelerometer and gyroscope.
 *
 * @return true if the initialization completed successfully.
 * @return false if the sensor could not be initialized.
 */
bool Mpu6050Init(void); 

/**
 * @brief Sets the accelerometer full-scale range.
 *
 * @param range Desired measurement range.
 *
 * @return true if the configuration was written successfully.
 * @return false otherwise.
 */
bool Mpu6050SetAccelRange(mpu6050_accel_range_t range); 

/**
 * @brief Sets the gyroscope full-scale range.
 *
 * @param range Desired measurement range.
 *
 * @return true if the configuration was written successfully.
 * @return false otherwise.
 */
bool Mpu6050SetGyroRange(mpu6050_gyro_range_t range); 

/**
 * @brief Reads the raw accelerometer values.
 *
 * @param acc Pointer where the measurement will be stored.
 *
 * @return true if the read operation succeeded.
 * @return false otherwise.
 */
bool Mpu6050ReadAccel(mpu6050_accel_t *acc); 

/**
 * @brief Reads the raw gyroscope values.
 *
 * @param gyro Pointer where the measurement will be stored.
 *
 * @return true if the read operation succeeded.
 * @return false otherwise.
 */
bool Mpu6050ReadGyro(mpu6050_gyro_t *gyro); 

/**
 * @brief Reads the internal temperature sensor.
 *
 * @param temp Pointer where the temperature in degrees Celsius will be stored.
 *
 * @return true if the read operation succeeded.
 * @return false otherwise.
 */
bool Mpu6050ReadTemperature(float *temp); 

/**
 * @brief Reads all sensor measurements.
 *
 * Performs a single I2C transaction to obtain the accelerometer,
 * gyroscope and temperature measurements.
 *
 * @param data Pointer where the measurements will be stored.
 *
 * @return true if the read operation succeeded.
 * @return false otherwise.
 */
bool Mpu6050ReadAll( mpu6050_data_t *data);

/**
 * @brief Checks whether the MPU6050 is present on the I2C bus.
 *
 * Reads the WHO_AM_I register to verify that the connected device
 * matches an MPU6050.
 *
 * @return true if the device is detected.
 * @return false otherwise.
 */
bool Mpu6050IsPresent(void);

/**
 * @brief Resets the MPU6050 to its default configuration.
 *
 * Performs a device reset using the PWR_MGMT_1 register.
 *
 * @return true if the reset command was accepted.
 * @return false otherwise.
 */
bool Mpu6050Reset(void);

#endif /* MPU6050_BSP_H */

/*==================[end of file]============================================*/
