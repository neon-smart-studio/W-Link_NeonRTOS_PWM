
#ifndef UART_PIN_TM4C1294_H
#define UART_PIN_TM4C1294_H

#include "GPIO/GPIO.h"

#define UART0_TX		        hwGPIO_Pin_A1
#define UART0_RX		        hwGPIO_Pin_A0

#define UART1_TX		        hwGPIO_Pin_B1
#define UART1_RX		        hwGPIO_Pin_B0

#define UART2_TX		        hwGPIO_Pin_A7
#define UART2_RX		        hwGPIO_Pin_A6

#define UART3_TX		        hwGPIO_Pin_A5
#define UART3_RX		        hwGPIO_Pin_A4

#define UART4_TX		        hwGPIO_Pin_K1
#define UART4_RX		        hwGPIO_Pin_K0

#define UART5_TX		        hwGPIO_Pin_C7
#define UART5_RX		        hwGPIO_Pin_C6

#define UART6_TX		        hwGPIO_Pin_P1
#define UART6_RX		        hwGPIO_Pin_P0

#define UART7_TX		        hwGPIO_Pin_C5
#define UART7_RX		        hwGPIO_Pin_C4

const hwGPIO_Pin UART_TX_Pins[] = {UART0_TX, UART1_TX, UART2_TX, UART3_TX, UART4_TX, UART5_TX, UART6_TX, UART7_TX};
const hwGPIO_Pin UART_RX_Pins[] = {UART0_RX, UART1_RX, UART2_RX, UART3_RX, UART4_RX, UART5_RX, UART6_RX, UART7_RX};

#endif //UART_PIN_TM4C1294_H