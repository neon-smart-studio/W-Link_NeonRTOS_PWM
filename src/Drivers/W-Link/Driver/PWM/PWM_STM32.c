
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "soc.h"

#include "NeonRTOS.h"

#include "Timer/Timer.h"

#include "PWM.h"

#ifdef STM32

#include "PWM_Pin.h"

#include "GPIO/GPIO_STM32.h"

#include "Timer/Timer_STM32.h"

#define PWM_PERIOD_US    1000

static bool PWM_Channel_Init_Status[hwPWM_Channel_MAX] = {false};
static bool PWM_Channel_OnOff_Status[hwPWM_Channel_MAX] = {false};
static bool PWM_Channel_Inverse_Status[hwPWM_Channel_MAX] = {false};
static uint16_t PWM_Channel_Current_Duty[hwPWM_Channel_MAX] = {0};

uint8_t STM32_PWM_GetAF(hwTimer_Index timer, hwGPIO_Pin pin)
{
    for (size_t i = 0; i < sizeof(PWM_Pin_AF_Map)/sizeof(PWM_Pin_AF_Map[0]); i++) {
        if (PWM_Pin_AF_Map[i].timer == timer &&
            PWM_Pin_AF_Map[i].pin == pin) {
            return PWM_Pin_AF_Map[i].af;
        }
    }
    return 0;
}

static const PWM_Pin_Def* PWM_Find_PinDef(hwPWM_Channel channel_index)
{
    for (int i = 0; i < hwPWM_Base_MAX; i++) {
        PWM_Pinset_t pinset = PWM_Index_Map_Alt[i];

        for (int j = 0; j < 4; j++) {
            const PWM_Pin_Def *def = &PWM_Pin_Def_Table[i][pinset][j];

            if (def->channel == channel_index)
                return def;
        }
    }
    return NULL;
}

static inline uint32_t PWM_Duty_To_CCR(
    uint16_t duty,
    uint32_t period,
    bool inverse)
{
    float ratio = (float)duty / PWM_MAX_DUTY;
    if (inverse) ratio = 1.0f - ratio;
    return (uint32_t)(ratio * period);
}

hwPWM_OpResult PWM_Channel_Init(hwPWM_Channel channel_index, bool inverse)
{
        if (channel_index >= hwPWM_Channel_MAX)
        {
                return hwPWM_InvalidParameter;
        }

        if (PWM_Channel_Init_Status[channel_index])
        {
                return hwPWM_OK;
        }

        const PWM_Pin_Def *def = PWM_Find_PinDef(channel_index);
        if (!def || def->pin == hwGPIO_Pin_NC)
        {
                return hwPWM_InvalidParameter;
        }

        if(Timer_is_Init(def->timer))
        {
                return hwPWM_HwError;
        }

        uint8_t af = STM32_PWM_GetAF(def->timer, def->pin);
        if (af == 0)
        {
                return hwPWM_InvalidParameter;
        }

        GPIO_TypeDef * pwm_soc_base = GPIO_Map_Soc_Base(def->pin);
        uint16_t pwm_soc_pin = GPIO_Map_Soc_Pin(def->pin);
        if (pwm_soc_base == NULL || pwm_soc_pin == 0)
        {
                return hwPWM_InvalidParameter;
        }

        GPIO_Enable_RCC_Clock(pwm_soc_base);

        switch(def->timer)
        {
                case hwTimer_Index_0:
                        __HAL_RCC_TIM1_CLK_ENABLE();
                        break;
                case hwTimer_Index_1:
                        __HAL_RCC_TIM2_CLK_ENABLE();
                        break;
                case hwTimer_Index_2:
                        __HAL_RCC_TIM3_CLK_ENABLE();
                        break;
                case hwTimer_Index_3:
                        __HAL_RCC_TIM4_CLK_ENABLE();
                        break;
                case hwTimer_Index_4:
                        __HAL_RCC_TIM5_CLK_ENABLE();
                        break;
                case hwTimer_Index_7:
                        __HAL_RCC_TIM8_CLK_ENABLE();
                        break;
                case hwTimer_Index_8:
                        __HAL_RCC_TIM9_CLK_ENABLE();
                        break;
                case hwTimer_Index_9:
                        __HAL_RCC_TIM10_CLK_ENABLE();
                        break;
                case hwTimer_Index_10:
                        __HAL_RCC_TIM11_CLK_ENABLE();
                        break;
                case hwTimer_Index_11:
                        __HAL_RCC_TIM12_CLK_ENABLE();
                        break;
                case hwTimer_Index_12:
                        __HAL_RCC_TIM13_CLK_ENABLE();
                        break;
                case hwTimer_Index_13:
                        __HAL_RCC_TIM14_CLK_ENABLE();
                        break;
        }

        GPIO_InitTypeDef gpio = {0};
        gpio.Mode      = GPIO_MODE_AF_PP;
        gpio.Pull      = GPIO_NOPULL;
        gpio.Speed     = GPIO_SPEED_FREQ_HIGH;
        gpio.Alternate = af;
        gpio.Pin = pwm_soc_pin;

        HAL_GPIO_Init(pwm_soc_base, &gpio);

        /* HAL init */
        if (HAL_TIM_PWM_Init(&g_timer[def->timer]) != HAL_OK)
        {
                HAL_GPIO_DeInit(pwm_soc_base, pwm_soc_pin);
                return hwPWM_HwError;
        }

        PWM_Channel_Init_Status[channel_index]    = true;
        PWM_Channel_Inverse_Status[channel_index] = inverse;
        PWM_Channel_Current_Duty[channel_index]   = 0;

        gpio_pin_init_status[def->pin] = true;

        return hwPWM_OK;
}

