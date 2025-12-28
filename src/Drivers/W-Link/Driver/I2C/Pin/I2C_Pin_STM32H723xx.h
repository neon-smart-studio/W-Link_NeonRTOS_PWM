
#ifndef SPI_PIN_STM32H723xx_H
#define SPI_PIN_STM32H723xx_H

#include "I2C_Pin_STM32.h"

typedef enum {
    I2C_Pinset_DEFAULT = 0,
    I2C_Pinset_ALT,
    I2C_Pinset_MAX
} I2C_Pinset_t;

#ifndef CONFIG_I2C0_PINSET
#define CONFIG_I2C0_PINSET I2C_Pinset_DEFAULT
#endif //CONFIG_I2C0_PINSET

#ifndef CONFIG_I2C1_PINSET
#define CONFIG_I2C1_PINSET I2C_Pinset_DEFAULT
#endif //CONFIG_I2C1_PINSET

#ifndef CONFIG_I2C2_PINSET
#define CONFIG_I2C2_PINSET I2C_Pinset_DEFAULT
#endif //CONFIG_I2C2_PINSET

const I2C_Pinset_t I2C_Index_Map_Alt[hwI2C_Index_MAX] = {
    CONFIG_I2C0_PINSET,
    CONFIG_I2C1_PINSET,
    CONFIG_I2C2_PINSET
};

const I2C_Pin_Def I2C_Pin_Def_Table[hwI2C_Index_MAX][I2C_Pinset_MAX] =
{
    /* ================= I2C1 ================= */
    {
        { hwGPIO_Pin_B6,  hwGPIO_Pin_B9 },
        { hwGPIO_Pin_B8,  hwGPIO_Pin_B7 },
    },

    /* ================= I2C2 ================= */
    {
        { hwGPIO_Pin_B10, hwGPIO_Pin_B11 },
        { hwGPIO_Pin_F1,  hwGPIO_Pin_F0  },
    },

    /* ================= I2C3 ================= */
    {
        { hwGPIO_Pin_A8,  hwGPIO_Pin_C9 },
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC },
    },

    /* ================= I2C4 ================= */
    {
        { hwGPIO_Pin_D12, hwGPIO_Pin_D13 },
        { hwGPIO_Pin_F14, hwGPIO_Pin_F15 },
    },

    /* ================= I2C5 ================= */
    {
        { hwGPIO_Pin_C11, hwGPIO_Pin_C10 },
        { hwGPIO_Pin_F1,  hwGPIO_Pin_F0 },
    },
};

const I2C_AF_Map I2C_Pin_AF_Map[] =
{
    /* ---------- I2C1 ---------- */
    { hwI2C_Index_0, hwGPIO_Pin_B6, GPIO_AF4_I2C1 },
    { hwI2C_Index_0, hwGPIO_Pin_B7, GPIO_AF4_I2C1 },
    { hwI2C_Index_0, hwGPIO_Pin_B8, GPIO_AF4_I2C1 },
    { hwI2C_Index_0, hwGPIO_Pin_B9, GPIO_AF4_I2C1 },

    /* ---------- I2C2 ---------- */
    { hwI2C_Index_1, hwGPIO_Pin_B10, GPIO_AF4_I2C2 },
    { hwI2C_Index_1, hwGPIO_Pin_B11, GPIO_AF4_I2C2 },
    { hwI2C_Index_1, hwGPIO_Pin_F0, GPIO_AF4_I2C2 },
    { hwI2C_Index_1, hwGPIO_Pin_F1, GPIO_AF4_I2C2 },

    /* ---------- I2C3 ---------- */
    { hwI2C_Index_2, hwGPIO_Pin_A8, GPIO_AF4_I2C3 },
    { hwI2C_Index_2, hwGPIO_Pin_C9, GPIO_AF4_I2C3 },

    /* ---------- I2C4 ---------- */
    { hwI2C_Index_3, hwGPIO_Pin_B6,  GPIO_AF6_I2C4 },
    { hwI2C_Index_3, hwGPIO_Pin_B7,  GPIO_AF6_I2C4 },
    { hwI2C_Index_3, hwGPIO_Pin_B8,  GPIO_AF6_I2C4 },
    { hwI2C_Index_3, hwGPIO_Pin_B9,  GPIO_AF6_I2C4 },
    { hwI2C_Index_3, hwGPIO_Pin_D12, GPIO_AF4_I2C4 },
    { hwI2C_Index_3, hwGPIO_Pin_D13, GPIO_AF4_I2C4 },
    { hwI2C_Index_3, hwGPIO_Pin_F14, GPIO_AF4_I2C4 },
    { hwI2C_Index_3, hwGPIO_Pin_F15, GPIO_AF4_I2C4 },

    /* ---------- I2C5 ---------- */
    { hwI2C_Index_4, hwGPIO_Pin_B6,  GPIO_AF6_I2C5 },
    { hwI2C_Index_4, hwGPIO_Pin_B7,  GPIO_AF6_I2C5 },
    { hwI2C_Index_4, hwGPIO_Pin_B8,  GPIO_AF6_I2C5 },
    { hwI2C_Index_4, hwGPIO_Pin_B9,  GPIO_AF6_I2C5 },
    { hwI2C_Index_4, hwGPIO_Pin_D12, GPIO_AF6_I2C5 },
    { hwI2C_Index_4, hwGPIO_Pin_D13, GPIO_AF6_I2C5 },
};

#endif //SPI_PIN_STM32H723xx_H