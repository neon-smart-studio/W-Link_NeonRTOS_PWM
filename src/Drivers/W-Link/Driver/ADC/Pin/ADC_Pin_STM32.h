
#ifndef ADC_PIN_STM32_H
#define ADC_PIN_STM32_H

#include "GPIO/GPIO.h"

#include "ADC/ADC.h"

typedef enum {
    hwADC_Instance_1 = 0,
    hwADC_Instance_2,
    hwADC_Instance_3,
    hwADC_Instance_MAX,
} hwADC_Instance;

typedef struct {
    hwGPIO_Pin adc_pin;
    hwADC_Instance inst;
} ADC_Channel_Def;

#endif //ADC_PIN_STM32_H