hwPWM_OpResult PWM_Channel_DeInit(hwPWM_Channel channel_index)
{
        if(channel_index>=hwPWM_Channel_MAX)
        {
                return hwPWM_InvalidParameter;
        }
        
        if(PWM_Channel_Init_Status[channel_index]==false)
        {
                return hwPWM_OK;
        }
        
        const PWM_Pin_Def *def = PWM_Find_PinDef(channel_index);
        if (!def || def->pin == hwGPIO_Pin_NC)
        {
                return hwPWM_InvalidParameter;
        }

        if(Timer_is_Init(def->timer))
        {
                return hwPWM_HwError;
        }

        GPIO_TypeDef * pwm_soc_base = GPIO_Map_Soc_Base(def->pin);
        uint16_t pwm_soc_pin = GPIO_Map_Soc_Pin(def->pin);
        if (pwm_soc_base == NULL || pwm_soc_pin == 0)
        {
                return hwPWM_InvalidParameter;
        }

        HAL_TIM_PWM_Stop(&g_timer[def->timer], def->chx);
	
        if (HAL_TIM_PWM_DeInit(&g_timer[def->timer]) != HAL_OK)
        {
                return hwPWM_HwError;
        }

        HAL_GPIO_DeInit(pwm_soc_base, pwm_soc_pin);

        PWM_Channel_Init_Status[channel_index] = false;
        PWM_Channel_OnOff_Status[channel_index] = false;
        PWM_Channel_Inverse_Status[channel_index] = false;
        
        bool all_off;

        switch(def->timer)
        {
                case hwTimer_Index_0:
                        all_off = true;
                        for(hwPWM_Channel i = hwPWM_Channel_1; i <= hwPWM_Channel_4; i++)
                        {
                                {
                                        if(PWM_Channel_Init_Status[i])
                                        {
                                            all_off = false;    
                                        }
                                }
                        }
                        if(all_off)
                        {
                                __HAL_RCC_TIM1_CLK_DISABLE();
                        }
                        break;
                case hwTimer_Index_1:
                        all_off = true;
                        for(hwPWM_Channel i = hwPWM_Channel_5; i <= hwPWM_Channel_8; i++)
                        {
                                {
                                        if(PWM_Channel_Init_Status[i])
                                        {
                                            all_off = false;    
                                        }
                                }
                        }
                        if(all_off)
                        {
                                __HAL_RCC_TIM2_CLK_DISABLE();
                        }
                        break;
                case hwTimer_Index_2:
                        all_off = true;
                        for(hwPWM_Channel i = hwPWM_Channel_9; i <= hwPWM_Channel_12; i++)
                        {
                                {
                                        if(PWM_Channel_Init_Status[i])
                                        {
                                            all_off = false;    
                                        }
                                }
                        }
                        if(all_off)
                        {
                                __HAL_RCC_TIM3_CLK_DISABLE();
                        }
                        break;
                case hwTimer_Index_3:
                        all_off = true;
                        for(hwPWM_Channel i = hwPWM_Channel_13; i <= hwPWM_Channel_16; i++)
                        {
                                {
                                        if(PWM_Channel_Init_Status[i])
                                        {
                                            all_off = false;    
                                        }
                                }
                        }
                        if(all_off)
                        {
                                __HAL_RCC_TIM4_CLK_DISABLE();
                        }
                        break;
                case hwTimer_Index_4:
                        all_off = true;
                        for(hwPWM_Channel i = hwPWM_Channel_17; i <= hwPWM_Channel_20; i++)
                        {
                                {
                                        if(PWM_Channel_Init_Status[i])
                                        {
                                            all_off = false;    
                                        }
                                }
                        }
                        if(all_off)
                        {
                                __HAL_RCC_TIM5_CLK_DISABLE();
                        }
                        break;
                case hwTimer_Index_7: //
                        all_off = true;
                        for(hwPWM_Channel i = hwPWM_Channel_21; i <= hwPWM_Channel_24; i++)
                        {
                                {
                                        if(PWM_Channel_Init_Status[i])
                                        {
                                            all_off = false;    
                                        }
                                }
                        }
                        if(all_off)
                        {
                                __HAL_RCC_TIM8_CLK_DISABLE();
                        }
                        break;
                case hwTimer_Index_8:
                        all_off = true;
                        for(hwPWM_Channel i = hwPWM_Channel_25; i <= hwPWM_Channel_26; i++)
                        {
                                {
                                        if(PWM_Channel_Init_Status[i])
                                        {
                                            all_off = false;    
                                        }
                                }
                        }
                        if(all_off)
                        {
                                __HAL_RCC_TIM9_CLK_DISABLE();
                        }
                        break;
                case hwTimer_Index_9:
                        all_off = true;
                        if(PWM_Channel_Init_Status[hwPWM_Channel_27])
                        {
                                all_off = false;  
                        }
                        if(all_off)
                        {
                                __HAL_RCC_TIM10_CLK_DISABLE();
                        }
                        break;
                case hwTimer_Index_10:
                        all_off = true;
                        if(PWM_Channel_Init_Status[hwPWM_Channel_28])
                        {
                                all_off = false;  
                        }
                        if(all_off)
                        {
                                __HAL_RCC_TIM12_CLK_DISABLE();
                        }
                        break;
                case hwTimer_Index_11:
                        all_off = true;
                        for(hwPWM_Channel i = hwPWM_Channel_29; i <= hwPWM_Channel_30; i++)
                        {
                                {
                                        if(PWM_Channel_Init_Status[i])
                                        {
                                            all_off = false;    
                                        }
                                }
                        }
                        if(all_off)
                        {
                                __HAL_RCC_TIM12_CLK_DISABLE();
                        }
                        break;
                case hwTimer_Index_12:
                        all_off = true;
                        if(PWM_Channel_Init_Status[hwPWM_Channel_31])
                        {
                                all_off = false;  
                        }
                        if(all_off)
                        {
                                __HAL_RCC_TIM13_CLK_DISABLE();
                        }
                        break;
                case hwTimer_Index_13:
                        all_off = true;
                        if(PWM_Channel_Init_Status[hwPWM_Channel_32])
                        {
                                all_off = false;  
                        }
                        if(all_off)
                        {
                                __HAL_RCC_TIM14_CLK_ENABLE();
                        }
                        break;
        }

        gpio_pin_init_status[def->pin] = false;

        return hwPWM_OK;
}

