
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include "soc.h"

#include "DMA.h"

#include "NeonRTOS.h"

#ifdef STM32

#define DMA_IRQ_NVIC_PRIORITY 5
#define DMA_IRQ_NVIC_SUB_PRIORITY 0

#define DMA_WAIT_ALLOCATED_TIMEOUT  1000
#define DMA_WAIT_TRANSFER_TIMEOUT   1000

DMA_HandleTypeDef g_dma[hwDMA_Stream_Index_MAX];

static bool DMA_Stream_Init_Status[hwDMA_Stream_Index_MAX] = {false};

DMA_TypeDef * DMA_Map_Soc_Base(hwDMA_Stream_Index index)
{
    switch(index)
    {
#if defined (DMA1_BASE)
        case hwDMA_Stream_Index_0:
        case hwDMA_Stream_Index_1:
        case hwDMA_Stream_Index_2:
        case hwDMA_Stream_Index_3:
        case hwDMA_Stream_Index_4:
        case hwDMA_Stream_Index_5:
        case hwDMA_Stream_Index_6:
        case hwDMA_Stream_Index_7:
                return DMA1;
#endif
#if defined (DMA2_BASE)
        case hwDMA_Stream_Index_8:
        case hwDMA_Stream_Index_9:
        case hwDMA_Stream_Index_10:
        case hwDMA_Stream_Index_11:
        case hwDMA_Stream_Index_12:
        case hwDMA_Stream_Index_13:
        case hwDMA_Stream_Index_14:
        case hwDMA_Stream_Index_15:
                return DMA2;
#endif
        default:
                break;
    }
    return NULL;
}

#if defined (BDMA_BASE)
BDMA_TypeDef * BDMA_Map_Soc_Base(hwDMA_Stream_Index index)
{
    switch(index)
    {
        case hwDMA_Stream_Index_16:
        case hwDMA_Stream_Index_17:
        case hwDMA_Stream_Index_18:
        case hwDMA_Stream_Index_19:
        case hwDMA_Stream_Index_20:
        case hwDMA_Stream_Index_21:
        case hwDMA_Stream_Index_22:
        case hwDMA_Stream_Index_23:
                return BDMA;
        default:
                break;
    }
    return NULL;
}
#endif //BDMA_BASE

DMA_Stream_TypeDef * DMA_Map_Soc_Stream_Base(hwDMA_Stream_Index index)
{
    switch(index)
    {
#if defined (DMA1_BASE)
        case hwDMA_Stream_Index_0: return DMA1_Stream0;
        case hwDMA_Stream_Index_1: return DMA1_Stream1;
        case hwDMA_Stream_Index_2: return DMA1_Stream2;
        case hwDMA_Stream_Index_3: return DMA1_Stream3;
        case hwDMA_Stream_Index_4: return DMA1_Stream4;
        case hwDMA_Stream_Index_5: return DMA1_Stream5;
        case hwDMA_Stream_Index_6: return DMA1_Stream6;
        case hwDMA_Stream_Index_7: return DMA1_Stream7;
#endif
#if defined (DMA2_BASE)
        case hwDMA_Stream_Index_8: return DMA2_Stream0;
        case hwDMA_Stream_Index_9: return DMA2_Stream1;
        case hwDMA_Stream_Index_10: return DMA2_Stream2;
        case hwDMA_Stream_Index_11: return DMA2_Stream3;
        case hwDMA_Stream_Index_12: return DMA2_Stream4;
        case hwDMA_Stream_Index_13: return DMA2_Stream5;
        case hwDMA_Stream_Index_14: return DMA2_Stream6;
        case hwDMA_Stream_Index_15: return DMA2_Stream7;
#endif
        default: break;
    }
    return NULL;
}

