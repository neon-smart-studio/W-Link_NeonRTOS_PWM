
#ifndef TIMER_INDEX_H
#define TIMER_INDEX_H

#include "soc.h"

#ifdef STM32

typedef enum hwTimer_Index_t
{
#if defined (TIM1_BASE)
  hwTimer_Index_0,
#endif
#if defined (TIM2_BASE)
  hwTimer_Index_1,
#endif
#if defined (TIM3_BASE)
  hwTimer_Index_2,
#endif
#if defined (TIM4_BASE)
  hwTimer_Index_3,
#endif
#if defined (TIM5_BASE)
  hwTimer_Index_4,
#endif
#if defined (TIM6_BASE)
  hwTimer_Index_5,
#endif
#if defined (TIM7_BASE)
  hwTimer_Index_6,
#endif
#if defined (TIM8_BASE)
  hwTimer_Index_7,
#endif
#if defined (TIM9_BASE)
  hwTimer_Index_8,
#endif
#if defined (TIM10_BASE)
  hwTimer_Index_9,
#endif
#if defined (TIM11_BASE)
  hwTimer_Index_10,
#endif
#if defined (TIM12_BASE)
  hwTimer_Index_11,
#endif
#if defined (TIM13_BASE)
  hwTimer_Index_12,
#endif
#if defined (TIM14_BASE)
  hwTimer_Index_13,
#endif
  hwTimer_Index_MAX,
}hwTimer_Index;

#endif //STM32

#ifdef DEVICE_TM4C1294
typedef enum hwTimer_Index_t
{
  hwTimer_Index_MAX,
}hwTimer_Index;
#endif //DEVICE_TM4C1294

#endif //TIMER_INDEX_H