hwPWM_OpResult PWM_Turn_On(hwPWM_Channel channel_index)
{
        if (channel_index >= hwPWM_Channel_MAX)
        {
                return hwPWM_InvalidParameter;
        }

        /* 必須先 Init */
        if (PWM_Channel_Init_Status[channel_index] == false)
        {
                return hwPWM_NotInit;
        }

        const PWM_Pin_Def *def = PWM_Find_PinDef(channel_index);
        if (!def || def->pin == hwGPIO_Pin_NC)
        {
                return hwPWM_InvalidParameter;
        }

        /* Timer 必須已初始化 */
        if (!Timer_is_Init(def->timer))
        {
                return hwPWM_NotInit;
        }

        /* duty clamp */
        uint16_t duty = PWM_Channel_Current_Duty[channel_index];
        if (duty > PWM_MAX_DUTY)
        {
                duty = PWM_MAX_DUTY;
                PWM_Channel_Current_Duty[channel_index] = duty;
        }

        /* 計算 CCR */
        uint32_t period = g_timer[def->timer].Init.Period;
        uint32_t pulse;

        if (PWM_Channel_Inverse_Status[channel_index])
        {
                pulse = period - ((uint32_t)duty * period) / PWM_MAX_DUTY;
        }
        else
        {
                pulse = ((uint32_t)duty * period) / PWM_MAX_DUTY;
        }

        TIM_OC_InitTypeDef oc = {0};
        oc.OCMode     = TIM_OCMODE_PWM1;
        oc.Pulse      = pulse;
        oc.OCPolarity = TIM_OCPOLARITY_HIGH;
        oc.OCFastMode = TIM_OCFAST_DISABLE;

        if (HAL_TIM_PWM_ConfigChannel(&g_timer[def->timer], &oc, def->chx) != HAL_OK)
        {
                return hwPWM_HwError;
        }

        if (HAL_TIM_PWM_Start(&g_timer[def->timer], def->chx) != HAL_OK)
        {
                return hwPWM_HwError;
        }

        PWM_Channel_OnOff_Status[channel_index] = true;
        return hwPWM_OK;
}

