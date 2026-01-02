
#ifndef CAN_H
#define CAN_H

#include "CAN_Index.h"

#include "NeonRTOS.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum hwCAN_OpResult_t
{
  hwCAN_OK = 0,
  hwCAN_NotInit = -1,
  hwCAN_InvalidParameter = -2,
  hwCAN_HwError = -4,
  hwCAN_MemoryError = -5,
  hwCAN_MutexTimeout = -6,
  hwCAN_Timeout = -7,
  hwCAN_BusError = -8,
  hwCAN_Unsupport = -9,
} hwCAN_OpResult;

hwCAN_OpResult CAN_Init(hwCAN_Index index);
hwCAN_OpResult CAN_DeInit(hwCAN_Index index);
hwCAN_OpResult CAN_Read(hwCAN_Index index, uint8_t *buf, uint32_t timeout);
hwCAN_OpResult CAN_Write(hwCAN_Index index, uint32_t id, uint8_t *data, uint8_t len, uint32_t timeout);
bool CAN_isInit(hwCAN_Index index);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // CAN_H