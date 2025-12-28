
#ifndef DMA_INDEX_H
#define DMA_INDEX_H

#include "soc.h"

typedef enum hwDMA_Stream_Index_t
{
#if defined (DMA1_BASE)
  hwDMA_Stream_Index_0,
  hwDMA_Stream_Index_1,
  hwDMA_Stream_Index_2,
  hwDMA_Stream_Index_3,
  hwDMA_Stream_Index_4,
  hwDMA_Stream_Index_5,
  hwDMA_Stream_Index_6,
  hwDMA_Stream_Index_7,
#endif //DMA1_BASE
#if defined (DMA2_BASE)
  hwDMA_Stream_Index_8,
  hwDMA_Stream_Index_9,
  hwDMA_Stream_Index_10,
  hwDMA_Stream_Index_11,
  hwDMA_Stream_Index_12,
  hwDMA_Stream_Index_13,
  hwDMA_Stream_Index_14,
  hwDMA_Stream_Index_15,
#endif //DMA2_BASE
#if defined (BDMA_BASE)
  hwDMA_Stream_Index_16,
  hwDMA_Stream_Index_17,
  hwDMA_Stream_Index_18,
  hwDMA_Stream_Index_19,
  hwDMA_Stream_Index_20,
  hwDMA_Stream_Index_21,
  hwDMA_Stream_Index_22,
  hwDMA_Stream_Index_23,
#endif //BDMA_BASE
  hwDMA_Stream_Index_MAX,
}hwDMA_Stream_Index;

#endif //DMA_INDEX_H