hwPWM_OpResult PWM_Turn_On_And_Set_Duty(hwPWM_Channel channel_index, uint16_t duty)
{
        if(channel_index>=hwPWM_Channel_MAX)
        {
                return hwPWM_InvalidParameter;
        }
        
        if(duty>PWM_MAX_DUTY)
        {
                return hwPWM_InvalidParameter;
        }

        if(PWM_Channel_Init_Status[channel_index]==false)
        {
                return hwPWM_OK;
        }
        
        const PWM_Pin_Def *def = PWM_Find_PinDef(channel_index);
        if (!def || def->pin == hwGPIO_Pin_NC)
        {
                return hwPWM_InvalidParameter;
        }

        /* Timer 必須已初始化 */
        if (!Timer_is_Init(def->timer))
        {
                return hwPWM_NotInit;
        }

        /* 計算 CCR */
        uint32_t period = g_timer[def->timer].Init.Period;
        uint32_t pulse;

        if (PWM_Channel_Inverse_Status[channel_index])
        {
                pulse = period - ((uint32_t)duty * period) / PWM_MAX_DUTY;
        }
        else
        {
                pulse = ((uint32_t)duty * period) / PWM_MAX_DUTY;
        }
	
        TIM_OC_InitTypeDef oc = {0};
        oc.OCMode     = TIM_OCMODE_PWM1;
        oc.Pulse      = pulse;
        oc.OCPolarity = TIM_OCPOLARITY_HIGH;
        oc.OCFastMode = TIM_OCFAST_DISABLE;

        if (HAL_TIM_PWM_ConfigChannel(&g_timer[def->timer], &oc, def->chx) != HAL_OK)
        {
                return hwPWM_HwError;
        }

        if (HAL_TIM_PWM_Start(&g_timer[def->timer], def->chx) != HAL_OK)
        {
                return hwPWM_HwError;
        }

        PWM_Channel_OnOff_Status[channel_index] = true;
        PWM_Channel_Current_Duty[channel_index] = duty;
        
        return hwPWM_OK;
}

