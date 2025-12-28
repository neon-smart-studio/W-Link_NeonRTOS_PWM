
#ifndef SPI_PIN_STM32F207xx_H
#define SPI_PIN_STM32F207xx_H

#include "SPI_Pin_STM32.h"

typedef enum {
    SPI_Pinset_DEFAULT = 0,
    SPI_Pinset_ALT,
    SPI_Pinset_MAX
} SPI_Pinset_t;

#ifndef CONFIG_SPI0_PINSET
#define CONFIG_SPI0_PINSET SPI_Pinset_DEFAULT
#endif //CONFIG_SPI0_PINSET

#ifndef CONFIG_SPI1_PINSET
#define CONFIG_SPI1_PINSET SPI_Pinset_DEFAULT
#endif //CONFIG_SPI1_PINSET

#ifndef CONFIG_SPI2_PINSET
#define CONFIG_SPI2_PINSET SPI_Pinset_DEFAULT
#endif //CONFIG_SPI2_PINSET

const SPI_Pinset_t SPI_Index_Map_Alt[hwSPI_Index_MAX] = {
    CONFIG_SPI0_PINSET,
    CONFIG_SPI1_PINSET,
    CONFIG_SPI2_PINSET
};

const SPI_Pin_Def SPI_Pin_Def_Table[hwSPI_Index_MAX][SPI_Pinset_MAX] =
{
    /* ================= SPI1 (SPI0) ================= */
    {
        /* DEFAULT */
        { hwGPIO_Pin_A7, hwGPIO_Pin_A6, hwGPIO_Pin_A5, hwGPIO_Pin_A4 },

        /* ALT */
        { hwGPIO_Pin_B5, hwGPIO_Pin_B4, hwGPIO_Pin_B3, hwGPIO_Pin_A15 },
    },

    /* ================= SPI2 (SPI1) ================= */
    {
        /* DEFAULT */
        { hwGPIO_Pin_B15, hwGPIO_Pin_B14, hwGPIO_Pin_B13, hwGPIO_Pin_B12 },

        /* ALT */
        { hwGPIO_Pin_C3,  hwGPIO_Pin_C2,  hwGPIO_Pin_B10, hwGPIO_Pin_B9  },
    },

    /* ================= SPI3 (SPI2) ================= */
    {
        /* DEFAULT */
        { hwGPIO_Pin_B5,  hwGPIO_Pin_B4,  hwGPIO_Pin_B3,  hwGPIO_Pin_A4 },

        /* ALT */
        { hwGPIO_Pin_C12, hwGPIO_Pin_C11, hwGPIO_Pin_C10, hwGPIO_Pin_A15 },
    },
};

const SPI_AF_Map SPI_Pin_AF_Map[] =
{
    /* ================= SPI1 (SPI0) ================= */
    { hwSPI_Index_0, hwGPIO_Pin_A5,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_A6,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_A7,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_A4,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_A15, GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_B3,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_B4,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_B5,  GPIO_AF5_SPI1 },

    /* ================= SPI2 (SPI1) ================= */
    { hwSPI_Index_1, hwGPIO_Pin_B13, GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_B14, GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_B15, GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_B12, GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_B9,  GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_C2,  GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_C3,  GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_B10, GPIO_AF5_SPI2 },

    /* ================= SPI3 (SPI2) ================= */
    { hwSPI_Index_2, hwGPIO_Pin_B3,  GPIO_AF6_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_B4,  GPIO_AF6_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_B5,  GPIO_AF6_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_A4,  GPIO_AF6_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_A15, GPIO_AF6_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_C10, GPIO_AF6_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_C11, GPIO_AF6_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_C12, GPIO_AF6_SPI3 },
};

#endif //SPI_PIN_STM32F207xx_H