
#ifndef SPI_MASTER_H
#define SPI_MASTER_H

#include "SPI_Index.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum hwSPI_OpMode_t
{
  hwSPI_OpMode_Polarity0_Phase0 = 0,
  hwSPI_OpMode_Polarity0_Phase1 = 1,
  hwSPI_OpMode_Polarity1_Phase0 = 2,
  hwSPI_OpMode_Polarity1_Phase1 = 3,
  hwSPI_OpMode_MAX = 4,
}hwSPI_OpMode;

typedef enum hwSPI_OpResult_t
{
  hwSPI_OK = 0,
  hwSPI_NotInit = -1,
  hwSPI_InvalidParameter = -2,
  hwSPI_MemoryError = -3,
  hwSPI_MutexTimeout = -4,
  hwSPI_SlaveTimeout = -5,
  hwSPI_Unsupport = -6,
}hwSPI_OpResult;

hwSPI_OpResult SPI_Master_DummyByte(hwSPI_Index index);
hwSPI_OpResult SPI_Master_WriteByte(hwSPI_Index index, uint8_t dat);
hwSPI_OpResult SPI_Master_ReadByte(hwSPI_Index index, uint8_t* dat);
hwSPI_OpResult SPI_Master_TransferByte(hwSPI_Index index, uint8_t wr_dat, uint8_t* rd_dat);
hwSPI_OpResult SPI_Master_Init(hwSPI_Index index, uint32_t clock_rate_hz, hwSPI_OpMode opMode);
hwSPI_OpResult SPI_Master_DeInit(hwSPI_Index index);
hwSPI_OpResult SPI_Change_Frequency(hwSPI_Index index, uint32_t clock_rate_hz);
hwSPI_OpResult SPI_Change_Mode(hwSPI_Index index, hwSPI_OpMode opMode);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif //SPI_MASTER_H