hwPWM_OpResult PWM_Turn_Off(hwPWM_Channel channel_index)
{
        if(channel_index>=hwPWM_Channel_MAX)
        {
                return hwPWM_InvalidParameter;
        }
        
        if(PWM_Channel_Init_Status[channel_index]==false)
        {
                return hwPWM_OK;
        }
        
        const PWM_Pin_Def *def = PWM_Find_PinDef(channel_index);
        if (!def || def->pin == hwGPIO_Pin_NC)
        {
                return hwPWM_InvalidParameter;
        }

        /* Timer 必須已初始化 */
        if (!Timer_is_Init(def->timer))
        {
                return hwPWM_NotInit;
        }

        /* 計算 CCR */
        uint32_t period = g_timer[def->timer].Init.Period;
        uint32_t pulse;

        if (PWM_Channel_Inverse_Status[channel_index])
        {
                pulse = period;
        }
        else
        {
                pulse = 0;
        }
	
        TIM_OC_InitTypeDef oc = {0};
        oc.OCMode     = TIM_OCMODE_PWM1;
        oc.Pulse      = pulse;
        oc.OCPolarity = TIM_OCPOLARITY_HIGH;
        oc.OCFastMode = TIM_OCFAST_DISABLE;

        if (HAL_TIM_PWM_ConfigChannel(&g_timer[def->timer], &oc, def->chx) != HAL_OK)
        {
                return hwPWM_HwError;
        }

        if (HAL_TIM_PWM_Start(&g_timer[def->timer], def->chx) != HAL_OK)
        {
                return hwPWM_HwError;
        }

        PWM_Channel_OnOff_Status[channel_index] = false;
        
        return hwPWM_OK;
}

hwPWM_OpResult PWM_Set_Duty(hwPWM_Channel channel_index, uint16_t duty)
{
        if(channel_index>=hwPWM_Channel_MAX)
        {
                return hwPWM_InvalidParameter;
        }

        if(duty>PWM_MAX_DUTY)
        {
                return hwPWM_InvalidParameter;
        }
        
        if(PWM_Channel_Init_Status[channel_index]==false)
        {
                return hwPWM_OK;
        }
        
        if(PWM_Channel_OnOff_Status[channel_index]==false){return hwPWM_NotTurnOn;}
        
        const PWM_Pin_Def *def = PWM_Find_PinDef(channel_index);
        if (!def || def->pin == hwGPIO_Pin_NC)
        {
                return hwPWM_InvalidParameter;
        }

        /* Timer 必須已初始化 */
        if (!Timer_is_Init(def->timer))
        {
                return hwPWM_NotInit;
        }

        /* 計算 CCR */
        uint32_t period = g_timer[def->timer].Init.Period;
        uint32_t pulse;

        if (PWM_Channel_Inverse_Status[channel_index])
        {
                pulse = period - ((uint32_t)duty * period) / PWM_MAX_DUTY;
        }
        else
        {
                pulse = ((uint32_t)duty * period) / PWM_MAX_DUTY;
        }
	
        TIM_OC_InitTypeDef oc = {0};
        oc.OCMode     = TIM_OCMODE_PWM1;
        oc.Pulse      = pulse;
        oc.OCPolarity = TIM_OCPOLARITY_HIGH;
        oc.OCFastMode = TIM_OCFAST_DISABLE;

        if (HAL_TIM_PWM_ConfigChannel(&g_timer[def->timer], &oc, def->chx) != HAL_OK)
        {
                return hwPWM_HwError;
        }

        if (HAL_TIM_PWM_Start(&g_timer[def->timer], def->chx) != HAL_OK)
        {
                return hwPWM_HwError;
        }

        PWM_Channel_Current_Duty[channel_index] = duty;
        
        return hwPWM_OK;
}

