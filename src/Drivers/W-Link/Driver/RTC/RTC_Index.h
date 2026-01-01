
#ifndef RTC_INDEX_H
#define RTC_INDEX_H

#ifdef STM32
typedef enum hwRTC_Index_t
{
  hwRTC_Index_0,
  hwRTC_Index_MAX,
}hwRTC_Index;
#define RTC_SUPPORT_ALARM
#endif //STM32

#endif //RTC_INDEX_H