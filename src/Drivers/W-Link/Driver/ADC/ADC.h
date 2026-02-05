
#ifndef ADC_H
#define ADC_H

#include "ADC_Def.h"

hwADC_OpStatus ADC_Channel_Init(hwADC_Channel_Index channel);
hwADC_OpStatus ADC_Channel_DeInit(hwADC_Channel_Index channel);
hwADC_OpStatus ADC_Read_MiniVolt(hwADC_Channel_Index channel, float* readMv);

#endif //ADC_H