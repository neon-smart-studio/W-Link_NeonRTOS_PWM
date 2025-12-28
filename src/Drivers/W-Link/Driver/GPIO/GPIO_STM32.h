
#ifndef GPIO_STM32_H
#define GPIO_STM32_H

#include "soc.h"

#include "Pin/GPIO_Pin.h"

#ifdef	__cplusplus
extern "C" {
#endif

extern bool gpio_pin_init_status[hwGPIO_Pin_MAX];
void GPIO_Enable_RCC_Clock(GPIO_TypeDef * base);
void GPIO_Disable_RCC_Clock(GPIO_TypeDef * base);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif //GPIO_STM32_H