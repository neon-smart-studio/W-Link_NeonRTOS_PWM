
#ifndef SPI_PIN_STM32_H
#define SPI_PIN_STM32_H

#include "GPIO/GPIO.h"

#include "SPI/SPI_Master.h"

typedef struct { 
    hwGPIO_Pin mosi_pin;
    hwGPIO_Pin miso_pin;
    hwGPIO_Pin sclk_pin;
    hwGPIO_Pin cs_pin; // 可為 NC
} SPI_Pin_Def;

typedef struct {
    hwSPI_Index spi;
    hwGPIO_Pin pin;
    uint32_t af;
} SPI_AF_Map;

#endif //SPI_PIN_STM32_H