
#ifndef ADC_DEF_H
#define ADC_DEF_H

#include "ADC_Channel.h"

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

#endif //ADC_DEF_H