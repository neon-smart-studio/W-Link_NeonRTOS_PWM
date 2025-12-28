
#ifndef UART_H
#define UART_H

#include <stdbool.h>

#include "soc.h"

#include "UART_Index.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum hwUART_OpResult_t
{
  hwUART_OK = 0,
  hwUART_NotInit = -1,
  hwUART_InvalidParameter = -2,
  hwUART_Busy = -3,
  hwUART_MemoryError = -4,
  hwUART_MutexTimeout = -5,
  hwUART_BusError = -6,
  hwUART_HwError = -7,
  hwUART_Unsupport = -8,
}hwUART_OpResult;

typedef enum {
  UART_Parity_None = 0,		/*!<parity disable */
  UART_Parity_Odd = 1,		/*!<odd parity enable */
  UART_Parity_Even = 2,		/*!<even paroty enable */
  UART_Parity_MAX = 3,		/*!<even paroty enable */
} UART_Parity;

typedef int(* onSendCompleteEventCallback)(hwUART_Index index);
typedef int(* onRecvCompleteEventCallback)(hwUART_Index index);

hwUART_OpResult UART_Open(hwUART_Index index, uint32_t baudrate, bool rts_cts);
hwUART_OpResult UART_Open_Specific_Format(hwUART_Index index, uint32_t baudrate, bool rts_cts, uint8_t data_bits, UART_Parity parity, uint8_t stop_bits);
hwUART_OpResult UART_Close(hwUART_Index index);
hwUART_OpResult UART_Read(hwUART_Index index, uint8_t* data_rd, size_t size, uint32_t timeoutMs);
hwUART_OpResult UART_GetChar(hwUART_Index index, uint8_t* char_rd, uint32_t timeoutMs);
hwUART_OpResult UART_Write(hwUART_Index index, uint8_t* data_wr, size_t size, uint32_t timeoutMs);
hwUART_OpResult UART_PutChar(hwUART_Index index, uint8_t char_wr, uint32_t timeoutMs);
void UART_Printf(const char *format, ...);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif //UART_H