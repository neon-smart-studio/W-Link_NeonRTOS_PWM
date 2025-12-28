
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include "soc.h"

#include "NeonRTOS.h"

#include "Timer.h"

#ifdef STM32

#define TIMER_MUTEX_ACCESS_TIMEOUT     500

#define TIMER_IRQ_NVIC_PRIORITY 5
#define TIMER_IRQ_NVIC_SUB_PRIORITY 0

static bool Timer_Init_Status[hwTimer_Index_MAX] = {false};
static bool Timer_IsPeriodic[hwTimer_Index_MAX] = {false};
static onTimerEventHandler Timer_Expired_Handler[hwTimer_Index_MAX] = {NULL};

TIM_HandleTypeDef g_timer[hwTimer_Index_MAX];

static hwTimer_Index Timer_IndexFromHandle(TIM_HandleTypeDef *htimer)
{
    for(int i=0;i<hwTimer_Index_MAX;i++){
        if(&g_timer[i] == htimer) return (hwTimer_Index)i;
    }
    return hwTimer_Index_MAX;
}

TIM_TypeDef * Timer_Map_Soc_Base(hwTimer_Index index)
{
    switch(index)
    {
        case hwTimer_Index_0: return TIM1;
        case hwTimer_Index_1: return TIM2;
        case hwTimer_Index_2: return TIM3;
        case hwTimer_Index_3: return TIM4;
        case hwTimer_Index_4: return TIM5;
        case hwTimer_Index_5: return TIM6;
        case hwTimer_Index_6: return TIM7;
        case hwTimer_Index_7: return TIM8;
        case hwTimer_Index_8: return TIM9;
        case hwTimer_Index_9: return TIM10;
        case hwTimer_Index_10: return TIM11;
        case hwTimer_Index_11: return TIM12;
        case hwTimer_Index_12: return TIM13;
        case hwTimer_Index_13: return TIM14;
        default: break;
    }
    return NULL;
}

static uint32_t GetAPB1TimerClock(void)
{
    uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();

    if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1)
        return pclk1 * 2;

    return pclk1;
}

static uint32_t GetAPB2TimerClock(void)
{
    uint32_t pclk2 = HAL_RCC_GetPCLK2Freq();

    if ((RCC->CFGR & RCC_CFGR_PPRE2) != RCC_CFGR_PPRE2_DIV1)
        return pclk2 * 2;

    return pclk2;
}

