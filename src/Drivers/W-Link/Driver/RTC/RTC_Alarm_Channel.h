
#ifndef RTC_ALARM_CHANNEL_H
#define RTC_ALARM_CHANNEL_H

#ifdef STM32
typedef enum {
    hwRTC_Alarm_Channel_Index_0 = 0, // Alarm A
#if defined(RTC_ALARM_B)
    hwRTC_Alarm_Channel_Index_1,     // Alarm B
#endif
    hwRTC_Alarm_Channel_Index_MAX
} hwRTC_Alarm_Channel_Index;
#endif //STM32

#endif //RTC_ALARM_CHANNEL_H