#if defined (BDMA_BASE)
BDMA_Channel_TypeDef * BDMA_Map_Soc_Channel_Base(hwDMA_Stream_Index index)
{
    switch(index)
    {
        case hwDMA_Stream_Index_16: return BDMA_Channel0;
        case hwDMA_Stream_Index_17: return BDMA_Channel1;
        case hwDMA_Stream_Index_18: return BDMA_Channel2;
        case hwDMA_Stream_Index_19: return BDMA_Channel3;
        case hwDMA_Stream_Index_20: return BDMA_Channel4;
        case hwDMA_Stream_Index_21: return BDMA_Channel5;
        case hwDMA_Stream_Index_22: return BDMA_Channel6;
        case hwDMA_Stream_Index_23: return BDMA_Channel7;
        default: break;
    }
    return NULL;
}
#endif //BDMA_BASE

static void DMA_IRQ_Handler(hwDMA_Stream_Index index)
{
        if(index>=hwDMA_Stream_Index_MAX)
        {
                return;
        }

        HAL_DMA_IRQHandler(&g_dma[index]);
}

#if defined (BDMA_BASE)
static void BDMA_IRQ_Handler(hwDMA_Stream_Index index)
{
        if(index>=hwDMA_Stream_Index_MAX)
        {
                return;
        }

        HAL_BDMA_IRQHandler(&g_dma[index]);
}
#endif //BDMA_BASE

#if defined (DMA1_BASE)
void DMA1_Stream0_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_0); }
void DMA1_Stream1_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_1); }
void DMA1_Stream2_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_2); }
void DMA1_Stream3_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_3); }
void DMA1_Stream4_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_4); }
void DMA1_Stream5_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_5); }
void DMA1_Stream6_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_6); }
void DMA1_Stream7_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_7); }
#endif //DMA1_BASE

#if defined (DMA2_BASE)
void DMA2_Stream0_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_8); }
void DMA2_Stream1_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_9); }
void DMA2_Stream2_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_10); }
void DMA2_Stream3_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_11); }
void DMA2_Stream4_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_12); }
void DMA2_Stream5_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_13); }
void DMA2_Stream6_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_14); }
void DMA2_Stream7_IRQHandler(void){ DMA_IRQ_Handler(hwDMA_Stream_Index_15); }
#endif //DMA2_BASE

#if defined (BDMA_BASE)
void BDMA_Channel0_IRQHandler(void){ BDMA_IRQ_Handler(hwDMA_Stream_Index_16); }
void BDMA_Channel1_IRQHandler(void){ BDMA_IRQ_Handler(hwDMA_Stream_Index_17); }
void BDMA_Channel2_IRQHandler(void){ BDMA_IRQ_Handler(hwDMA_Stream_Index_18); }
void BDMA_Channel3_IRQHandler(void){ BDMA_IRQ_Handler(hwDMA_Stream_Index_19); }
void BDMA_Channel4_IRQHandler(void){ BDMA_IRQ_Handler(hwDMA_Stream_Index_20); }
void BDMA_Channel5_IRQHandler(void){ BDMA_IRQ_Handler(hwDMA_Stream_Index_21); }
void BDMA_Channel6_IRQHandler(void){ BDMA_IRQ_Handler(hwDMA_Stream_Index_22); }
void BDMA_Channel7_IRQHandler(void){ BDMA_IRQ_Handler(hwDMA_Stream_Index_23); }
#endif //BDMA_BASE

