```c
/**
 * @file imu_filter_middleware.c
 * @author Federico Andriollo
 * @brief Middleware for MPU6050 measurement processing.
 * @version 0.1
 * @date AAAA-MM-DD
 *
 * @copyright Copyright (c) AAAA
 *
 */

/*==================[inclusions]=============================================*/
#include "imu_filter_middleware.h"
#include <math.h>
#include <stdbool.h>

/*==================[macros and definitions]=================================*/

/**
 * @brief MPU6050 accelerometer sensitivity for ±2 g range.
 *
 * The MPU6050 provides 16384 LSB per g when configured to ±2 g.
 */
#define IMU_ACCEL_SENSITIVITY      16384.0f

/**
 * @brief MPU6050 gyroscope sensitivity for ±250 °/s range.
 *
 * The MPU6050 provides 131 LSB per °/s when configured to ±250 °/s.
 */
#define IMU_GYRO_SENSITIVITY       131.0f

/**
 * @brief Gravitational acceleration in m/s².
 */
#define IMU_GRAVITY                9.80665f

/**
 * @brief Sampling period used by the complementary filter.
 *
 * This value must match the period at which ImuFilterUpdate()
 * is called.
 */
#define IMU_FILTER_SAMPLE_TIME_S   0.01f

/**
 * @brief Complementary filter coefficient.
 *
 * A value close to 1 gives more weight to the gyroscope,
 * while a lower value gives more weight to the accelerometer.
 */
#define IMU_FILTER_ALPHA           0.98f

/*==================[internal data declaration]==============================*/

/**
 * @brief Internal state of the complementary filter.
 */
static float roll = 0.0f;
static float pitch = 0.0f;
static bool initialized = false;

/*==================[internal functions declaration]=========================*/

static float ImuConvertAccelToMs2(int16_t raw);
static float ImuConvertGyroToDps(int16_t raw);
static float ImuCalculateAccelRoll(const mpu6050_accel_t *accel);
static float ImuCalculateAccelPitch(const mpu6050_accel_t *accel);

/*==================[external data declaration]==============================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/**
 * @brief Converts raw accelerometer data into m/s².
 *
 * @param raw Raw accelerometer value.
 *
 * @return Acceleration in m/s².
 */
static float ImuConvertAccelToMs2(int16_t raw)
{
    return ((float)raw / IMU_ACCEL_SENSITIVITY) * IMU_GRAVITY;
}

/**
 * @brief Converts raw gyroscope data into °/s.
 *
 * @param raw Raw gyroscope value.
 *
 * @return Angular velocity in °/s.
 */
static float ImuConvertGyroToDps(int16_t raw)
{
    return (float)raw / IMU_GYRO_SENSITIVITY;
}

/**
 * @brief Calculates roll from accelerometer measurements.
 *
 * @param accel Pointer to accelerometer measurement.
 *
 * @return Roll angle in degrees.
 */
static float ImuCalculateAccelRoll(const mpu6050_accel_t *accel)
{
    return atan2f(
        (float)accel->y,
        (float)accel->z
    ) * 180.0f / (float)M_PI;
}

/**
 * @brief Calculates pitch from accelerometer measurements.
 *
 * @param accel Pointer to accelerometer measurement.
 *
 * @return Pitch angle in degrees.
 */
static float ImuCalculateAccelPitch(const mpu6050_accel_t *accel)
{
    return atan2f(
        -(float)accel->x,
        sqrtf(
            ((float)accel->y * (float)accel->y) +
            ((float)accel->z * (float)accel->z)
        )
    ) * 180.0f / (float)M_PI;
}

/*==================[external functions definition]==========================*/

void ImuFilterInit(void)
{
    roll = 0.0f;
    pitch = 0.0f;
    initialized = true;
}

bool ImuFilterUpdate(const mpu6050_data_t *raw, imu_data_t *filtered)
{
    float accel_roll;
    float accel_pitch;
    float gyro_roll;
    float gyro_pitch;

    if (!initialized || raw == NULL || filtered == NULL)
    {
        return false;
    }

    /* Convert accelerometer measurements to m/s² */
    filtered->accel_x = ImuConvertAccelToMs2(raw->accel.x);
    filtered->accel_y = ImuConvertAccelToMs2(raw->accel.y);
    filtered->accel_z = ImuConvertAccelToMs2(raw->accel.z);

    /* Convert gyroscope measurements to °/s */
    filtered->gyro_x = ImuConvertGyroToDps(raw->gyro.x);
    filtered->gyro_y = ImuConvertGyroToDps(raw->gyro.y);
    filtered->gyro_z = ImuConvertGyroToDps(raw->gyro.z);

    /* Temperature is already converted to °C by the BSP */
    filtered->temperature = raw->temperature;

    /* Calculate orientation from accelerometer */
    accel_roll = ImuCalculateAccelRoll(&raw->accel);
    accel_pitch = ImuCalculateAccelPitch(&raw->accel);

    /* Integrate gyroscope measurements */
    gyro_roll = roll + filtered->gyro_x * IMU_FILTER_SAMPLE_TIME_S;
    gyro_pitch = pitch + filtered->gyro_y * IMU_FILTER_SAMPLE_TIME_S;

    /* Complementary filter */
    roll = IMU_FILTER_ALPHA * gyro_roll
         + (1.0f - IMU_FILTER_ALPHA) * accel_roll;

    pitch = IMU_FILTER_ALPHA * gyro_pitch
          + (1.0f - IMU_FILTER_ALPHA) * accel_pitch;

    /* Store filtered orientation */
    filtered->roll = roll;
    filtered->pitch = pitch;

    return true;
}

float ImuGetRoll(void)
{
    return roll;
}

float ImuGetPitch(void)
{
    return pitch;
}

/*==================[end of file]============================================*/
```

