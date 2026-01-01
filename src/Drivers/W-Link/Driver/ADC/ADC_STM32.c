
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "soc.h"

#include "NeonRTOS.h"

#include "GPIO/GPIO.h"

#include "ADC_Pin.h"

#include "ADC.h"

#define ADC_VREF_MV     3300.0f
#define ADC_MAX_COUNT   4095.0f

#define ADC_CONV_TIMEOUT_MS 100

typedef struct {
    uint32_t       raw;
} ADC_QueueItem;

static ADC_HandleTypeDef g_adc[hwADC_Instance_MAX];
static bool ADC_Instance_Init_Status[hwADC_Instance_MAX];
static bool ADC_Channel_Init_Status[hwADC_Channel_Index_MAX] = {false};
static NeonRTOS_MsgQ_t ADC_Channel_SyncQueue[hwADC_Instance_MAX] = {NULL};

static uint32_t ADC_Channel_To_HAL(hwADC_Channel_Index ch)
{
    switch (ch) {
        case hwADC_Channel_Index_0:  return ADC_CHANNEL_0;
        case hwADC_Channel_Index_1:  return ADC_CHANNEL_1;
        case hwADC_Channel_Index_2:  return ADC_CHANNEL_2;
        case hwADC_Channel_Index_3:  return ADC_CHANNEL_3;
        case hwADC_Channel_Index_4:  return ADC_CHANNEL_4;
        case hwADC_Channel_Index_5:  return ADC_CHANNEL_5;
        case hwADC_Channel_Index_6:  return ADC_CHANNEL_6;
        case hwADC_Channel_Index_7:  return ADC_CHANNEL_7;
        case hwADC_Channel_Index_8:  return ADC_CHANNEL_8;
        case hwADC_Channel_Index_9:  return ADC_CHANNEL_9;
        case hwADC_Channel_Index_10: return ADC_CHANNEL_10;
        case hwADC_Channel_Index_11: return ADC_CHANNEL_11;
        case hwADC_Channel_Index_12: return ADC_CHANNEL_12;
        case hwADC_Channel_Index_13: return ADC_CHANNEL_13;
        case hwADC_Channel_Index_14: return ADC_CHANNEL_14;
        case hwADC_Channel_Index_15: return ADC_CHANNEL_15;
        default: return 0;
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    ADC_QueueItem item;

    for (hwADC_Instance inst = 0; inst < hwADC_Instance_MAX; inst++)
    {
        if (hadc == &g_adc[inst])
        {
            item.raw  = HAL_ADC_GetValue(hadc);

            NeonRTOS_MsgQWrite(&ADC_Channel_SyncQueue[inst], &item, 0);

            break;
        }
    }
}

void ADC_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&g_adc[hwADC_Instance_1]);
    HAL_ADC_IRQHandler(&g_adc[hwADC_Instance_2]);
    HAL_ADC_IRQHandler(&g_adc[hwADC_Instance_3]);
}

hwADC_OpStatus hwADC_Channel_Init(hwADC_Channel_Index ch)
{
    hwADC_Instance inst = ADC_Channel_Def_Table[ch].inst;
    if(inst>=hwADC_Instance_MAX)
    {
            return hwADC_InvalidParameter;
    }

    GPIO_TypeDef * adc_soc_base = GPIO_Map_Soc_Base(ADC_Channel_Def_Table[ch].adc_pin);
    uint16_t adc_soc_pin = GPIO_Map_Soc_Pin(ADC_Channel_Def_Table[ch].adc_pin);

    if(adc_soc_pin==0 || adc_soc_base==NULL)
    {
            return hwADC_InvalidParameter;
    }

    if (!ADC_Instance_Init_Status[inst])
    {
        if(NeonRTOS_MsgQCreate(&ADC_Channel_SyncQueue[inst], "adc", sizeof(ADC_QueueItem), 1)!=NeonRTOS_OK)
        {
            return hwADC_MemoryError;
        }
    }

    GPIO_Enable_RCC_Clock(adc_soc_base);

    GPIO_InitTypeDef adc_init = {0};
    adc_init.Pin  = adc_soc_pin;
    adc_init.Mode = GPIO_MODE_ANALOG;
    adc_init.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(adc_soc_base, &adc_init);

    /* ADC instance */
    if (!ADC_Instance_Init_Status[inst])
    {
        switch (inst)
        {
            case hwADC_Instance_1:
                __HAL_RCC_ADC1_CLK_ENABLE();
                g_adc[inst].Instance = ADC1;
                break;
            case hwADC_Instance_2:
                __HAL_RCC_ADC2_CLK_ENABLE();
                g_adc[inst].Instance = ADC2;
                break;
            case hwADC_Instance_3:
                __HAL_RCC_ADC3_CLK_ENABLE();
                g_adc[inst].Instance = ADC3;
                break;
            default:
                return;
        }

        g_adc[inst].Init.Resolution            = ADC_RESOLUTION_12B;
        g_adc[inst].Init.ScanConvMode          = DISABLE;
        g_adc[inst].Init.ContinuousConvMode    = DISABLE;
        g_adc[inst].Init.DiscontinuousConvMode = DISABLE;
        g_adc[inst].Init.ExternalTrigConv      = ADC_SOFTWARE_START;
        g_adc[inst].Init.DataAlign             = ADC_DATAALIGN_RIGHT;
        g_adc[inst].Init.NbrOfConversion       = 1;

        if(HAL_ADC_Init(&g_adc[inst])!=HAL_OK)
        {
            return hwADC_HwError;
        }

        ADC_Instance_Init_Status[inst] = true;
    }

    gpio_pin_init_status[ADC_Channel_Def_Table[ch].adc_pin] = true;

    ADC_Channel_Init_Status[ch] = true;

    return hwADC_OK;
}

