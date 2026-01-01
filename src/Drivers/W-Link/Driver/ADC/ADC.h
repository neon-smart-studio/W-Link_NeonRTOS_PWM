
#ifndef ADC_H
#define ADC_H

#include "ADC_Channel.h"

#if defined(DEVICE_RTL8720DN)
#define ADC_MS_MAX_MINI_VOLTS            3300
#endif //DEVICE_RTL8720DN

typedef enum hwADC_OpStatus_t
{
  hwADC_OK = 0,
  hwADC_NotInit = -1,
  hwADC_InvalidParameter = -2,
  hwADC_HwError = -3,
  hwADC_MemoryError = -4,
  hwADC_MutexTimeout = -5,
  hwADC_HW_Error = -6,
  hwADC_Unsupport = -7
} hwADC_OpStatus;

hwADC_OpStatus ADC_Channel_Init(hwADC_Channel_Index channel);
hwADC_OpStatus ADC_Channel_DeInit(hwADC_Channel_Index channel);
hwADC_OpStatus ADC_Read_MiniVolt(hwADC_Channel_Index channel, float* readMv);

#endif //ADC_H