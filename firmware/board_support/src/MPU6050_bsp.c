/**
 * @file MPU6050_bsp.c
 * @author Federico Andriollo
 * @brief BSP driver for the MPU6050 sensor.
 * @version 0.1
 * @date 2026-07-20
 *
 * @copyright Copyright (c) 2026
 *
 */

/*==================[inclusions]=============================================*/
//#include "template_bsp.h"
#include "MPU6050_registers.h"
#include "MPU6050_bsp.h"
#include "i2c_hal.h"
/* TODO: Include the HAL headers needed to access hardware resources.
 * BSP drivers must only use HAL functions, never ESP-IDF APIs directly.
 * Examples:
 *   #include "gpio_hal.h"
 *   #include "uart_hal.h"
 *   #include "spi_hal.h"
 */
/*==================[macros and definitions]=================================*/
/* TODO: Define the GPIO pins or peripheral instances used by this component.
 * Map the component's physical pins to HAL identifiers here.
 * Example:
 *   #define SENSOR_GPIO_DATA    GPIO_3
 *   #define SENSOR_GPIO_ENABLE  GPIO_4
 */

/*==================[internal data declaration]==============================*/
static i2c_hal_device_t mpu6050_device;
static bool initialized = false;
/*==================[internal functions declaration]=========================*/
static bool Mpu6050WakeUp(void);
static bool Mpu6050WriteAccelRange(mpu6050_accel_range_t range);
static bool Mpu6050WriteGyroRange(mpu6050_gyro_range_t range);
/*==================[external data declaration]==============================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
static bool Mpu6050WakeUp(void)
{
	/* Limpia el bit SLEEP del registro PWR_MGMT_1 */
	uint8_t value = MPU6050_PWR_WAKE_UP;
	return I2CHalWrite(mpu6050_device, MPU6050_REG_PWR_MGMT_1, I2C_HAL_REG_ADDR_8_BITS, &value, 1);
}
static bool Mpu6050WriteAccelRange(mpu6050_accel_range_t range)
{
    uint8_t value;
    switch (range)
    {
        case MPU6050_ACCEL_RANGE_2G:
            value = MPU6050_ACCEL_2G;
            break;

        case MPU6050_ACCEL_RANGE_4G:
            value = MPU6050_ACCEL_4G;
            break;

        case MPU6050_ACCEL_RANGE_8G:
            value = MPU6050_ACCEL_8G;
            break;

        case MPU6050_ACCEL_RANGE_16G:
            value = MPU6050_ACCEL_16G;
            break;

        default:
            return false;
    }

    return I2CHalWrite(mpu6050_device, MPU6050_REG_ACCEL_CONFIG, I2C_HAL_REG_ADDR_8_BITS, &value, 1);
}

static bool Mpu6050WriteGyroRange(mpu6050_gyro_range_t range)
{
    uint8_t value;
    switch (range)
    {
        case MPU6050_GYRO_RANGE_250DPS:
            value = MPU6050_GYRO_250DPS;
            break;

        case MPU6050_GYRO_RANGE_500DPS:
            value = MPU6050_GYRO_500DPS;
            break;

        case MPU6050_GYRO_RANGE_1000DPS:
            value = MPU6050_GYRO_1000DPS;
            break;

        case MPU6050_GYRO_RANGE_2000DPS:
            value = MPU6050_GYRO_2000DPS;
            break;

        default:
            return false;
    }

    return I2CHalWrite(mpu6050_device, MPU6050_REG_GYRO_CONFIG, I2C_HAL_REG_ADDR_8_BITS, &value, 1);
}

/*==================[external functions definition]==========================*/
bool Mpu6050Init(void)
{
    uint8_t id;
    if (initialized)
    {
        return true;
    }
    /* Inicializa el periférico I2C */
    if (!I2CHalInit())
		{
			return false;
		}

    /* Registra el MPU6050 como un dispositivo del bus I2C */
    if (!I2CHalAddDevice(MPU6050_ADDRESS, MPU6050_SPEED, &mpu6050_device))
		{
			return false;
		}

    /* Verifica la presencia del dispositivo leyendo el registro WHO_AM_I */
    if (!I2CHalRead(mpu6050_device, MPU6050_REG_WHO_AM_I, I2C_HAL_REG_ADDR_8_BITS, &id, 1))
		{
			return false;
		}
    /* Comprueba que el dispositivo encontrado sea un MPU6050 */
    if (id != MPU6050_WHO_AM_I_VALUE)
		{
			return false;
		}
    /* Sale del modo Sleep */
    if (!Mpu6050WakeUp())
		{
            
			return false;
		}

    /* Configura el acelerómetro en ±2 g */
	if(!Mpu6050WriteAccelRange(MPU6050_ACCEL_RANGE_2G))
		{
            
			return false;
		}
    /* Configura el giroscopio en ±250 °/s */
    if (!Mpu6050WriteGyroRange(MPU6050_GYRO_RANGE_250DPS))
		{
            
			return false;
		}
    /* El driver ya posee un dispositivo válido */
    initialized = true;
    return true;
}



