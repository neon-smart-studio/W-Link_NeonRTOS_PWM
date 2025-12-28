
#ifndef GPIO_PIN_H
#define GPIO_PIN_H

#include "soc.h"

#ifdef STM32
#include "GPIO_Pin_STM32.h"
#endif

#ifdef DEVICE_TM4C1294
#include "GPIO_Pin_TM4C1294.h"
#endif

#endif //GPIO_PIN_H