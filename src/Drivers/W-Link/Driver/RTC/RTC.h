
#ifndef RTC_H
#define RTC_H

#include <time.h>

#include "RTC_Index.h"

#ifdef RTC_SUPPORT_ALARM
#include "RTC_Alarm_Channel.h"
#endif //RTC_SUPPORT_ALARM

#define SECONDS_FROM_1970_TO_2000 946684800

typedef enum hwRTC_OpResult_t
{
  hwRTC_OK = 0,
  hwRTC_NotInit = -1,
  hwRTC_InvalidParameter = -2,
  hwRTC_HwError = -3,
  hwRTC_MemoryError = -4,
  hwRTC_MutexTimeout = -5,
  hwRTC_Unsupport = -6,
}hwRTC_OpResult;

hwRTC_OpResult RTC_Timer_Init(hwRTC_Index hw_index);
hwRTC_OpResult RTC_Timer_DeInit(hwRTC_Index hw_index);
hwRTC_OpResult RTC_Timer_Read(hwRTC_Index hw_index, time_t* unix_time);
hwRTC_OpResult RTC_Timer_Write(hwRTC_Index hw_index, time_t unix_time);

#ifdef RTC_SUPPORT_ALARM
typedef void(* onAlarmEventCallback)(hwRTC_Index index, hwRTC_Alarm_Channel_Index alarm_ch_index);
hwRTC_OpResult RTC_Timer_Set_Alarm(hwRTC_Index hw_index, hwRTC_Alarm_Channel_Index alarm_ch_index, time_t alarm_unix_time, onAlarmEventCallback alarm_handler);
hwRTC_OpResult RTC_Timer_Clear_Alarm(hwRTC_Index hw_index, hwRTC_Alarm_Channel_Index alarm_ch_index);
#endif //RTC_SUPPORT_ALARM

#endif //RTC_H