hwADC_OpStatus hwADC_Channel_DeInit(hwADC_Channel_Index ch)
{
    hwADC_Instance inst = ADC_Channel_Def_Table[ch].inst;
    if(inst>=hwADC_Instance_MAX)
    {
            return hwADC_InvalidParameter;
    }

    if (ch >= hwADC_Channel_Index_MAX)
    {
        return hwADC_InvalidParameter;
    }

    if (!ADC_Channel_Init_Status[ch])
    {
        return hwADC_OK;
    }

    GPIO_TypeDef * adc_soc_base = GPIO_Map_Soc_Base(ADC_Channel_Def_Table[ch].adc_pin);
    uint16_t adc_soc_pin = GPIO_Map_Soc_Pin(ADC_Channel_Def_Table[ch].adc_pin);

    if(adc_soc_pin==0 || adc_soc_base==NULL)
    {
            return hwADC_InvalidParameter;
    }

    ADC_Channel_Init_Status[ch] = false;

    /* 檢查 instance 是否還有人在用 */
    for (hwADC_Instance inst = 0; inst < hwADC_Instance_MAX; inst++)
    {
        bool used = false;

        for (size_t i = 0; i < hwADC_Channel_Index_MAX; i++)
        {
            if (ADC_Channel_Init_Status[i])
            {
                used = true;
                break;
            }
        }

        if (!used && ADC_Instance_Init_Status[inst])
        {
            HAL_ADC_DeInit(&g_adc[inst]);

            switch (inst)
            {
                case hwADC_Instance_1: __HAL_RCC_ADC1_CLK_DISABLE(); break;
                case hwADC_Instance_2: __HAL_RCC_ADC2_CLK_DISABLE(); break;
                case hwADC_Instance_3: __HAL_RCC_ADC3_CLK_DISABLE(); break;
            }

            ADC_Instance_Init_Status[inst] = false;
            
            NeonRTOS_MsgQDelete(&ADC_Channel_SyncQueue[inst]);
        }
    }

    HAL_GPIO_DeInit(adc_soc_base, adc_soc_pin);

    gpio_pin_init_status[ADC_Channel_Def_Table[ch].adc_pin] = false;

    return hwADC_OK;
}

hwADC_OpStatus hwADC_Read_MiniVolt(hwADC_Channel_Index ch, float *readMv)
{
    hwADC_Instance inst = ADC_Channel_Def_Table[ch].inst;
    if(inst>=hwADC_Instance_MAX)
    {
            return hwADC_InvalidParameter;
    }

    if (!readMv || ch >= hwADC_Channel_Index_MAX)
    {
        return hwADC_InvalidParameter;
    }

    if (!ADC_Channel_Init_Status[ch])
    {
        return hwADC_NotInit;
    }

    ADC_HandleTypeDef *hadc = &g_adc[inst];

    ADC_ChannelConfTypeDef cfg = {0};
    cfg.Channel      = ADC_Channel_To_HAL(ch);

#if defined(ADC_REGULAR_RANK_1)
    cfg.Rank = ADC_REGULAR_RANK_1;
#endif

#if defined(ADC_SAMPLETIME_247CYCLES_5)
    cfg.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
#else
    cfg.SamplingTime = ADC_SAMPLETIME_144CYCLES;
#endif

    HAL_ADC_ConfigChannel(hadc, &cfg);

    HAL_ADC_Start_IT(hadc);

    ADC_QueueItem item;
    if (NeonRTOS_MsgQRead(&ADC_Channel_SyncQueue[inst], &item, ADC_CONV_TIMEOUT_MS) != NeonRTOS_OK)
    {
        HAL_ADC_Stop_IT(hadc);
        return hwADC_HW_Error;
    }

    HAL_ADC_Stop_IT(hadc);

    *readMv = (item.raw * ADC_VREF_MV) / ADC_MAX_COUNT;

    return hwADC_OK;
}
