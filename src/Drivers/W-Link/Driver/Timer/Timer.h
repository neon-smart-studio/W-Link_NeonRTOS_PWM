
#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>

#include "soc.h"

#include "Timer_Index.h"

typedef enum hwTimer_OpMode_t
{
  hwTimer_Mode_NotInit = -1,
  hwTimer_Mode_Idle = 0,
  hwTimer_Mode_OneShout = 1,
  hwTimer_Mode_Period = 2,
}hwTimer_OpMode;

typedef enum hwTimer_OpResult_t
{
  hwTimer_OK = 0,
  hwTimer_NotInit = -1,
  hwTimer_InvalidParameter = -2,
  hwTimer_HwError = -3,
  hwTimer_MemoryError = -4,
  hwTimer_MutexTimeout = -5,
  hwTimer_Unsupport = -6,
}hwTimer_OpResult;

typedef void(* onTimerEventHandler)(hwTimer_Index index);

bool Timer_is_Init(hwTimer_Index hw_index);
hwTimer_OpResult Timer_Init(hwTimer_Index hw_index);
hwTimer_OpResult Timer_DeInit(hwTimer_Index hw_index);
hwTimer_OpResult Timer_Start_OneShout(hwTimer_Index hw_index, uint32_t duration_us, onTimerEventHandler timer_exp_cb);
hwTimer_OpResult Timer_Start_Period(hwTimer_Index hw_index, uint32_t duration_us, onTimerEventHandler timer_exp_cb);
hwTimer_OpResult Timer_Reload(hwTimer_Index hw_index, uint32_t duration_us);
hwTimer_OpResult Timer_Stop(hwTimer_Index hw_index);
hwTimer_OpResult Timer_Read_Ticks(hwTimer_Index hw_index, uint32_t* ticks);
hwTimer_OpResult Timer_Read_uSec(hwTimer_Index hw_index, uint32_t* uSec);

#endif //TIMER_H