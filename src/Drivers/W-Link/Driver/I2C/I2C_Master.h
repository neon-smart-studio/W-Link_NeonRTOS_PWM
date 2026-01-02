
#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include "I2C_Index.h"

#include "NeonRTOS.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum hwI2C_Speed_Mode_t
{
  hwI2C_Standard_Mode = 0,
  hwI2C_Fast_Mode = 1,
  hwI2C_High_Speed_Mode = 2,
  hwI2C_Speed_Mode_MAX = 3,
} hwI2C_Speed_Mode;

typedef enum hwI2C_OpResult_t
{
  hwI2C_OK = 0,
  hwI2C_NotInit = -1,
  hwI2C_InvalidParameter = -2,
  hwI2C_HwError = -4,
  hwI2C_MemoryError = -5,
  hwI2C_MutexTimeout = -6,
  hwI2C_SlaveTimeout = -7,
  hwI2C_BusError = -8,
  hwI2C_Unsupport = -9,
} hwI2C_OpResult;

hwI2C_OpResult I2C_Master_Init(hwI2C_Index index, hwI2C_Speed_Mode speed_mode);
hwI2C_OpResult I2C_Master_DeInit(hwI2C_Index index);
hwI2C_OpResult I2C_Master_Reset(hwI2C_Index index);
hwI2C_OpResult I2C_Master_Read(hwI2C_Index index, uint8_t address, uint8_t *read_dat, uint8_t read_len, bool stop, NeonRTOS_Time_t timeoutMs);
hwI2C_OpResult I2C_Master_Write(hwI2C_Index index, uint8_t address, uint8_t *write_dat, uint8_t write_len, bool stop, NeonRTOS_Time_t timeoutMs);
hwI2C_OpResult I2C_Master_Read_FromISR(hwI2C_Index index, uint8_t address, uint8_t *read_dat, uint8_t read_len, bool stop, NeonRTOS_Time_t timeoutMs);
hwI2C_OpResult I2C_Master_Write_FromISR(hwI2C_Index index, uint8_t address, uint8_t *write_dat, uint8_t write_len, bool stop, NeonRTOS_Time_t timeoutMs);
bool I2C_Master_isInit(hwI2C_Index index);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // I2C_MASTER_H