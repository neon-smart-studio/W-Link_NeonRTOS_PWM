
#ifndef SPI_PIN_STM32H723xx_H
#define SPI_PIN_STM32H723xx_H

#include "SPI_Pin_STM32.h"

typedef enum {
    SPI_Pinset_DEFAULT = 0,
    SPI_Pinset_ALT1,
    SPI_Pinset_ALT2,
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

#ifndef CONFIG_SPI3_PINSET
#define CONFIG_SPI3_PINSET SPI_Pinset_DEFAULT
#endif //CONFIG_SPI3_PINSET

#ifndef CONFIG_SPI4_PINSET
#define CONFIG_SPI4_PINSET SPI_Pinset_DEFAULT
#endif //CONFIG_SPI4_PINSET

#ifndef CONFIG_SPI5_PINSET
#define CONFIG_SPI5_PINSET SPI_Pinset_DEFAULT
#endif //CONFIG_SPI5_PINSET

const SPI_Pinset_t SPI_Index_Map_Alt[hwSPI_Index_MAX] = {
    CONFIG_SPI0_PINSET,
    CONFIG_SPI1_PINSET,
    CONFIG_SPI2_PINSET,
    CONFIG_SPI3_PINSET,
    CONFIG_SPI4_PINSET,
    CONFIG_SPI5_PINSET
};

const SPI_Pin_Def SPI_Pin_Def_Table[hwSPI_Index_MAX][SPI_Pinset_MAX] =
{
    /* ================= SPI1 (SPI0) ================= */
    {
        { hwGPIO_Pin_A7,  hwGPIO_Pin_A6,  hwGPIO_Pin_A5,  hwGPIO_Pin_A4 },   // DEFAULT
        { hwGPIO_Pin_B5,  hwGPIO_Pin_B4,  hwGPIO_Pin_B3,  hwGPIO_Pin_A15 },  // ALT1
        { hwGPIO_Pin_D7,  hwGPIO_Pin_G9,  hwGPIO_Pin_G11, hwGPIO_Pin_G10 },  // ALT2
    },

    /* ================= SPI2 (SPI1) ================= */
    {
        { hwGPIO_Pin_B15, hwGPIO_Pin_B14, hwGPIO_Pin_B13, hwGPIO_Pin_B12 },
        { hwGPIO_Pin_C1,  hwGPIO_Pin_C2,  hwGPIO_Pin_B10, hwGPIO_Pin_B9  },
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC  },
    },

    /* ================= SPI3 (SPI2) ================= */
    {
        { hwGPIO_Pin_C12, hwGPIO_Pin_C11, hwGPIO_Pin_C10, hwGPIO_Pin_A15 },
        { hwGPIO_Pin_B2,  hwGPIO_Pin_B4,  hwGPIO_Pin_B3,  hwGPIO_Pin_A4  },
        { hwGPIO_Pin_D6,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC  },
    },

    /* ================= SPI4 (SPI3) ================= */
    {
        { hwGPIO_Pin_E6,  hwGPIO_Pin_E5,  hwGPIO_Pin_E2,  hwGPIO_Pin_E4  },
        { hwGPIO_Pin_E14, hwGPIO_Pin_E13, hwGPIO_Pin_E12, hwGPIO_Pin_E11 },
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC  },
    },

    /* ================= SPI5 (SPI4) ================= */
    {
        { hwGPIO_Pin_F9,  hwGPIO_Pin_F8,  hwGPIO_Pin_F7,  hwGPIO_Pin_F6  },
        { hwGPIO_Pin_F11, hwGPIO_Pin_H7,  hwGPIO_Pin_H6,  hwGPIO_Pin_H5  },
        { hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC,  hwGPIO_Pin_NC  },
    },

    /* ================= SPI6 (SPI5) ================= */
    {
        { hwGPIO_Pin_G14, hwGPIO_Pin_G12, hwGPIO_Pin_G13, hwGPIO_Pin_G8  },  // DEFAULT
        { hwGPIO_Pin_A7,  hwGPIO_Pin_A6,  hwGPIO_Pin_A5,  hwGPIO_Pin_A4  },  // FMC
        { hwGPIO_Pin_B5,  hwGPIO_Pin_B4,  hwGPIO_Pin_B3,  hwGPIO_Pin_A15 },  // ETH
    },
};

