
#ifndef PWM_H
#define PWM_H

#include "PWM_Index.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define PWM_MAX_DUTY  1000
#define PWM_MIN_DUTY  0

typedef enum hwPWM_OpResult_t
{
  hwPWM_OK = 0,
  hwPWM_InvalidParameter = -1,
  hwPWM_NotInit = -2,
  hwPWM_NotTurnOn = -3,
  hwPWM_SlaveTimeout = -4,
  hwPWM_HwError = -5,
  hwPWM_Unsupport = -6,
}hwPWM_OpResult;

typedef enum hwPWM_Step_Direction_t
{
  hwPWM_Step_Dir_Down = 0,
  hwPWM_Step_Dir_Up = 1,
  hwPWM_Step_Dir_MAX = 2
}hwPWM_Step_Direction;

bool PWM_is_Init(hwPWM_Channel channel_index);
hwPWM_OpResult PWM_Channel_Init(hwPWM_Channel channel_index, bool inverse_PWM);
hwPWM_OpResult PWM_Channel_DeInit(hwPWM_Channel channel_index);
hwPWM_OpResult PWM_Turn_On(hwPWM_Channel channel_index);
hwPWM_OpResult PWM_Turn_On_And_Set_Duty(hwPWM_Channel channel_index, uint16_t duty);
hwPWM_OpResult PWM_Turn_Off(hwPWM_Channel channel_index);
hwPWM_OpResult PWM_Set_Duty(hwPWM_Channel channel_index, uint16_t duty);
hwPWM_OpResult PWM_Step_Duty(hwPWM_Channel channel_index, uint16_t step_duty, hwPWM_Step_Direction direction);
hwPWM_OpResult PWM_Get_Channel_OnOff_Status(hwPWM_Channel channel_index, bool* onoff_status);
hwPWM_OpResult PWM_Get_Channel_Current_Duty(hwPWM_Channel channel_index, uint16_t* current_duty);
hwPWM_OpResult PWM_Get_Channel_Current_OnOff_Duty(hwPWM_Channel channel_index, bool* onoff_status, uint16_t* current_duty);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif //PWM_H