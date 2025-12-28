
#ifndef TIMER_INDEX_H
#define TIMER_INDEX_H

#ifdef STM32F2
typedef enum hwTimer_Index_t
{
  hwTimer_Index_0,
  hwTimer_Index_1,
  hwTimer_Index_2,
  hwTimer_Index_3,
  hwTimer_Index_4,
  hwTimer_Index_5,
  hwTimer_Index_6,
  hwTimer_Index_7,
  hwTimer_Index_8,
  hwTimer_Index_9,
  hwTimer_Index_10,
  hwTimer_Index_11,
  hwTimer_Index_12,
  hwTimer_Index_13,
  hwTimer_Index_MAX,
}hwTimer_Index;
#endif //STM32F2

#ifdef STM32F7
typedef enum hwTimer_Index_t
{
  hwTimer_Index_0,
  hwTimer_Index_1,
  hwTimer_Index_2,
  hwTimer_Index_3,
  hwTimer_Index_4,
  hwTimer_Index_5,
  hwTimer_Index_6,
  hwTimer_Index_7,
  hwTimer_Index_8,
  hwTimer_Index_9,
  hwTimer_Index_10,
  hwTimer_Index_11,
  hwTimer_Index_12,
  hwTimer_Index_13,
  hwTimer_Index_MAX,
}hwTimer_Index;
#endif //STM32F7

#ifdef STM32H7
typedef enum hwTimer_Index_t
{
  hwTimer_Index_0,
  hwTimer_Index_1,
  hwTimer_Index_2,
  hwTimer_Index_3,
  hwTimer_Index_4,
  hwTimer_Index_5,
  hwTimer_Index_6,
  hwTimer_Index_7,
  hwTimer_Index_8,
  hwTimer_Index_9,
  hwTimer_Index_10,
  hwTimer_Index_11,
  hwTimer_Index_12,
  hwTimer_Index_13,
  hwTimer_Index_MAX,
}hwTimer_Index;
#endif //STM32H7

#ifdef DEVICE_TM4C1294
typedef enum hwTimer_Index_t
{
  hwTimer_Index_MAX,
}hwTimer_Index;
#endif //DEVICE_TM4C1294

#endif //TIMER_INDEX_H