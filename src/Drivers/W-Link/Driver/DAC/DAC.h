
#ifndef DAC_H
#define DAC_H

#include "DAC_Channel.h"

#if defined(DEVICE_RTL8720DN)
#define DAC_MS_MAX_MINI_VOLTS            3300
#endif //DEVICE_RTL8720DN

typedef enum hwDAC_OpStatus_t
{
  hwDAC_OK = 0,
  hwDAC_NotInit = -1,
  hwDAC_InvalidParameter = -2,
  hwDAC_HwError = -3,
  hwDAC_MemoryError = -4,
  hwDAC_MutexTimeout = -5,
  hwDAC_HW_Error = -6,
  hwDAC_Unsupport = -7
} hwDAC_OpStatus;

hwDAC_OpStatus DAC_Channel_Init(hwDAC_Channel_Index channel);
hwDAC_OpStatus DAC_Channel_DeInit(hwDAC_Channel_Index channel);
hwDAC_OpStatus DAC_Read_MiniVolt(hwDAC_Channel_Index channel, float* readMv);

#endif //DAC_H