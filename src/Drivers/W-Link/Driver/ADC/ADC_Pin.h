
#ifndef ADC_PIN_H
#define ADC_PIN_H

#ifdef STM32F207xx
#include "Pin/ADC_Pin_STM32F207xx.h"
#endif //STM32F207xx

#ifdef STM32F767xx
#include "Pin/ADC_Pin_STM32F767xx.h"
#endif //STM32F767xx

#ifdef STM32H723xx
#include "Pin/ADC_Pin_STM32H723xx.h"
#endif //STM32H723xx

#endif //ADC_PIN_H