static uint32_t Timer_GetInputClock(hwTimer_Index index)
{
    switch (index)
    {
        /* ---------- APB2 ---------- */
        case hwTimer_Index_0:   // TIM1
        case hwTimer_Index_7:   // TIM8
        case hwTimer_Index_8:   // TIM9
        case hwTimer_Index_9:   // TIM10
        case hwTimer_Index_10:  // TIM11
            return GetAPB2TimerClock();

        /* ---------- APB1 ---------- */
        default:
            return GetAPB1TimerClock();
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    for (int i = 0; i < hwTimer_Index_MAX; i++)
    {
        if (&g_timer[i] == htim)
        {
                // 👈 這裡才是 index 與 Instance 的對應點
                if(Timer_Expired_Handler[i]!=NULL)
                {
                        Timer_Expired_Handler[i]((hwTimer_Index)i);
                }
                
                /* One-shot 要自動停 */
                if (!Timer_IsPeriodic[i])
                {
                        HAL_TIM_Base_Stop_IT(htim);
                }
                break;
        }
    }
}

// TIM IRQ 統一入口
static void Timer_HAL_IRQHandler(hwTimer_Index index)
{
        HAL_TIM_IRQHandler(&g_timer[index]);
}

void TIM1_UP_TIM10_IRQHandler(void)      { Timer_HAL_IRQHandler(hwTimer_Index_0); \
                                           Timer_HAL_IRQHandler(hwTimer_Index_9); }
void TIM2_IRQHandler(void)               { Timer_HAL_IRQHandler(hwTimer_Index_1); }
void TIM3_IRQHandler(void)               { Timer_HAL_IRQHandler(hwTimer_Index_2); }
void TIM4_IRQHandler(void)               { Timer_HAL_IRQHandler(hwTimer_Index_3); }
void TIM5_IRQHandler(void)               { Timer_HAL_IRQHandler(hwTimer_Index_4); }
void TIM6_DAC_IRQHandler(void)           { Timer_HAL_IRQHandler(hwTimer_Index_5); }
void TIM7_IRQHandler(void)               { Timer_HAL_IRQHandler(hwTimer_Index_6); }
void TIM8_UP_TIM13_IRQHandler(void)      { Timer_HAL_IRQHandler(hwTimer_Index_7); \
                                           Timer_HAL_IRQHandler(hwTimer_Index_12); }
void TIM1_BRK_TIM9_IRQHandler(void)      { Timer_HAL_IRQHandler(hwTimer_Index_8); }
void TIM1_TRG_COM_TIM11_IRQHandler(void) { Timer_HAL_IRQHandler(hwTimer_Index_10); }
void TIM8_BRK_TIM12_IRQHandler(void)     { Timer_HAL_IRQHandler(hwTimer_Index_11); }
void TIM8_TRG_COM_TIM14_IRQHandler(void) { Timer_HAL_IRQHandler(hwTimer_Index_13); }

hwTimer_OpResult Timer_Init(hwTimer_Index index)
{
        if (index >= hwTimer_Index_MAX)
        {
                return hwTimer_InvalidParameter;
        }

        if (Timer_Init_Status[index])
        {
                return hwTimer_OK;
        }
        
        TIM_TypeDef * timer_soc_base = Timer_Map_Soc_Base(index);
        if(timer_soc_base==NULL)
        {
                return hwTimer_InvalidParameter;
        }

        switch (index)
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
                case hwTimer_Index_5:
                        __HAL_RCC_TIM6_CLK_ENABLE();
                        break;
                case hwTimer_Index_6:
                        __HAL_RCC_TIM7_CLK_ENABLE();
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

        switch (index)
        {
                /* ---------- TIM1 / TIM10 共用 ---------- */
                case hwTimer_Index_0:   // TIM1
                case hwTimer_Index_9:   // TIM10
                        HAL_NVIC_SetPriority(
                                TIM1_UP_TIM10_IRQn,
                                TIMER_IRQ_NVIC_PRIORITY,
                                TIMER_IRQ_NVIC_SUB_PRIORITY
                        );
                        HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
                        break;

                /* ---------- TIM2 ---------- */
                case hwTimer_Index_1:
                        HAL_NVIC_SetPriority(
                                TIM2_IRQn,
                                TIMER_IRQ_NVIC_PRIORITY,
                                TIMER_IRQ_NVIC_SUB_PRIORITY
                        );
                        HAL_NVIC_EnableIRQ(TIM2_IRQn);
                        break;

                /* ---------- TIM3 ---------- */
                case hwTimer_Index_2:
                        HAL_NVIC_SetPriority(
                                TIM3_IRQn,
                                TIMER_IRQ_NVIC_PRIORITY,
                                TIMER_IRQ_NVIC_SUB_PRIORITY
                        );
                        HAL_NVIC_EnableIRQ(TIM3_IRQn);
                        break;

                /* ---------- TIM4 ---------- */
                case hwTimer_Index_3:
                        HAL_NVIC_SetPriority(
                                TIM4_IRQn,
                                TIMER_IRQ_NVIC_PRIORITY,
                                TIMER_IRQ_NVIC_SUB_PRIORITY
                        );
                        HAL_NVIC_EnableIRQ(TIM4_IRQn);
                        break;

                /* ---------- TIM5 ---------- */
                case hwTimer_Index_4:
                        HAL_NVIC_SetPriority(
                                TIM5_IRQn,
                                TIMER_IRQ_NVIC_PRIORITY,
                                TIMER_IRQ_NVIC_SUB_PRIORITY
                        );
                        HAL_NVIC_EnableIRQ(TIM5_IRQn);
                        break;

                /* ---------- TIM6 ---------- */
                case hwTimer_Index_5:
                        HAL_NVIC_SetPriority(
                                TIM6_DAC_IRQn,
                                TIMER_IRQ_NVIC_PRIORITY,
                                TIMER_IRQ_NVIC_SUB_PRIORITY
                        );
                        HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
                        break;

                /* ---------- TIM7 ---------- */
                case hwTimer_Index_6:
                        HAL_NVIC_SetPriority(
                                TIM7_IRQn,
                                TIMER_IRQ_NVIC_PRIORITY,
                                TIMER_IRQ_NVIC_SUB_PRIORITY
                        );
                        HAL_NVIC_EnableIRQ(TIM7_IRQn);
                        break;

                /* ---------- TIM8 / TIM13 共用 ---------- */
                case hwTimer_Index_7:   // TIM8
                case hwTimer_Index_12:  // TIM13
                        HAL_NVIC_SetPriority(
                                TIM8_UP_TIM13_IRQn,
                                TIMER_IRQ_NVIC_PRIORITY,
                                TIMER_IRQ_NVIC_SUB_PRIORITY
                        );
                        HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
                        break;

                /* ---------- TIM9 ---------- */
                case hwTimer_Index_8:
                        HAL_NVIC_SetPriority(
                                TIM1_BRK_TIM9_IRQn,
                                TIMER_IRQ_NVIC_PRIORITY,
                                TIMER_IRQ_NVIC_SUB_PRIORITY
                        );
                        HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
                        break;

                /* ---------- TIM11 ---------- */
                case hwTimer_Index_10:
                        HAL_NVIC_SetPriority(
                                TIM1_TRG_COM_TIM11_IRQn,
                                TIMER_IRQ_NVIC_PRIORITY,
                                TIMER_IRQ_NVIC_SUB_PRIORITY
                        );
                        HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
                        break;

                /* ---------- TIM12 ---------- */
                case hwTimer_Index_11:
                        HAL_NVIC_SetPriority(
                                TIM8_BRK_TIM12_IRQn,
                                TIMER_IRQ_NVIC_PRIORITY,
                                TIMER_IRQ_NVIC_SUB_PRIORITY
                        );
                        HAL_NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);
                        break;

                /* ---------- TIM14 ---------- */
                case hwTimer_Index_13:
                        HAL_NVIC_SetPriority(
                                TIM8_TRG_COM_TIM14_IRQn,
                                TIMER_IRQ_NVIC_PRIORITY,
                                TIMER_IRQ_NVIC_SUB_PRIORITY
                        );
                        HAL_NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn);
                        break;

                default:
                        break;
        }

        g_timer[index].Instance = timer_soc_base;
        g_timer[index].Init.Prescaler = (Timer_GetInputClock(index) / 1000000) - 1; // 1 MHz = 1us
        g_timer[index].Init.CounterMode = TIM_COUNTERMODE_UP;
        g_timer[index].Init.Period = 1000 - 1; // 預設 1ms
        g_timer[index].Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        g_timer[index].Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

        HAL_TIM_Base_Init(&g_timer[index]);
        
        Timer_Init_Status[index] = true;

        return hwTimer_OK;
}

