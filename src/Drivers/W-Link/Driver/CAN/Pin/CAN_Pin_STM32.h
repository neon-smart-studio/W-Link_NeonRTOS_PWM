
#ifndef CAN_PIN_STM32_H
#define CAN_PIN_STM32_H

#include "GPIO/GPIO.h"

#include "CAN/CAN.h"

typedef struct {
    hwGPIO_Pin tx_pin;
    hwGPIO_Pin rx_pin;
} CAN_Pin_Def;

typedef struct {
    hwCAN_Index can;
    hwGPIO_Pin pin;
    uint32_t af;
} CAN_AF_Map;

#endif //CAN_PIN_STM32_H