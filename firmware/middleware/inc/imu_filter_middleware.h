#ifndef IMU_FILTER_MIDDLEWARE_H
#define IMU_FILTER_MIDDLEWARE_H

/** @defgroup middleware Middleware
 *  @brief Capa de middleware para procesamiento de datos.
 *  @{
 */

/**
 * @file imu_filter_middleware.h
 * @brief Middleware for processing MPU6050 measurements.
 *
 * This module converts raw MPU6050 measurements into physical units
 * and applies the selected filtering algorithm to the accelerometer
 * and gyroscope data.
 *
 * The middleware is independent of the hardware platform and uses
 * the BSP interface to obtain raw sensor measurements.
 *
 * Physical units used by this module:
 * - Acceleration: m/s²
 * - Angular velocity: °/s
 * - Orientation angles: °
 *
 * Raw sensor measurements remain available through the MPU6050 BSP
 * interface when required by the application.
 *
 * @author Federico Andriollo
 */

/*==================[inclusions]=============================================*/
#include <stdbool.h>
#include <stdint.h>

#include "mpu6050_bsp.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/**
 * @brief Processed IMU measurement.
 *
 * Contains accelerometer and gyroscope measurements converted to
 * physical units, together with the estimated orientation angles.
 */
typedef struct
{
    float accel_x;      /**< Acceleration along X axis in m/s². */
    float accel_y;      /**< Acceleration along Y axis in m/s². */
    float accel_z;      /**< Acceleration along Z axis in m/s². */

    float gyro_x;       /**< Angular velocity around X axis in °/s. */
    float gyro_y;       /**< Angular velocity around Y axis in °/s. */
    float gyro_z;       /**< Angular velocity around Z axis in °/s. */

    float temperature;  /**< Temperature in °C. */

    float roll;         /**< Estimated roll angle in degrees. */
    float pitch;        /**< Estimated pitch angle in degrees. */
} imu_data_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/**
 * @brief Initializes the IMU filtering module.
 *
 * Initializes the internal state required by the filtering algorithm.
 * This function must be called before processing measurements.
 */
void ImuFilterInit(void);

/**
 * @brief Processes a raw MPU6050 measurement.
 *
 * Converts the raw accelerometer and gyroscope measurements into
 * physical units and applies the configured filtering algorithm.
 *
 * @param raw Pointer to the raw MPU6050 measurement.
 * @param filtered Pointer where the processed measurement will be stored.
 *
 * @return true if the measurement was processed successfully.
 * @return false if the module is not initialized or an invalid pointer
 *         was provided.
 */
bool ImuFilterUpdate(const mpu6050_data_t *raw, imu_data_t *filtered);

/**
 * @brief Gets the current roll angle.
 *
 * @return Current roll angle in degrees.
 */
float ImuGetRoll(void);

/**
 * @brief Gets the current pitch angle.
 *
 * @return Current pitch angle in degrees.
 */

float ImuGetPitch(void);

#endif /* IMU_FILTER_MIDDLEWARE_H */

/** @} */

/*==================[end of file]============================================*/

