
#ifndef SPI_PIN_STM32F746xx_H
#define SPI_PIN_STM32F746xx_H

#include "CAN_Pin_STM32.h"

typedef enum {
    CAN_Pinset_DEFAULT = 0,
    CAN_Pinset_ALT1,
    CAN_Pinset_ALT2,
    CAN_Pinset_MAX
} CAN_Pinset_t;

#ifndef CONFIG_CAN0_PINSET
#define CONFIG_CAN0_PINSET CAN_Pinset_DEFAULT
#endif //CONFIG_CAN0_PINSET

#ifndef CONFIG_CAN1_PINSET
#define CONFIG_CAN1_PINSET CAN_Pinset_DEFAULT
#endif //CONFIG_CAN1_PINSET

const CAN_Pinset_t CAN_Index_Map_Alt[hwCAN_Index_MAX] = {
    CONFIG_CAN0_PINSET,
    CONFIG_CAN1_PINSET
};

const CAN_Pin_Def CAN_Pin_Def_Table[hwCAN_Index_MAX][CAN_Pinset_MAX] =
{
    /* ================= CAN1 (CAN0) ================= */
    {
        /* DEFAULT */
        { hwGPIO_Pin_A12, hwGPIO_Pin_A11 },

        /* ALT */
        { hwGPIO_Pin_B9, hwGPIO_Pin_B8 },

        /* ALT */
        { hwGPIO_Pin_D1, hwGPIO_Pin_D0 },
    },

    /* ================= CAN2 (CAN1) ================= */
    {
        /* DEFAULT */
        { hwGPIO_Pin_B6,  hwGPIO_Pin_B5 },

        /* ALT */
        { hwGPIO_Pin_B13, hwGPIO_Pin_B12 },

        /* ALT */
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC },
    },
    
    /* ================= CAN3 (CAN2) ================= */
    {
        /* DEFAULT */
        { hwGPIO_Pin_A15, hwGPIO_Pin_A8 },

        /* ALT */
        { hwGPIO_Pin_B4,  hwGPIO_Pin_B3 },

        /* ALT */
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC },
    },
};

static const CAN_AF_Map CAN_Pin_AF_Map[] = {
    // CAN1
    { hwCAN_Index_0, hwGPIO_Pin_A12, GPIO_AF9_CAN1 },
    { hwCAN_Index_0, hwGPIO_Pin_A11, GPIO_AF9_CAN1 },
    { hwCAN_Index_0, hwGPIO_Pin_B8,  GPIO_AF9_CAN1 },
    { hwCAN_Index_0, hwGPIO_Pin_B9,  GPIO_AF9_CAN1 },
    { hwCAN_Index_0, hwGPIO_Pin_D1,  GPIO_AF9_CAN1 },
    { hwCAN_Index_0, hwGPIO_Pin_D0,  GPIO_AF9_CAN1 },

    // CAN2
    { hwCAN_Index_1, hwGPIO_Pin_B6,  GPIO_AF9_CAN2 },
    { hwCAN_Index_1, hwGPIO_Pin_B5,  GPIO_AF9_CAN2 },
    { hwCAN_Index_1, hwGPIO_Pin_B13, GPIO_AF9_CAN2 },
    { hwCAN_Index_1, hwGPIO_Pin_B12, GPIO_AF9_CAN2 },
    
    // CAN3
    { hwCAN_Index_2, hwGPIO_Pin_A15, GPIO_AF11_CAN3 },
    { hwCAN_Index_2, hwGPIO_Pin_A8,  GPIO_AF11_CAN3 },
    { hwCAN_Index_2, hwGPIO_Pin_B3,  GPIO_AF11_CAN3 },
    { hwCAN_Index_2, hwGPIO_Pin_B4,  GPIO_AF11_CAN3 },
};

#endif //SPI_PIN_STM32F746xx_H