
#ifndef ADC_PIN_STM32F767xx_H
#define ADC_PIN_STM32F767xx_H

#include "ADC_Pin_STM32.h"

const ADC_Channel_Def ADC_Channel_Def_Table[hwADC_Channel_Index_MAX] = {
    /* ===== ADC1 ===== */
    { hwGPIO_Pin_A0, hwADC_Instance_1 },
    { hwGPIO_Pin_A1, hwADC_Instance_1 },
    { hwGPIO_Pin_A2, hwADC_Instance_1 },
    { hwGPIO_Pin_A3, hwADC_Instance_1 },
    { hwGPIO_Pin_A4, hwADC_Instance_1 },
    { hwGPIO_Pin_A5, hwADC_Instance_1 },

    /* ===== ADC2 ===== */
    { hwGPIO_Pin_A6, hwADC_Instance_2 },
    { hwGPIO_Pin_A7, hwADC_Instance_2 },
    { hwGPIO_Pin_B0, hwADC_Instance_2 },
    { hwGPIO_Pin_B1, hwADC_Instance_2 },
    { hwGPIO_Pin_C0, hwADC_Instance_2 },
    { hwGPIO_Pin_C1, hwADC_Instance_2 },

    /* ===== ADC3 ===== */
    { hwGPIO_Pin_C2, hwADC_Instance_3 },
    { hwGPIO_Pin_C3, hwADC_Instance_3 },
    { hwGPIO_Pin_C4, hwADC_Instance_3 },
    { hwGPIO_Pin_C5, hwADC_Instance_3 },
};

#endif //ADC_PIN_STM32F767xx_H