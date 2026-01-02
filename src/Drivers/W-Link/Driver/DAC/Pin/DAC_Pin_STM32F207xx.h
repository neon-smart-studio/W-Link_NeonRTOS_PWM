
#ifndef DAC_PIN_STM32F207xx_H
#define DAC_PIN_STM32F207xx_H

#include "DAC_Pin_STM32.h"

const DAC_Channel_Def DAC_Channel_Def_Table[hwDAC_Channel_Index_MAX] = {
    /* ===== DAC1 ===== */
    { hwGPIO_Pin_A4, hwDAC_Instance_1 },
    { hwGPIO_Pin_A5, hwDAC_Instance_1 },
};

#endif //DAC_PIN_STM32F207xx_H