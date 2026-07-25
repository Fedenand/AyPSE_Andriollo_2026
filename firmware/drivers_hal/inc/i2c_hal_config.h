#pragma once

#define HAL_I2C_PORT         I2C_NUM_0      // Controlador de hardware 0 o 1
#define HAL_I2C_SDA_PIN      5             // Pin físico SDA en ESP32
#define HAL_I2C_SCL_PIN      6             // Pin físico SCL en ESP32
#define HAL_I2C_SPEED_HZ     400000U         // 400 kHz (Fast Mode para MPU6050)
#define HAL_I2C_GLITCH_IGNORE_CNT   7
#define HAL_I2C_MAX_DEVICES    8
#define HAL_I2C_TIMEOUT_MS    100
#define HAL_I2C_MAX_TRANSFER    64