hwTimer_OpResult Timer_DeInit(hwTimer_Index index)
{
        if(index>=hwTimer_Index_MAX)
        {
                return hwTimer_InvalidParameter;
        }
        
        if(Timer_Init_Status[index]==false)
        {
                return hwTimer_OK;
        }

        Timer_Init_Status[index] = false;

        HAL_TIM_Base_Stop_IT(&g_timer[index]);
        HAL_TIM_Base_DeInit(&g_timer[index]);
        
        switch (index)
        {
                /* ---------- TIM1 / TIM10 共用 ---------- */
                case hwTimer_Index_0:   // TIM1
                case hwTimer_Index_9:   // TIM10
                        if (!Timer_Init_Status[hwTimer_Index_0] && !Timer_Init_Status[hwTimer_Index_9])
                        {
                                HAL_NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
                        }
                        break;

                /* ---------- TIM2 ---------- */
                case hwTimer_Index_1:
                        HAL_NVIC_DisableIRQ(TIM2_IRQn);
                        break;

                /* ---------- TIM3 ---------- */
                case hwTimer_Index_2:
                        HAL_NVIC_DisableIRQ(TIM3_IRQn);
                        break;

                /* ---------- TIM4 ---------- */
                case hwTimer_Index_3:
                        HAL_NVIC_DisableIRQ(TIM4_IRQn);
                        break;

                /* ---------- TIM5 ---------- */
                case hwTimer_Index_4:
                        HAL_NVIC_DisableIRQ(TIM5_IRQn);
                        break;

                /* ---------- TIM6 ---------- */
                case hwTimer_Index_5:
                        HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
                        break;

                /* ---------- TIM7 ---------- */
                case hwTimer_Index_6:
                        HAL_NVIC_DisableIRQ(TIM7_IRQn);
                        break;

                /* ---------- TIM8 / TIM13 共用 ---------- */
                case hwTimer_Index_7:   // TIM8
                case hwTimer_Index_12:  // TIM13
                        if (!Timer_Init_Status[hwTimer_Index_7] && !Timer_Init_Status[hwTimer_Index_12])
                        {
                                HAL_NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
                        }
                        break;

                /* ---------- TIM9 ---------- */
                case hwTimer_Index_8:
                        HAL_NVIC_DisableIRQ(TIM1_BRK_TIM9_IRQn);
                        break;

                /* ---------- TIM11 ---------- */
                case hwTimer_Index_10:
                        HAL_NVIC_DisableIRQ(TIM1_TRG_COM_TIM11_IRQn);
                        break;

                /* ---------- TIM12 ---------- */
                case hwTimer_Index_11:
                        HAL_NVIC_DisableIRQ(TIM8_BRK_TIM12_IRQn);
                        break;

                /* ---------- TIM14 ---------- */
                case hwTimer_Index_13:
                        HAL_NVIC_DisableIRQ(TIM8_TRG_COM_TIM14_IRQn);
                        break;

                default:
                        break;
        }

        switch (index)
        {
                case hwTimer_Index_0:
                        __HAL_RCC_TIM1_CLK_DISABLE();
                        break;
                case hwTimer_Index_1:
                        __HAL_RCC_TIM2_CLK_DISABLE();
                        break;
                case hwTimer_Index_2:
                        __HAL_RCC_TIM3_CLK_DISABLE();
                        break;
                case hwTimer_Index_3:
                        __HAL_RCC_TIM4_CLK_DISABLE();
                        break;
                case hwTimer_Index_4:
                        __HAL_RCC_TIM5_CLK_DISABLE();
                        break;
                case hwTimer_Index_5:
                        __HAL_RCC_TIM6_CLK_DISABLE();
                        break;
                case hwTimer_Index_6:
                        __HAL_RCC_TIM7_CLK_DISABLE();
                        break;
                case hwTimer_Index_7:
                        __HAL_RCC_TIM8_CLK_DISABLE();
                        break;
                case hwTimer_Index_8:
                        __HAL_RCC_TIM9_CLK_DISABLE();
                        break;
                case hwTimer_Index_9:
                        __HAL_RCC_TIM10_CLK_DISABLE();
                        break;
                case hwTimer_Index_10:
                        __HAL_RCC_TIM11_CLK_DISABLE();
                        break;
                case hwTimer_Index_11:
                        __HAL_RCC_TIM12_CLK_DISABLE();
                        break;
                case hwTimer_Index_12:
                        __HAL_RCC_TIM13_CLK_DISABLE();
                        break;
                case hwTimer_Index_13:
                        __HAL_RCC_TIM14_CLK_DISABLE();
                        break;
        }

        return hwTimer_OK;
}

