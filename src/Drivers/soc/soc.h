
#ifndef SOC_H
#define SOC_H

#ifdef DEVICE_TM4C1294
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_emac.h"
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/debug.h"
#include "driverlib/emac.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/i2c.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/flash.h"
#endif //DEVICE_TM4C1294

#if defined (STM32F205xx) || defined (STM32F215xx) || defined (STM32F207xx) || defined (STM32F217xx)
#include "stm32f2xx.h"
#include "stm32f2xx_hal_gpio.h"
#include "stm32f2xx_hal_exti.h"
#include "stm32f2xx_hal_uart.h"
#include "stm32f2xx_hal_spi.h"
#include "stm32f2xx_hal_i2c.h"
#include "stm32f2xx_hal_dma.h"
#include "stm32f2xx_hal_rtc.h"
#include "stm32f2xx_hal_adc.h"
#include "stm32f2xx_hal_can.h"
#include "stm32f2xx_hal_dac.h"
#define STM32
#endif //STM32F2

#if defined (STM32F756xx) || defined (STM32F746xx) || defined (STM32F745xx) || defined (STM32F765xx) || \
    defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx) || \
    defined (STM32F722xx) || defined (STM32F723xx) || defined (STM32F732xx) || defined (STM32F733xx) || \
    defined (STM32F730xx) || defined (STM32F750xx)
#include "stm32f7xx.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_exti.h"
#include "stm32f7xx_hal_uart.h"
#include "stm32f7xx_hal_spi.h"
#include "stm32f7xx_hal_i2c.h"
#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_rtc.h"
#include "stm32f7xx_hal_adc.h"
#include "stm32f7xx_hal_can.h"
#include "stm32f7xx_hal_dac.h"
#define STM32
#endif //STM32F7

#if defined (STM32H743xx) || defined (STM32H753xx)  || defined (STM32H750xx) || defined (STM32H742xx)  || \
    defined (STM32H745xx) || defined (STM32H745xG)  || defined (STM32H755xx) || defined (STM32H747xx)  || defined (STM32H747xG) || defined (STM32H757xx) || \
    defined (STM32H7A3xx) || defined (STM32H7A3xxQ) || defined (STM32H7B3xx) || defined (STM32H7B3xxQ) || defined (STM32H7B0xx) || defined (STM32H7B0xxQ) || \
    defined (STM32H735xx) || defined (STM32H733xx)  || defined (STM32H730xx) || defined (STM32H730xxQ) || defined (STM32H725xx) || defined (STM32H723xx)
#include "stm32h7xx.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_exti.h"
#include "stm32h7xx_hal_uart.h"
#include "stm32h7xx_hal_spi.h"
#include "stm32h7xx_hal_i2c.h"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_rtc.h"
#include "stm32h7xx_hal_adc.h"
#include "stm32h7xx_hal_can.h"
#include "stm32h7xx_hal_dac.h"
#define STM32
#endif //STM32H7

#if defined (STM32F0) || defined (STM32F7) || defined (STM32F3) || defined (STM32G0) || \
    defined (STM32G4) || defined (STM32H7) || defined (STM32L0) || defined (STM32L4) || \
    defined (STM32L5) || defined (STM32U5) || defined (STM32WB) || defined (STM32WL)
#define STM32_I2C_NEW_ARCH
#endif
#if defined (STM32F1) || defined (STM32F2) || defined (STM32F4) || defined (STM32L1)
#define STM32_I2C_LEGACY_ARCH
#endif

#if defined (STM32F0) || defined (STM32F3) || defined (STM32G0) || \
    defined (STM32G4) || defined (STM32H7) || defined (STM32L0) || defined (STM32L4) || \
    defined (STM32L5) || defined (STM32U5) || defined (STM32WB) || defined (STM32WL)
#define STM32_DMA_NEW_ARCH
#endif
#if defined (STM32F1) || defined (STM32F2) || defined (STM32F4) || defined (STM32F7) || defined (STM32L1)
#define STM32_DMA_LEGACY_ARCH
#endif

#endif //SOC_H