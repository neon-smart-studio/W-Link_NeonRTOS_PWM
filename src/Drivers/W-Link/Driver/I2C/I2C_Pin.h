
#ifndef I2C_PIN_H
#define I2C_PIN_H

#ifdef STM32F207xx
#include "Pin/I2C_Pin_STM32F207xx.h"
#endif //STM32F207xx

#ifdef STM32F767xx
#include "Pin/I2C_Pin_STM32F767xx.h"
#endif //STM32F767xx

#ifdef STM32H723xx
#include "Pin/I2C_Pin_STM32H723xx.h"
#endif //STM32H723xx

#ifdef DEVICE_TM4C1294
#include "Pin/I2C_Pin_TM4C1294.h"
#endif //DEVICE_TM4C1294

#endif //I2C_PIN_H