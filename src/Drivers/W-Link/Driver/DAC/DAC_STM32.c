
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "soc.h"

#include "NeonRTOS.h"

#include "GPIO/GPIO.h"

#include "DAC_Pin.h"

#include "DAC.h"

#ifdef STM32

#define DAC_VREF_MV     3300.0f
#define DAC_MAX_COUNT   4095.0f

static bool DAC_Instance_Init_Status[hwDAC_Instance_MAX];
static bool DAC_Channel_Init_Status[hwDAC_Channel_Index_MAX];

static DAC_HandleTypeDef g_dac[hwDAC_Instance_MAX];

static uint32_t DAC_Channel_To_HAL(hwDAC_Channel_Index ch)
{
    switch (ch) {
        case hwDAC_Channel_Index_0: return DAC_CHANNEL_1;
        case hwDAC_Channel_Index_1: return DAC_CHANNEL_2;
        default: return 0;
    }
}

hwDAC_OpStatus hwDAC_Channel_Init(hwDAC_Channel_Index ch)
{
    hwDAC_Instance inst = DAC_Channel_Def_Table[ch].inst;
    if (inst >= hwDAC_Instance_MAX)
    {
        return hwDAC_InvalidParameter;
    }

    if (ch >= hwDAC_Channel_Index_MAX)
    {
        return hwDAC_InvalidParameter;
    }

    if (DAC_Channel_Init_Status[ch])
    {
        return hwDAC_OK;
    }

    GPIO_TypeDef *dac_pin_soc_base = GPIO_Map_Soc_Base(DAC_Channel_Def_Table[ch].dac_pin);
    uint16_t dac_soc_pin = GPIO_Map_Soc_Pin(DAC_Channel_Def_Table[ch].dac_pin);

    if(dac_soc_pin==0 || dac_pin_soc_base==NULL)
    {
        return hwDAC_InvalidParameter;
    }

    GPIO_Enable_RCC_Clock(dac_pin_soc_base);

    GPIO_InitTypeDef g_dac_pin = {0};
    g_dac_pin.Pin  = dac_soc_pin;
    g_dac_pin.Mode = GPIO_MODE_ANALOG;
    g_dac_pin.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(dac_pin_soc_base, &g_dac_pin);

    if (!DAC_Instance_Init_Status[inst])
    {
        __HAL_RCC_DAC_CLK_ENABLE();

        g_dac[inst].Instance = DAC;

        if(HAL_DAC_Init(&g_dac[inst])!=HAL_OK)
        {
            return hwDAC_HwError;
        }

        DAC_Instance_Init_Status[inst] = true;
    }

    DAC_ChannelConfTypeDef cfg = {0};
    cfg.DAC_Trigger = DAC_TRIGGER_NONE;
    cfg.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

    if(HAL_DAC_ConfigChannel(&g_dac[inst], &cfg, DAC_Channel_To_HAL(ch))!=HAL_OK)
    {
        return hwDAC_HwError;
    }
    
    if(HAL_DAC_Start(&g_dac[inst], DAC_Channel_To_HAL(ch))!=HAL_OK)
    {
        return hwDAC_HwError;
    }

    gpio_pin_init_status[DAC_Channel_Def_Table[ch].dac_pin] = true;

    DAC_Channel_Init_Status[ch] = true;
    
    return hwDAC_OK;
}

hwDAC_OpStatus hwDAC_Channel_DeInit(hwDAC_Channel_Index ch)
{
    hwDAC_Instance inst = DAC_Channel_Def_Table[ch].inst;
    if(inst>=hwDAC_Instance_MAX)
    {
        return hwDAC_InvalidParameter;
    }

    if (ch >= hwDAC_Channel_Index_MAX)
    {
        return hwDAC_InvalidParameter;
    }

    if (!DAC_Channel_Init_Status[ch])
    {
        return hwDAC_OK;
    }

    GPIO_TypeDef *dac_pin_soc_base = GPIO_Map_Soc_Base(DAC_Channel_Def_Table[ch].dac_pin);
    uint16_t dac_soc_pin = GPIO_Map_Soc_Pin(DAC_Channel_Def_Table[ch].dac_pin);

    if(dac_soc_pin==0 || dac_pin_soc_base==NULL)
    {
        return hwDAC_InvalidParameter;
    }

    DAC_Channel_Init_Status[ch] = false;

    /* 檢查 instance 是否還有人在用 */
    for (hwDAC_Instance inst = 0; inst < hwDAC_Instance_MAX; inst++)
    {
        bool used = false;

        for (size_t i = 0; i < hwDAC_Channel_Index_MAX; i++)
        {
            if (DAC_Channel_Init_Status[i])
            {
                used = true;
                break;
            }
        }

        if (!used && DAC_Instance_Init_Status[inst])
        {
            HAL_DAC_DeInit(&g_dac[inst]);

            switch (inst)
            {
                case hwDAC_Instance_1: __HAL_RCC_DAC_CLK_DISABLE(); break;
            }

            DAC_Instance_Init_Status[inst] = false;
        }
    }

    HAL_GPIO_DeInit(dac_pin_soc_base, dac_soc_pin);

    gpio_pin_init_status[DAC_Channel_Def_Table[ch].dac_pin] = false;

    return hwDAC_OK;
}

hwDAC_OpStatus hwDAC_Write_MiniVolt(hwDAC_Channel_Index ch, float mv)
{
    if (mv < 0) mv = 0;
    if (mv > DAC_VREF_MV) mv = DAC_VREF_MV;

    uint32_t raw = (uint32_t)((mv / DAC_VREF_MV) * DAC_MAX_COUNT);

    hwDAC_Instance inst = DAC_Channel_Def_Table[ch].inst;

    if (!DAC_Channel_Init_Status[ch])
    {
        return hwDAC_NotInit;
    }

    if(HAL_DAC_SetValue(&g_dac[inst], DAC_Channel_To_HAL(ch), DAC_ALIGN_12B_R, raw)!=HAL_OK)
    {
        return hwDAC_HwError;
    }

    return hwDAC_OK;
}


#endif //STM32
