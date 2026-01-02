
#ifndef DAC_PIN_H
#define DAC_PIN_H

#ifdef STM32F207xx
#include "Pin/DAC_Pin_STM32F207xx.h"
#endif //STM32F207xx

#ifdef STM32F767xx
#include "Pin/DAC_Pin_STM32F767xx.h"
#endif //STM32F767xx

#ifdef STM32H723xx
#include "Pin/DAC_Pin_STM32H723xx.h"
#endif //STM32H723xx

#endif //DAC_PIN_H