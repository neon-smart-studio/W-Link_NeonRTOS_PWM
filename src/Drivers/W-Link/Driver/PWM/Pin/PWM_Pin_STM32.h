
#ifndef PWM_PIN_STM32_H
#define PWM_PIN_STM32_H

#include "GPIO/GPIO.h"

#include "Timer/Timer.h"

#include "PWM/PWM.h"

typedef enum
{
	hwPWM_Base_1 = 0,
	hwPWM_Base_2,
	hwPWM_Base_3,
	hwPWM_Base_4,
	hwPWM_Base_5,
	hwPWM_Base_8,
	hwPWM_Base_9,
	hwPWM_Base_10,
	hwPWM_Base_11,
	hwPWM_Base_12,
	hwPWM_Base_13,
	hwPWM_Base_14,
	hwPWM_Base_MAX
}hwPWM_Base_Index;

typedef struct {
    hwPWM_Channel channel;
    hwTimer_Index timer;
    hwGPIO_Pin    pin;
    uint8_t       chx;   // 1~4
    uint8_t       is_n;       // CHxN
} PWM_Pin_Def;

typedef struct {
    hwTimer_Index timer;
    hwGPIO_Pin   pin;
    uint8_t      af;
} PWM_AF_Map;

#endif //PWM_PIN_STM32_H