bool Mpu6050SetAccelRange(mpu6050_accel_range_t range)
{   
    if (!initialized)
    {
        return false;
    }
    return Mpu6050WriteAccelRange(range);
}

bool Mpu6050SetGyroRange(mpu6050_gyro_range_t range)
{
    if (!initialized)
    {
        return false;
    }
    return Mpu6050WriteGyroRange(range);
}   
   
bool Mpu6050ReadAccel(mpu6050_accel_t *acc)
{
    if (!initialized || acc==NULL)
    {
        return false;
    }
    uint8_t buffer[6];
    if (!I2CHalRead(mpu6050_device, MPU6050_REG_ACCEL_XOUT_H, I2C_HAL_REG_ADDR_8_BITS, buffer, 6))
    {
        return false;
    }

    acc->x = (int16_t)((buffer[0] << 8) | buffer[1]);
    acc->y = (int16_t)((buffer[2] << 8) | buffer[3]);
    acc->z = (int16_t)((buffer[4] << 8) | buffer[5]);

    return true;
}

bool Mpu6050ReadGyro(mpu6050_gyro_t *gyro)
{
    if (!initialized || gyro == NULL)
    {
        return false;
    }
    uint8_t buffer[6];
    if (!I2CHalRead(mpu6050_device, MPU6050_REG_GYRO_XOUT_H, I2C_HAL_REG_ADDR_8_BITS, buffer, 6))
    {
        return false;
    }
    gyro->x = (int16_t)((buffer[0] << 8) | buffer[1]);
    gyro->y = (int16_t)((buffer[2] << 8) | buffer[3]);
    gyro->z = (int16_t)((buffer[4] << 8) | buffer[5]);

    return true;
}

bool Mpu6050ReadTemperature(float *temp)
{
    if (!initialized || temp == NULL)
    {
        return false;
    }
    uint8_t buffer[2];
    if (!I2CHalRead(mpu6050_device, MPU6050_REG_TEMP_OUT_H, I2C_HAL_REG_ADDR_8_BITS, buffer, 2))
    {
        return false;
    }
    int16_t raw_temp = (int16_t)((buffer[0] << 8) | buffer[1]);
    *temp = (raw_temp / 340.0f) + 36.53f;

    return true;    
}

bool Mpu6050ReadAll( mpu6050_data_t *data)
{
    if (!initialized || data == NULL)
    {
        return false;
    }
    uint8_t buffer[14];
    if (!I2CHalRead(mpu6050_device, MPU6050_REG_ACCEL_XOUT_H, I2C_HAL_REG_ADDR_8_BITS, buffer, 14))
    {
        return false;
    }

    data->accel.x = (int16_t)((buffer[0] << 8) | buffer[1]);
    data->accel.y = (int16_t)((buffer[2] << 8) | buffer[3]);
    data->accel.z = (int16_t)((buffer[4] << 8) | buffer[5]);

    int16_t raw_temp = (int16_t)((buffer[6] << 8) | buffer[7]);
    data->temperature = (raw_temp / 340.0f) + 36.53f;

    data->gyro.x = (int16_t)((buffer[8] << 8) | buffer[9]);
    data->gyro.y = (int16_t)((buffer[10] << 8) | buffer[11]);
    data->gyro.z = (int16_t)((buffer[12] << 8) | buffer[13]);

    return true;    
}

bool Mpu6050IsPresent(void)
{
    if (!initialized)
    {
        return false;
    }
    uint8_t id;
    if (!I2CHalRead(mpu6050_device, MPU6050_REG_WHO_AM_I, I2C_HAL_REG_ADDR_8_BITS, &id, 1))
    {
        return false;
    }
    return (id == MPU6050_WHO_AM_I_VALUE);    
}

bool Mpu6050Reset(void)
{
    if (!initialized)
    {
        return false;
    }

    if (!Mpu6050WritePwrMgmt1(MPU6050_PWR_MGMT_1_RESET))
    {
        return false;
    }

    initialized = false;

    //return Mpu6050Init();
}

/*==================[end of file]============================================*/