hwTimer_OpResult Timer_Start_OneShout(hwTimer_Index index, uint32_t duration_us, onTimerEventHandler timer_exp_cb)
{
        if(index>=hwTimer_Index_MAX)
        {
                return hwTimer_InvalidParameter;
        }
        
        if(Timer_Init_Status[index]==false)
        {
                return hwTimer_NotInit;
        }

        if (duration_us == 0)
        {
                return hwTimer_InvalidParameter;
        }

        /* 設定 callback 與模式 */
        Timer_Expired_Handler[index] = timer_exp_cb;
        Timer_IsPeriodic[index] = false;

        /* 停止再設定，避免 race */
        HAL_TIM_Base_Stop_IT(&g_timer[index]);

        /* 1 tick = 1 us，所以 ARR = us - 1 */
        g_timer[index].Init.Period = duration_us - 1;
        HAL_TIM_Base_Init(&g_timer[index]);

        /* 重新開始 */
        HAL_TIM_Base_Start_IT(&g_timer[index]);

        return hwTimer_OK;
}

hwTimer_OpResult Timer_Start_Period(hwTimer_Index index, uint32_t duration_us, onTimerEventHandler timer_exp_cb)
{
        if(index>=hwTimer_Index_MAX)
        {
                return hwTimer_InvalidParameter;
        }
        
        if(Timer_Init_Status[index]==false)
        {
                return hwTimer_NotInit;
        }

        if (duration_us == 0)
        {
                return hwTimer_InvalidParameter;
        }

        Timer_Expired_Handler[index] = timer_exp_cb;
        Timer_IsPeriodic[index] = true;

        HAL_TIM_Base_Stop_IT(&g_timer[index]);

        g_timer[index].Init.Period = duration_us - 1;
        HAL_TIM_Base_Init(&g_timer[index]);

        HAL_TIM_Base_Start_IT(&g_timer[index]);

        return hwTimer_OK;
}