const SPI_AF_Map SPI_Pin_AF_Map[] =
{
    { hwSPI_Index_0, hwGPIO_Pin_A7,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_B5,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_D7,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_A6,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_B4,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_G9,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_A5,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_B3,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_G11, GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_A4,  GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_A15, GPIO_AF5_SPI1 },
    { hwSPI_Index_0, hwGPIO_Pin_G10, GPIO_AF5_SPI1 },

    { hwSPI_Index_1, hwGPIO_Pin_B15, GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_C1,  GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_B14, GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_C2,  GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_B13, GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_A9,  GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_A12, GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_B10, GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_D3,  GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_B12, GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_B9,  GPIO_AF5_SPI2 },
    { hwSPI_Index_1, hwGPIO_Pin_B4,  GPIO_AF7_SPI2 },

    { hwSPI_Index_2, hwGPIO_Pin_B2,  GPIO_AF7_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_B5,  GPIO_AF7_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_D6,  GPIO_AF5_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_C12, GPIO_AF6_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_C11, GPIO_AF6_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_B4,  GPIO_AF6_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_B3,  GPIO_AF6_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_C10, GPIO_AF6_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_A4,  GPIO_AF6_SPI3 },
    { hwSPI_Index_2, hwGPIO_Pin_A15, GPIO_AF6_SPI3 },

    { hwSPI_Index_3, hwGPIO_Pin_E6,  GPIO_AF5_SPI4 },
    { hwSPI_Index_3, hwGPIO_Pin_E14, GPIO_AF5_SPI4 },
    { hwSPI_Index_3, hwGPIO_Pin_E5,  GPIO_AF5_SPI4 },
    { hwSPI_Index_3, hwGPIO_Pin_E13, GPIO_AF5_SPI4 },
    { hwSPI_Index_3, hwGPIO_Pin_E2,  GPIO_AF5_SPI4 },
    { hwSPI_Index_3, hwGPIO_Pin_E12, GPIO_AF5_SPI4 },
    { hwSPI_Index_3, hwGPIO_Pin_E4,  GPIO_AF5_SPI4 },
    { hwSPI_Index_3, hwGPIO_Pin_E11, GPIO_AF5_SPI4 },

    { hwSPI_Index_4, hwGPIO_Pin_F9,  GPIO_AF5_SPI5 },
    { hwSPI_Index_4, hwGPIO_Pin_F11, GPIO_AF5_SPI5 },
    { hwSPI_Index_4, hwGPIO_Pin_F8,  GPIO_AF5_SPI5 },
    { hwSPI_Index_4, hwGPIO_Pin_F7,  GPIO_AF5_SPI5 },
    { hwSPI_Index_4, hwGPIO_Pin_F6,  GPIO_AF5_SPI5 },

    { hwSPI_Index_5, hwGPIO_Pin_A7,  GPIO_AF8_SPI6 },
    { hwSPI_Index_5, hwGPIO_Pin_B5,  GPIO_AF8_SPI6 },
    { hwSPI_Index_5, hwGPIO_Pin_G14, GPIO_AF5_SPI6 },
    { hwSPI_Index_5, hwGPIO_Pin_A6,  GPIO_AF8_SPI6 },
    { hwSPI_Index_5, hwGPIO_Pin_B4,  GPIO_AF8_SPI6 },
    { hwSPI_Index_5, hwGPIO_Pin_G12, GPIO_AF5_SPI6 },
    { hwSPI_Index_5, hwGPIO_Pin_A5,  GPIO_AF8_SPI6 },
    { hwSPI_Index_5, hwGPIO_Pin_B3,  GPIO_AF8_SPI6 },
    { hwSPI_Index_5, hwGPIO_Pin_C12, GPIO_AF5_SPI6 },
    { hwSPI_Index_5, hwGPIO_Pin_G13, GPIO_AF5_SPI6 },
    { hwSPI_Index_5, hwGPIO_Pin_A4,  GPIO_AF8_SPI6 },
    { hwSPI_Index_5, hwGPIO_Pin_A15, GPIO_AF7_SPI6 },
    { hwSPI_Index_5, hwGPIO_Pin_A0,  GPIO_AF5_SPI6 },
    { hwSPI_Index_5, hwGPIO_Pin_G8,  GPIO_AF5_SPI6 },
};

#endif //SPI_PIN_STM32H723xx_H