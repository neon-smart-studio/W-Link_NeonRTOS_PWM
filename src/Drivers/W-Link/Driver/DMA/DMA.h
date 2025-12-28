
#ifndef DMA_H
#define DMA_H

#include "DMA_Index.h"

typedef enum hwDMA_Peripheral_Direction_t
{
  hwDMA_Peripheral_Direction_RX = 0,
  hwDMA_Peripheral_Direction_TX,
  hwDMA_Peripheral_Direction_MAX
}hwDMA_Peripheral_Direction;

typedef enum hwDMA_OpMode_t
{
  hwDMA_Mode_NotInit = -1,
  hwDMA_Mode_Idle = 0,
  hwDMA_Mode_OneShout = 1,
  hwDMA_Mode_Period = 2,
}hwDMA_OpMode_t;

typedef enum hwDMA_OpResult_t
{
  hwDMA_OK = 0,
  hwDMA_NotInit = -1,
  hwDMA_InvalidParameter = -2,
  hwDMA_MemoryError = -3,
  hwDMA_MutexTimeout = -4,
  hwDMA_XferTimeout = -5,
  hwDMA_Unsupport = -6,
}hwDMA_OpResult;

hwDMA_OpResult DMA_Stream_Init(hwDMA_Stream_Index stream_index);
hwDMA_OpResult DMA_Stream_DeInit(hwDMA_Stream_Index stream_index);

#endif //DMA_H