hwPWM_OpResult PWM_Step_Duty(hwPWM_Channel channel_index, uint16_t step_duty, hwPWM_Step_Direction direction)
{
        if(channel_index>=hwPWM_Channel_MAX)
        {
                return hwPWM_InvalidParameter;
        }
        if(direction>=hwPWM_Step_Dir_MAX){return hwPWM_InvalidParameter;}
        
        if(PWM_Channel_Init_Status[channel_index]==false)
        {
                return hwPWM_OK;
        }
        
        if(PWM_Channel_OnOff_Status[channel_index]==false){return hwPWM_NotTurnOn;}
        
        const PWM_Pin_Def *def = PWM_Find_PinDef(channel_index);
        if (!def || def->pin == hwGPIO_Pin_NC)
        {
                return hwPWM_InvalidParameter;
        }

        /* Timer 必須已初始化 */
        if (!Timer_is_Init(def->timer))
        {
                return hwPWM_NotInit;
        }

        uint16_t current_duty = PWM_Channel_Current_Duty[channel_index];
        
        switch(direction)
        {
        case hwPWM_Step_Dir_Up:
                current_duty += step_duty;
                if(current_duty>PWM_MAX_DUTY){current_duty = PWM_MAX_DUTY;}
                break;
        case hwPWM_Step_Dir_Down:
                if((current_duty-PWM_MIN_DUTY)<step_duty){
                        current_duty = PWM_MIN_DUTY;
                }
                else{
                        current_duty -= step_duty;
                }
                break;
        }
        
        /* 計算 CCR */
        uint32_t period = g_timer[def->timer].Init.Period;
        uint32_t pulse;

        if (PWM_Channel_Inverse_Status[channel_index])
        {
                pulse = period - ((uint32_t)current_duty * period) / PWM_MAX_DUTY;
        }
        else
        {
                pulse = ((uint32_t)current_duty * period) / PWM_MAX_DUTY;
        }
	
        TIM_OC_InitTypeDef oc = {0};
        oc.OCMode     = TIM_OCMODE_PWM1;
        oc.Pulse      = pulse;
        oc.OCPolarity = TIM_OCPOLARITY_HIGH;
        oc.OCFastMode = TIM_OCFAST_DISABLE;

        if (HAL_TIM_PWM_ConfigChannel(&g_timer[def->timer], &oc, def->chx) != HAL_OK)
        {
                return hwPWM_HwError;
        }

        if (HAL_TIM_PWM_Start(&g_timer[def->timer], def->chx) != HAL_OK)
        {
                return hwPWM_HwError;
        }

        PWM_Channel_Current_Duty[channel_index] = current_duty;
        
        return hwPWM_OK;
}

hwPWM_OpResult PWM_Get_Channel_OnOff_Status(hwPWM_Channel channel_index, bool* onoff_status)
{
        if(channel_index>=hwPWM_Channel_MAX)
        {
                return hwPWM_InvalidParameter;
        }
        
        if(onoff_status==NULL){return hwPWM_InvalidParameter;}
        
        if(PWM_Channel_Init_Status[channel_index]==false)
        {
                return hwPWM_OK;
        }
        
        *onoff_status = PWM_Channel_OnOff_Status[channel_index];
        
        return hwPWM_OK;
}

hwPWM_OpResult PWM_Get_Channel_Current_Duty(hwPWM_Channel channel_index, uint16_t* current_duty)
{
        if(channel_index>=hwPWM_Channel_MAX)
        {
                return hwPWM_InvalidParameter;
        }
        
        if(current_duty==NULL){return hwPWM_InvalidParameter;}
        
        if(PWM_Channel_Init_Status[channel_index]==false)
        {
                return hwPWM_OK;
        }
        
        *current_duty = PWM_Channel_Current_Duty[channel_index];
        
        return hwPWM_OK;
}

hwPWM_OpResult PWM_Get_Channel_Current_OnOff_Duty(hwPWM_Channel channel_index, bool* onoff_status, uint16_t* current_duty)
{
        if(channel_index>=hwPWM_Channel_MAX)
        {
                return hwPWM_InvalidParameter;
        }
        
        if(onoff_status==NULL || current_duty==NULL){return hwPWM_InvalidParameter;}
        
        if(PWM_Channel_Init_Status[channel_index]==false)
        {
                return hwPWM_OK;
        }
        
        *onoff_status = PWM_Channel_OnOff_Status[channel_index];
        *current_duty = PWM_Channel_Current_Duty[channel_index];
        
        return hwPWM_OK;
}

bool PWM_is_Init(hwPWM_Channel channel_index)
{
    if(channel_index>=hwPWM_Channel_MAX)
    {
      return false;
    }
    
    return PWM_Channel_Init_Status[channel_index];
}

#endif //STM32
