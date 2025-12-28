
#ifndef UART_PIN_STM32F746xx_H
#define UART_PIN_STM32F746xx_H

#include "UART_Pin_STM32.h"

typedef enum {
    UART_Pinset_DEFAULT = 0,
    UART_Pinset_ALT1,
    UART_Pinset_ALT2,
    UART_Pinset_MAX
} UART_Pinset_t;

#ifndef CONFIG_UART0_PINSET
#define CONFIG_UART0_PINSET UART_Pinset_DEFAULT
#endif //CONFIG_UART0_PINSET

#ifndef CONFIG_UART1_PINSET
#define CONFIG_UART1_PINSET UART_Pinset_DEFAULT
#endif //CONFIG_UART1_PINSET

#ifndef CONFIG_UART2_PINSET
#define CONFIG_UART2_PINSET UART_Pinset_DEFAULT
#endif //CONFIG_UART2_PINSET

#ifndef CONFIG_UART3_PINSET
#define CONFIG_UART3_PINSET UART_Pinset_DEFAULT
#endif //CONFIG_UART3_PINSET

#ifndef CONFIG_UART4_PINSET
#define CONFIG_UART4_PINSET UART_Pinset_DEFAULT
#endif //CONFIG_UART4_PINSET

#ifndef CONFIG_UART5_PINSET
#define CONFIG_UART5_PINSET UART_Pinset_DEFAULT
#endif //CONFIG_UART5_PINSET

#ifndef CONFIG_UART6_PINSET
#define CONFIG_UART6_PINSET UART_Pinset_DEFAULT
#endif //CONFIG_UART6_PINSET

#ifndef CONFIG_UART7_PINSET
#define CONFIG_UART7_PINSET UART_Pinset_DEFAULT
#endif //CONFIG_UART7_PINSET

const UART_Pinset_t UART_Index_Map_Alt[hwUART_Index_MAX] = {
    CONFIG_UART0_PINSET,
    CONFIG_UART1_PINSET,
    CONFIG_UART2_PINSET,
    CONFIG_UART3_PINSET,
    CONFIG_UART4_PINSET,
    CONFIG_UART5_PINSET,
    CONFIG_UART6_PINSET,
    CONFIG_UART7_PINSET,
};

const UART_Pin_Def UART_Pin_Def_Table[hwUART_Index_MAX][UART_Pinset_MAX] =
{
    /* USART1 (UART0) */
    {
        { hwGPIO_Pin_A9,  hwGPIO_Pin_A10, hwGPIO_Pin_A12, hwGPIO_Pin_A11 },   // DEFAULT
        { hwGPIO_Pin_B6,  hwGPIO_Pin_B7,  hwGPIO_Pin_A12, hwGPIO_Pin_A11 },   // ALT1
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC  },
    },

    /* USART2 (UART1) */
    {
        { hwGPIO_Pin_A2,  hwGPIO_Pin_A3,  hwGPIO_Pin_A1,  hwGPIO_Pin_A0  },
        { hwGPIO_Pin_D5,  hwGPIO_Pin_D6,  hwGPIO_Pin_D4,  hwGPIO_Pin_D3  },
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC  },
    },

    /* USART3 (UART2) */
    {
        { hwGPIO_Pin_D8,  hwGPIO_Pin_D9,  hwGPIO_Pin_D12, hwGPIO_Pin_D11 },
        { hwGPIO_Pin_C10, hwGPIO_Pin_C11, hwGPIO_Pin_B14, hwGPIO_Pin_B13 },
        { hwGPIO_Pin_B10, hwGPIO_Pin_B11, hwGPIO_Pin_B14, hwGPIO_Pin_B13 },
    },

    /* UART4 (UART3) */
    {
        { hwGPIO_Pin_C10, hwGPIO_Pin_C11, hwGPIO_Pin_A15, hwGPIO_Pin_B0 },
        { hwGPIO_Pin_A0,  hwGPIO_Pin_A1,  hwGPIO_Pin_A15, hwGPIO_Pin_B0 },
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC },
    },

    /* UART5 (UART4) */
    {
        { hwGPIO_Pin_C12, hwGPIO_Pin_D2,  hwGPIO_Pin_C8,  hwGPIO_Pin_C9 },
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC },
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC },
    },

    /* USART6 (UART5) */
    {
        { hwGPIO_Pin_G14, hwGPIO_Pin_G9,  hwGPIO_Pin_G12, hwGPIO_Pin_G15 },
        { hwGPIO_Pin_C6,  hwGPIO_Pin_C7,  hwGPIO_Pin_G8,  hwGPIO_Pin_G13 },
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC },
    },

    /* UART7 (UART6) */
    {
        { hwGPIO_Pin_E8,  hwGPIO_Pin_E7,  hwGPIO_Pin_E9,  hwGPIO_Pin_E10 },
        { hwGPIO_Pin_F7,  hwGPIO_Pin_F6,  hwGPIO_Pin_F8,  hwGPIO_Pin_F9  },
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC },
    },

    /* UART8 (UART7) */
    {
        { hwGPIO_Pin_E1,  hwGPIO_Pin_E0,  hwGPIO_Pin_D15, hwGPIO_Pin_D14 },
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC },
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC },
    },
};

