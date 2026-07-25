#ifndef I2C_HAL_H
#define I2C_HAL_H

/** @defgroup I2C HAL
 *  @brief Capa de abstraccion de hardware.
 *  @{
 */

/** \brief Hardware Abstraction Layer para el periférico I2C.
 *
 * El módulo i2c_hal proporciona una interfaz independiente del microcontrolador para 
 * inicializar el periférico I2C y realizar operaciones genéricas de lectura y escritura 
 * sobre dispositivos conectados al bus.
 * 
 *
 * @author Federico Andriollo.
 *
 * @section changelog
 *
 * |   Date     | Description                                            |
 * |:----------:|:-------------------------------------------------------|
 * | 14/07/2026 | Document creation                                      |
 *
 **/

/*==================[inclusions]=============================================*/
#include <stdbool.h>
#include <stdint.h>
#include "i2c_hal_config.h"
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
typedef uint8_t i2c_hal_device_t;
typedef enum
{
    I2C_HAL_REG_ADDR_8_BITS = 1,
    I2C_HAL_REG_ADDR_16_BITS =2
} i2c_hal_reg_addr_size_t;
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief inicializa el periférico I2C.
 *
 * @return true si la inicialización fue exitosa.
 * @return false en caso de error.
 */
bool I2CHalInit(void);

/**
 * @brief Escribe datos en un dispositivo I2C.
 *
 * @param register_address Registro inicial.
 * @param data Buffer con los datos a transmitir.
 * @param length Cantidad de bytes.
 *
 * @return true si la operación fue exitosa.
 * @return false en caso de error.
 */
bool I2CHalWrite( uint8_t register_address, const uint8_t *data, uint16_t length);

/**
 * @brief Lee datos desde un dispositivo I2C.
 *
 * @param register_address Registro inicial.
 * @param data Buffer donde almacenar los datos.
 * @param length Cantidad de bytes.
 *
 * @return true si la operación fue exitosa.
 * @return false en caso de error.
 */
bool I2CHalRead( i2c_hal_device_t device, uint16_t register_address, i2c_hal_reg_addr_size_t addr_size, uint8_t *data, uint16_t length)

/**
 * @brief registra un dispositivo en I2C.
 *
 * @param address Dirección I2C del dispositivo.
 * @param speed_hz Velocidad del bus I2C.
 * @return true si el registro fue exitoso.
 * @return false en caso de error.
 *
 */
bool I2CHalAddDevice( uint8_t address, uint32_t speed_hz, i2c_hal_device_t *device); //Nota: para mantener la independencia del microcontrolador, esta funcion debe ser llamada desde board layer

#endif /* #ifndef I2C_HAL_H */

/*==================[end of file]============================================*/