hwTimer_OpResult Timer_Reload(hwTimer_Index index, uint32_t duration_us)
{
        if(index>=hwTimer_Index_MAX)
        {
                return hwTimer_InvalidParameter;
        }
        
        if(Timer_Init_Status[index]==false)
        {
                return hwTimer_NotInit;
        }

        if (duration_us == 0)
        {
                return hwTimer_InvalidParameter;
        }

        HAL_TIM_Base_Stop_IT(&g_timer[index]);

        g_timer[index].Init.Period = duration_us - 1;
        HAL_TIM_Base_Init(&g_timer[index]);

        HAL_TIM_Base_Start_IT(&g_timer[index]);

        return hwTimer_OK;
}

hwTimer_OpResult Timer_Stop(hwTimer_Index index)
{
        if(index>=hwTimer_Index_MAX)
        {
                return hwTimer_InvalidParameter;
        }
        
        if(Timer_Init_Status[index]==false)
        {
                return hwTimer_NotInit;
        }

        HAL_TIM_Base_Stop_IT(&g_timer[index]);

        return hwTimer_OK;
}

hwTimer_OpResult Timer_Read_Ticks(hwTimer_Index index, uint32_t* ticks)
{
        if(index>=hwTimer_Index_MAX)
        {
                return hwTimer_InvalidParameter;
        }
        
        if(ticks==NULL)
        {
                return hwTimer_InvalidParameter;
        }
        
        if(Timer_Init_Status[index]==false)
        {
                return hwTimer_NotInit;
        }

        *ticks = __HAL_TIM_GET_COUNTER(&g_timer[index]);

        return hwTimer_OK;
}

hwTimer_OpResult Timer_Read_uSec(hwTimer_Index index, uint32_t* uSec)
{
        if(index>=hwTimer_Index_MAX)
        {
                return hwTimer_InvalidParameter;
        }
        
        if(uSec==NULL)
        {
                return hwTimer_InvalidParameter;
        }
        
        if(Timer_Init_Status[index]==false)
        {
                return hwTimer_NotInit;
        }

        return Timer_Read_Ticks(index, uSec);
}

bool Timer_is_Init(hwTimer_Index hw_index)
{
    if(hw_index>=hwTimer_Index_MAX)
    {
      return false;
    }
    
    return Timer_Init_Status[hw_index];
}

#endif //STM32