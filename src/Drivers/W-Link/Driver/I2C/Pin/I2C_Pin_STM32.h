
#ifndef I2C_PIN_STM32_H
#define I2C_PIN_STM32_H

#include "GPIO/GPIO.h"

#include "I2C/I2C_Master.h"

typedef struct {
    hwGPIO_Pin scl_pin;
    hwGPIO_Pin sda_pin;
} I2C_Pin_Def;

typedef struct {
    hwI2C_Index i2c;
    hwGPIO_Pin pin;
    uint32_t af;
} I2C_AF_Map;

#endif //I2C_PIN_STM32_H