
#ifndef PWM_PIN_H
#define PWM_PIN_H

#include "GPIO/GPIO.h"

#ifdef STM32F207xx
#include "Pin/PWM_Pin_STM32F207xx.h"
#endif // STM32F207xx

#ifdef STM32F767xx
#include "Pin/PWM_Pin_STM32F767xx.h"
#endif //STM32H723xx

#ifdef STM32H723xx
#include "Pin/PWM_Pin_STM32H723xx.h"
#endif //STM32H723xx

#ifdef DEVICE_TM4C1294
#include "Pin/PWM_Pin_TM4C1294.h"
#endif //DEVICE_TM4C1294

#endif //PWM_PIN_H