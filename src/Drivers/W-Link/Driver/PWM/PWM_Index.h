
#ifndef PWM_INDEX_H
#define PWM_INDEX_H

#define DEFAULT_PWM_INDEX

#ifdef STM32F207xx
#undef DEFAULT_PWM_INDEX
typedef enum
{
	hwPWM_Channel_1 = 0,
	hwPWM_Channel_2,
	hwPWM_Channel_3,
	hwPWM_Channel_4,
	hwPWM_Channel_5,
	hwPWM_Channel_6,
	hwPWM_Channel_7,
	hwPWM_Channel_8,
	hwPWM_Channel_9,
	hwPWM_Channel_10,
	hwPWM_Channel_11,
	hwPWM_Channel_12,
	hwPWM_Channel_13,
	hwPWM_Channel_14,
	hwPWM_Channel_15,
	hwPWM_Channel_16,
	hwPWM_Channel_17,
	hwPWM_Channel_18,
	hwPWM_Channel_19,
	hwPWM_Channel_20,
	hwPWM_Channel_21,
	hwPWM_Channel_22,
	hwPWM_Channel_23,
	hwPWM_Channel_24,
	hwPWM_Channel_25,
	hwPWM_Channel_26,
	hwPWM_Channel_27,
	hwPWM_Channel_28,
	hwPWM_Channel_29,
	hwPWM_Channel_30,
	hwPWM_Channel_31,
	hwPWM_Channel_32,
	hwPWM_Channel_MAX
}hwPWM_Channel;
#endif //STM32F207xx

#ifdef STM32F767xx
#undef DEFAULT_PWM_INDEX
typedef enum
{
	hwPWM_Channel_1 = 0,
	hwPWM_Channel_2,
	hwPWM_Channel_3,
	hwPWM_Channel_4,
	hwPWM_Channel_5,
	hwPWM_Channel_6,
	hwPWM_Channel_7,
	hwPWM_Channel_8,
	hwPWM_Channel_9,
	hwPWM_Channel_10,
	hwPWM_Channel_11,
	hwPWM_Channel_12,
	hwPWM_Channel_13,
	hwPWM_Channel_14,
	hwPWM_Channel_15,
	hwPWM_Channel_16,
	hwPWM_Channel_17,
	hwPWM_Channel_18,
	hwPWM_Channel_19,
	hwPWM_Channel_20,
	hwPWM_Channel_21,
	hwPWM_Channel_22,
	hwPWM_Channel_23,
	hwPWM_Channel_24,
	hwPWM_Channel_25,
	hwPWM_Channel_26,
	hwPWM_Channel_27,
	hwPWM_Channel_28,
	hwPWM_Channel_29,
	hwPWM_Channel_30,
	hwPWM_Channel_31,
	hwPWM_Channel_32,
	hwPWM_Channel_MAX
}hwPWM_Channel;
#endif //STM32F767xx

#ifdef STM32H723xx
#undef DEFAULT_PWM_INDEX
typedef enum
{
	hwPWM_Channel_1 = 0,
	hwPWM_Channel_2,
	hwPWM_Channel_3,
	hwPWM_Channel_4,
	hwPWM_Channel_5,
	hwPWM_Channel_6,
	hwPWM_Channel_7,
	hwPWM_Channel_8,
	hwPWM_Channel_9,
	hwPWM_Channel_10,
	hwPWM_Channel_11,
	hwPWM_Channel_12,
	hwPWM_Channel_13,
	hwPWM_Channel_14,
	hwPWM_Channel_15,
	hwPWM_Channel_16,
	hwPWM_Channel_17,
	hwPWM_Channel_18,
	hwPWM_Channel_19,
	hwPWM_Channel_20,
	hwPWM_Channel_21,
	hwPWM_Channel_22,
	hwPWM_Channel_23,
	hwPWM_Channel_24,
	hwPWM_Channel_25,
	hwPWM_Channel_26,
	hwPWM_Channel_27,
	hwPWM_Channel_28,
	hwPWM_Channel_29,
	hwPWM_Channel_30,
	hwPWM_Channel_31,
	hwPWM_Channel_32,
	hwPWM_Channel_MAX
}hwPWM_Channel;
#endif //STM32H723xx

#ifdef DEVICE_TM4C1294
#undef DEFAULT_PWM_INDEX
typedef enum
{
	hwPWM_Channel_1 = 0,
	hwPWM_Channel_2,
	hwPWM_Channel_3,
	hwPWM_Channel_4,
	hwPWM_Channel_5,
	hwPWM_Channel_6,
	hwPWM_Channel_7,
	hwPWM_Channel_8,
  	hwPWM_Channel_MAX,
}hwPWM_Channel;
#endif //DEVICE_TM4C1294

#ifdef DEFAULT_PWM_INDEX
typedef enum
{
	hwPWM_Channel_1 = 0,
  hwPWM_Channel_MAX,
}hwPWM_Channel;
#endif //DEFAULT_PWM_INDEX

#endif //PWM_INDEX_H