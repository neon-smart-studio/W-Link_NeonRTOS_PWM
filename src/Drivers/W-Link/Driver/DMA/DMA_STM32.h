
#ifndef DMA_STM32_H
#define DMA_STM32_H

#include "soc.h"

#include "DMA_Index.h"

extern DMA_HandleTypeDef g_dma[hwDMA_Stream_Index_MAX];

DMA_TypeDef * DMA_Map_Soc_Base(hwDMA_Stream_Index index);
#if defined (BDMA_BASE)
BDMA_TypeDef * BDMA_Map_Soc_Base(hwDMA_Stream_Index index)
#endif //BDMA_BASE

DMA_Stream_TypeDef * DMA_Map_Soc_Stream_Base(hwDMA_Stream_Index index);
#if defined (BDMA_BASE)
BDMA_Channel_TypeDef * BDMA_Map_Soc_Channel_Base(hwDMA_Stream_Index index);
#endif //BDMA_BASE

#endif //DMA_STM32_H