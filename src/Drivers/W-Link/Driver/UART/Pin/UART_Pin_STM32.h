
#ifndef UART_PIN_STM32_H
#define UART_PIN_STM32_H

#include "GPIO/GPIO.h"

#include "UART/UART.h"

typedef struct { 
    hwGPIO_Pin tx_pin;
    hwGPIO_Pin rx_pin;
    hwGPIO_Pin rts_pin; // 可為 NC
    hwGPIO_Pin cts_pin; // 可為 NC
} UART_Pin_Def;

typedef struct {
    hwUART_Index uart;
    hwGPIO_Pin pin;
    uint32_t af;
} UART_AF_Map;

#endif //UART_PIN_STM32_H