hwDMA_OpResult DMA_Stream_Init(hwDMA_Stream_Index stream_index)
{
        if(stream_index>=hwDMA_Stream_Index_MAX)
        {
                return hwDMA_InvalidParameter;
        }

        if(DMA_Stream_Init_Status[stream_index]==true)
        {
                return hwDMA_OK;
        }

        switch(stream_index)
        {
#if defined (DMA1_BASE)
                case hwDMA_Stream_Index_0:
                case hwDMA_Stream_Index_1:
                case hwDMA_Stream_Index_2:
                case hwDMA_Stream_Index_3:
                case hwDMA_Stream_Index_4:
                case hwDMA_Stream_Index_5:
                case hwDMA_Stream_Index_6:
                case hwDMA_Stream_Index_7:
                        __HAL_RCC_DMA1_CLK_ENABLE();
                        break;
#endif //DMA1_BASE

#if defined (DMA2_BASE)
                case hwDMA_Stream_Index_8:
                case hwDMA_Stream_Index_9:
                case hwDMA_Stream_Index_10:
                case hwDMA_Stream_Index_11:
                case hwDMA_Stream_Index_12:
                case hwDMA_Stream_Index_13:
                case hwDMA_Stream_Index_14:
                case hwDMA_Stream_Index_15:
                        __HAL_RCC_DMA2_CLK_ENABLE();
                        break;
#endif //DMA2_BASE

#if defined (BDMA_BASE)
                case hwDMA_Stream_Index_16:
                case hwDMA_Stream_Index_17:
                case hwDMA_Stream_Index_18:
                case hwDMA_Stream_Index_19:
                case hwDMA_Stream_Index_20:
                case hwDMA_Stream_Index_21:
                case hwDMA_Stream_Index_22:
                case hwDMA_Stream_Index_23:
                        __HAL_RCC_BDMA_CLK_ENABLE();
                        break;
#endif //BDMA_BASE
        }

        switch(stream_index)
        {
#if defined (DMA1_BASE)
                case hwDMA_Stream_Index_0:
                        HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
                        break;
                case hwDMA_Stream_Index_1:
                        HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
                        break;
                case hwDMA_Stream_Index_2:
                        HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
                        break;
                case hwDMA_Stream_Index_3:
                        HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
                        break;
                case hwDMA_Stream_Index_4:
                        HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
                        break;
                case hwDMA_Stream_Index_5:
                        HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
                        break;
                case hwDMA_Stream_Index_6:
                        HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
                        break;
                case hwDMA_Stream_Index_7:
                        HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
                        break;
#endif //DMA1_BASE
#if defined (DMA2_BASE)
                case hwDMA_Stream_Index_8:
                        HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
                        break;
                case hwDMA_Stream_Index_9:
                        HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
                        break;
                case hwDMA_Stream_Index_10:
                        HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
                        break;
                case hwDMA_Stream_Index_11:
                        HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
                        break;
                case hwDMA_Stream_Index_12:
                        HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
                        break;
                case hwDMA_Stream_Index_13:
                        HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
                        break;
                case hwDMA_Stream_Index_14:
                        HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
                        break;
                case hwDMA_Stream_Index_15:
                        HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
                        break;
#endif //DMA2_BASE
#if defined (BDMA_BASE)
                case hwDMA_Stream_Index_16:
                        HAL_NVIC_SetPriority(BDMA_Channel0_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(BDMA_Channel0_IRQn);
                        break;
                case hwDMA_Stream_Index_17:
                        HAL_NVIC_SetPriority(BDMA_Channel1_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(BDMA_Channel1_IRQn);
                        break;
                case hwDMA_Stream_Index_18:
                        HAL_NVIC_SetPriority(BDMA_Channel2_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(BDMA_Channel2_IRQn);
                        break;
                case hwDMA_Stream_Index_19:
                        HAL_NVIC_SetPriority(BDMA_Channel3_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(BDMA_Channel3_IRQn);
                        break;
                case hwDMA_Stream_Index_20:
                        HAL_NVIC_SetPriority(BDMA_Channel4_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(BDMA_Channel4_IRQn);
                        break;
                case hwDMA_Stream_Index_21:
                        HAL_NVIC_SetPriority(BDMA_Channel5_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(BDMA_Channel5_IRQn);
                        break;
                case hwDMA_Stream_Index_22:
                        HAL_NVIC_SetPriority(BDMA_Channel6_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(BDMA_Channel6_IRQn);
                        break;
                case hwDMA_Stream_Index_23:
                        HAL_NVIC_SetPriority(BDMA_Channel7_IRQn, DMA_IRQ_NVIC_PRIORITY, DMA_IRQ_NVIC_SUB_PRIORITY);
                        HAL_NVIC_EnableIRQ(BDMA_Channel7_IRQn);
                        break;
#endif //BDMA_BASE
        }

        DMA_Stream_Init_Status[stream_index] = true;

        return hwDMA_OK;
}

hwDMA_OpResult DMA_Stream_DeInit(hwDMA_Stream_Index stream_index)
{
        if(stream_index>=hwDMA_Stream_Index_MAX)
        {
                return hwDMA_InvalidParameter;
        }

        if(DMA_Stream_Init_Status[stream_index]==false)
        {
                return hwDMA_OK;
        }

        HAL_DMA_DeInit(&g_dma[stream_index]);

        switch(stream_index)
        {
#if defined (DMA1_BASE)
                case hwDMA_Stream_Index_0:
                        HAL_NVIC_DisableIRQ(DMA1_Stream0_IRQn);
                        break;
                case hwDMA_Stream_Index_1:
                        HAL_NVIC_DisableIRQ(DMA1_Stream1_IRQn);
                        break;
                case hwDMA_Stream_Index_2:
                        HAL_NVIC_DisableIRQ(DMA1_Stream2_IRQn);
                        break;
                case hwDMA_Stream_Index_3:
                        HAL_NVIC_DisableIRQ(DMA1_Stream3_IRQn);
                        break;
                case hwDMA_Stream_Index_4:
                        HAL_NVIC_DisableIRQ(DMA1_Stream4_IRQn);
                        break;
                case hwDMA_Stream_Index_5:
                        HAL_NVIC_DisableIRQ(DMA1_Stream5_IRQn);
                        break;
                case hwDMA_Stream_Index_6:
                        HAL_NVIC_DisableIRQ(DMA1_Stream6_IRQn);
                        break;
                case hwDMA_Stream_Index_7:
                        HAL_NVIC_DisableIRQ(DMA1_Stream7_IRQn);
                        break;
#endif //DMA1_BASE
#if defined (DMA2_BASE)
                case hwDMA_Stream_Index_8:
                        HAL_NVIC_DisableIRQ(DMA2_Stream0_IRQn);
                        break;
                case hwDMA_Stream_Index_9:
                        HAL_NVIC_DisableIRQ(DMA2_Stream1_IRQn);
                        break;
                case hwDMA_Stream_Index_10:
                        HAL_NVIC_DisableIRQ(DMA2_Stream2_IRQn);
                        break;
                case hwDMA_Stream_Index_11:
                        HAL_NVIC_DisableIRQ(DMA2_Stream3_IRQn);
                        break;
                case hwDMA_Stream_Index_12:
                        HAL_NVIC_DisableIRQ(DMA2_Stream4_IRQn);
                        break;
                case hwDMA_Stream_Index_13:
                        HAL_NVIC_DisableIRQ(DMA2_Stream5_IRQn);
                        break;
                case hwDMA_Stream_Index_14:
                        HAL_NVIC_DisableIRQ(DMA2_Stream6_IRQn);
                        break;
                case hwDMA_Stream_Index_15:
                        HAL_NVIC_DisableIRQ(DMA2_Stream7_IRQn);
                        break;
#endif //DMA2_BASE
#if defined (BDMA_BASE)
                case hwDMA_Stream_Index_16:
                        HAL_NVIC_DisableIRQ(BDMA_Channel0_IRQn);
                        break;
                case hwDMA_Stream_Index_17:
                        HAL_NVIC_DisableIRQ(BDMA_Channel1_IRQn);
                        break;
                case hwDMA_Stream_Index_18:
                        HAL_NVIC_DisableIRQ(BDMA_Channel2_IRQn);
                        break;
                case hwDMA_Stream_Index_19:
                        HAL_NVIC_DisableIRQ(BDMA_Channel3_IRQn);
                        break;
                case hwDMA_Stream_Index_20:
                        HAL_NVIC_DisableIRQ(BDMA_Channel4_IRQn);
                        break;
                case hwDMA_Stream_Index_21:
                        HAL_NVIC_DisableIRQ(BDMA_Channel5_IRQn);
                        break;
                case hwDMA_Stream_Index_22:
                        HAL_NVIC_DisableIRQ(BDMA_Channel6_IRQn);
                        break;
                case hwDMA_Stream_Index_23:
                        HAL_NVIC_DisableIRQ(BDMA_Channel7_IRQn);
                        break;
#endif //BDMA_BASE
        }

        return hwDMA_OK;
}

#endif //STM32