const UART_AF_Map UART_Pin_AF_Map[] = {
    { hwUART_Index_0, hwGPIO_Pin_A9,  GPIO_AF7_USART1 },
    { hwUART_Index_0, hwGPIO_Pin_A10, GPIO_AF7_USART1 },
    { hwUART_Index_0, hwGPIO_Pin_A11, GPIO_AF7_USART1 },
    { hwUART_Index_0, hwGPIO_Pin_A12, GPIO_AF7_USART1 },
    { hwUART_Index_0, hwGPIO_Pin_B6,  GPIO_AF7_USART1 },
    { hwUART_Index_0, hwGPIO_Pin_B7,  GPIO_AF7_USART1 },

    { hwUART_Index_1, hwGPIO_Pin_A2,  GPIO_AF7_USART2 },
    { hwUART_Index_1, hwGPIO_Pin_A3,  GPIO_AF7_USART2 },
    { hwUART_Index_1, hwGPIO_Pin_A0,  GPIO_AF7_USART2 },
    { hwUART_Index_1, hwGPIO_Pin_A1,  GPIO_AF7_USART2 },
    { hwUART_Index_1, hwGPIO_Pin_D5,  GPIO_AF7_USART2 },
    { hwUART_Index_1, hwGPIO_Pin_D6,  GPIO_AF7_USART2 },
    { hwUART_Index_1, hwGPIO_Pin_D3,  GPIO_AF7_USART2 },
    { hwUART_Index_1, hwGPIO_Pin_D4,  GPIO_AF7_USART2 },

    { hwUART_Index_2, hwGPIO_Pin_D8,  GPIO_AF7_USART3 },
    { hwUART_Index_2, hwGPIO_Pin_D9,  GPIO_AF7_USART3 },
    { hwUART_Index_2, hwGPIO_Pin_D11, GPIO_AF7_USART3 },
    { hwUART_Index_2, hwGPIO_Pin_D12, GPIO_AF7_USART3 },
    { hwUART_Index_2, hwGPIO_Pin_B10, GPIO_AF7_USART3 },
    { hwUART_Index_2, hwGPIO_Pin_B11, GPIO_AF7_USART3 },
    { hwUART_Index_2, hwGPIO_Pin_B13, GPIO_AF7_USART3 },
    { hwUART_Index_2, hwGPIO_Pin_B14, GPIO_AF7_USART3 },

    { hwUART_Index_3, hwGPIO_Pin_C10, GPIO_AF8_UART4 },
    { hwUART_Index_3, hwGPIO_Pin_C11, GPIO_AF8_UART4 },
    { hwUART_Index_3, hwGPIO_Pin_A0,  GPIO_AF8_UART4 },
    { hwUART_Index_3, hwGPIO_Pin_A1,  GPIO_AF8_UART4 },

    { hwUART_Index_4, hwGPIO_Pin_C12, GPIO_AF8_UART5 },
    { hwUART_Index_4, hwGPIO_Pin_D2,  GPIO_AF8_UART5 },

    { hwUART_Index_5, hwGPIO_Pin_C6,  GPIO_AF8_USART6 },
    { hwUART_Index_5, hwGPIO_Pin_C7,  GPIO_AF8_USART6 },
    { hwUART_Index_5, hwGPIO_Pin_G14, GPIO_AF8_USART6 },
    { hwUART_Index_5, hwGPIO_Pin_G9,  GPIO_AF8_USART6 },
    { hwUART_Index_5, hwGPIO_Pin_G12, GPIO_AF8_USART6 },
    { hwUART_Index_5, hwGPIO_Pin_G13, GPIO_AF8_USART6 },
    
    { hwUART_Index_6, hwGPIO_Pin_E8,  GPIO_AF8_UART7 },
    { hwUART_Index_6, hwGPIO_Pin_F7,  GPIO_AF8_UART7 },
    { hwUART_Index_6, hwGPIO_Pin_E7,  GPIO_AF8_UART7 },
    { hwUART_Index_6, hwGPIO_Pin_F6,  GPIO_AF8_UART7 },
    { hwUART_Index_6, hwGPIO_Pin_E9,  GPIO_AF8_UART7 },
    { hwUART_Index_6, hwGPIO_Pin_F8,  GPIO_AF8_UART7 },
    { hwUART_Index_6, hwGPIO_Pin_E10, GPIO_AF8_UART7 },
    { hwUART_Index_6, hwGPIO_Pin_F9,  GPIO_AF8_UART7 },
    
    { hwUART_Index_7, hwGPIO_Pin_E1,  GPIO_AF8_UART8 },
    { hwUART_Index_7, hwGPIO_Pin_E0,  GPIO_AF8_UART8 },
    { hwUART_Index_7, hwGPIO_Pin_D15, GPIO_AF8_UART8 },
    { hwUART_Index_7, hwGPIO_Pin_D14, GPIO_AF8_UART8 },
};

#endif //UART_PIN_STM32F746xx_H