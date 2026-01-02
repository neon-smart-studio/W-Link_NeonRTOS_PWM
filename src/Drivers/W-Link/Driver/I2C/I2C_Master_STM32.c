#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "soc.h"

#include "NeonRTOS.h"

#include "I2C_Master.h"

#ifdef STM32

#include "I2C_Pin.h"

#include "GPIO/GPIO_STM32.h"

#define I2C_IRQ_NVIC_PRIORITY 5
#define I2C_IRQ_NVIC_SUB_PRIORITY 0

#ifdef STM32_I2C_LEGACY_ARCH
#define I2C_MASTER_STANDARD_MODE_CLK_FREQUENCY      100000
#define I2C_MASTER_FAST_MODE_CLK_FREQUENCY          400000
#endif // STM32_I2C_LEGACY_ARCH

#ifdef STM32F0
#define TIMING_VAL_48M_CLK_100KHZ  0x10805E89  // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_48M_CLK_400KHZ  0x00901850  // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_48M_CLK_1MHZ    0x00700818  // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_48M               48000000    // 48 MHz    
#endif //STM32F0

#ifdef STM32F7
#define TIMING_VAL_54M_CLK_100KHZ  0x10916998  // Standard mode with Rise time = 120ns, Fall time = 120ns
#define TIMING_VAL_54M_CLK_400KHZ  0x00B11B54  // Fast Mode with Rise time = 120ns, Fall time = 120ns
#define TIMING_VAL_54M_CLK_1MHZ    0x0090091B  // Fast Mode Plus with Rise time = 120ns, Fall time = 10ns
#define I2C_PCLK_54M               54000000    // 54 MHz
#endif //STM32F7

#ifdef STM32F3
#define TIMING_VAL_64M_CLK_100KHZ  0x10B17DB4  // Standard mode with Rise time = 120ns, Fall time = 120ns
#define TIMING_VAL_64M_CLK_400KHZ  0x00E22163  // Fast Mode with Rise time = 120ns, Fall time = 120ns
#define TIMING_VAL_64M_CLK_1MHZ    0x00A00D1E  // Fast Mode Plus with Rise time = 120ns, Fall time = 10ns
#define I2C_PCLK_64M               64000000    // 64 MHz

#define TIMING_VAL_72M_CLK_100KHZ  0x10D28DCB  // Standard mode with Rise time = 120ns, Fall time = 120ns
#define TIMING_VAL_72M_CLK_400KHZ  0x00F32571  // Fast Mode with Rise time = 120ns, Fall time = 120ns
#define TIMING_VAL_72M_CLK_1MHZ    0x00C00D24  // Fast Mode Plus with Rise time = 120ns, Fall time = 10ns
#define I2C_PCLK_72M               72000000    // 72 MHz
#endif //STM32F3

#ifdef STM32G0
#define TIMING_VAL_64M_CLK_100KHZ  0xC0311319  // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_64M_CLK_400KHZ  0x10B1102E  // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_64M_CLK_1MHZ    0x00710B1E  // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_64M               64000000    // 64 MHz
#endif //STM32G0

#ifdef STM32G4
#define TIMING_VAL_160M_CLK_100KHZ  0x90A13E56  // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_160M_CLK_400KHZ  0x30D2153A  // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_160M_CLK_1MHZ    0x10830F28  // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_160M               160000000   // 160 MHz
#endif //STM32G4

#ifdef STM32H7
#define TIMING_VAL_140M_CLK_100KHZ 0x70B1445E   // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_140M_CLK_400KHZ 0x30C11233   // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_140M_CLK_1MHZ   0x00F41B47   // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_140M              140000000    // 140 MHz

#define TIMING_VAL_137M5_CLK_100KHZ 0x70B1445B   // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_137M5_CLK_400KHZ 0x80510815   // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_137M5_CLK_1MHZ   0x00F41A46   // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_137M5              137500000    // 137.5 MHz

#define TIMING_VAL_120M_CLK_100KHZ 0x40E15676   // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_120M_CLK_400KHZ 0x20C11434   // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_120M_CLK_1MHZ   0x00C31536   // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_120M              120000000    // 120 MHz

#define TIMING_VAL_112M5_CLK_100KHZ 0x80813142   // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_112M5_CLK_400KHZ 0x20D11337   // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_112M5_CLK_1MHZ   0x00C31538   // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_112M5              112500000    // 112.5 MHz

#define TIMING_VAL_100M_CLK_100KHZ 0x50A14159   // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_100M_CLK_400KHZ 0x20B11130   // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_100M_CLK_1MHZ   0x00A21232   // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_100M              100000000    // 100 MHz
#endif //STM32H7

#ifdef STM32L0
#define TIMING_VAL_32M_CLK_100KHZ  0x20602938  // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_32M_CLK_400KHZ  0x00B0122A  // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_32M_CLK_1MHZ    0x0030040E  // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_32M               32000000    // 32 MHz
#endif //STM32L0

#ifdef STM32L4
#define TIMING_VAL_80M_CLK_100KHZ 0x30C14E6B   // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_80M_CLK_400KHZ 0x10D1143A   // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_80M_CLK_1MHZ   0x00810E27   // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_80M              80000000     // 80 MHz

#define TIMING_VAL_48M_CLK_100KHZ 0x20A03E55   // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_48M_CLK_400KHZ 0x10800C21   // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_48M_CLK_1MHZ   0x00500816   // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_48M              48000000     // 48 MHz

#define TIMING_VAL_120M_CLK_100KHZ 0x107075B0  // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_120M_CLK_400KHZ 0x00501E6C  // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_120M_CLK_1MHZ   0x00200A26  // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_120M              120000000   // 120 MHz
#endif //STM32L4

#ifdef STM32L5
#define TIMING_VAL_110M_CLK_100KHZ 0x40E15676   // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_110M_CLK_400KHZ 0x20C11434   // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_110M_CLK_1MHZ   0x00C31536   // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_110M              110000000    // 110 MHz
#endif //STM32L5

#ifdef STM32U5
#define TIMING_VAL_160M_CLK_100KHZ 0x90A13E56   // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_160M_CLK_400KHZ 0x30D2153A   // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_160M_CLK_1MHZ   0x10830F28   // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_160M              160000000    // 160 MHz
#endif //STM32U5

#ifdef STM32WB
#define TIMING_VAL_64M_CLK_100KHZ 0x10707DBC   // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_64M_CLK_400KHZ 0x00602173   // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_64M_CLK_1MHZ   0x00300B29   // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_64M              64000000     // 64 MHz

#define TIMING_VAL_32M_CLK_100KHZ 0x00707CBB   // Standard mode with Rise Time = 400ns and Fall Time = 100ns
#define TIMING_VAL_32M_CLK_400KHZ 0x00300F38   // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_32M_CLK_1MHZ   0x00100413   // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_32M              32000000     // 32 MHz
#endif //STM32WB

#ifdef STM32WL
#define TIMING_VAL_48M_CLK_100KHZ  0x20E03F53  // Standard mode with Rise Time = 640ns and Fall Time = 20ns
#define TIMING_VAL_48M_CLK_400KHZ  0x20500817  // Fast mode with Rise Time = 250ns and Fall Time = 100ns
#define TIMING_VAL_48M_CLK_1MHZ    0x00500A18  // Fast mode Plus with Rise Time = 60ns and Fall Time = 100ns
#define I2C_PCLK_48M               48000000    // 48 MHz
#endif //STM32WL

static bool I2C_Master_Init_Status[hwI2C_Index_MAX] = {false};
static hwI2C_Speed_Mode I2C_Clock_Speed_Mode[hwI2C_Index_MAX] = {hwI2C_Standard_Mode};
static NeonRTOS_SyncObj_t I2C_Master_Done_SyncHandle[hwI2C_Index_MAX];

static I2C_HandleTypeDef g_i2c[hwI2C_Index_MAX];

#ifdef STM32_I2C_NEW_ARCH
uint32_t I2C_Get_PCLK(hwI2C_Index index)
{
    uint32_t clocksource;
    uint32_t pclk = 0;

#ifdef STM32H7
    PLL3_ClocksTypeDef pll3_clocks;
    if ((index == hwI2C_Index_0) || (index == hwI2C_Index_1) || (index == hwI2C_Index_2)) {
        clocksource = __HAL_RCC_GET_I2C123_SOURCE();
        switch (clocksource) {
            case RCC_I2C123CLKSOURCE_D2PCLK1:
                pclk = HAL_RCC_GetPCLK1Freq();
                break;
            case RCC_I2C123CLKSOURCE_PLL3:
                HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
                pclk = pll3_clocks.PLL3_R_Frequency;
                break;
            case RCC_I2C123CLKSOURCE_HSI:
                pclk = HSI_VALUE;
                break;
            case RCC_I2C123CLKSOURCE_CSI:
                pclk = CSI_VALUE;
                break;
            default:
                // should not happend
                error("I2C123: Invalid clock source");
                break;
        }
    }
#if defined I2C4_BASE
    else if (index == hwI2C_Index_3) {
        clocksource = __HAL_RCC_GET_I2C4_SOURCE();
        switch (clocksource) {
            case RCC_I2C4CLKSOURCE_D3PCLK1:
                pclk = HAL_RCCEx_GetD3PCLK1Freq();
                break;
            case RCC_I2C4CLKSOURCE_PLL3:
                HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
                pclk = pll3_clocks.PLL3_R_Frequency;
                break;
            case RCC_I2C4CLKSOURCE_HSI:
                pclk = HSI_VALUE;
                break;
            case RCC_I2C4CLKSOURCE_CSI:
                pclk = CSI_VALUE;
                break;
            default:
                // should not happend
                error("I2C4: Invalid clock source");
                break;
        }
    }
#endif
#if defined I2C5_BASE
    else if (index == hwI2C_Index_4) {
        clocksource = __HAL_RCC_GET_I2C5_SOURCE();
        switch (clocksource) {
            case RCC_I2C5CLKSOURCE_D2PCLK1:
                pclk = HAL_RCC_GetPCLK1Freq();
                break;
            case RCC_I2C5CLKSOURCE_PLL3:
                HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
                pclk = pll3_clocks.PLL3_R_Frequency;
                break;
            case RCC_I2C5CLKSOURCE_HSI:
                pclk = HSI_VALUE;
                break;
            case RCC_I2C5CLKSOURCE_CSI:
                pclk = CSI_VALUE;
                break;
            default:
                // should not happend
                error("I2C5: Invalid clock source");
                break;
        }
    }
#endif
#else //STM32H7
    if (index == hwI2C_Index_0) {
        clocksource = __HAL_RCC_GET_I2C1_SOURCE();
        switch (clocksource) {
            case RCC_I2C1CLKSOURCE_PCLK1:
                pclk = HAL_RCC_GetPCLK1Freq();
                break;

            case RCC_I2C1CLKSOURCE_SYSCLK:
                pclk = HAL_RCC_GetSysClockFreq();
                break;

            case RCC_I2C1CLKSOURCE_HSI:
                pclk = HSI_VALUE;
                break;
        }
    }
#if defined I2C2_BASE
    else if (index == hwI2C_Index_1) {
        clocksource = __HAL_RCC_GET_I2C2_SOURCE();
        switch (clocksource) {
            case RCC_I2C2CLKSOURCE_PCLK1:
                pclk = HAL_RCC_GetPCLK1Freq();
                break;

            case RCC_I2C2CLKSOURCE_SYSCLK:
                pclk = HAL_RCC_GetSysClockFreq();
                break;

            case RCC_I2C2CLKSOURCE_HSI:
                pclk = HSI_VALUE;
                break;
        }
    }
#endif
#if defined I2C3_BASE
    else if (index == hwI2C_Index_2) {
        clocksource = __HAL_RCC_GET_I2C3_SOURCE();
        switch (clocksource) {
            case RCC_I2C3CLKSOURCE_PCLK1:
                pclk = HAL_RCC_GetPCLK1Freq();
                break;

            case RCC_I2C3CLKSOURCE_SYSCLK:
                pclk = HAL_RCC_GetSysClockFreq();
                break;

            case RCC_I2C3CLKSOURCE_HSI:
                pclk = HSI_VALUE;
                break;
        }
    }
#endif
#if defined I2C4_BASE
    else if (index == hwI2C_Index_3) {
        clocksource = __HAL_RCC_GET_I2C4_SOURCE();
        switch (clocksource) {
            case RCC_I2C4CLKSOURCE_PCLK1:
                pclk = HAL_RCC_GetPCLK1Freq();
                break;

            case RCC_I2C4CLKSOURCE_SYSCLK:
                pclk = HAL_RCC_GetSysClockFreq();
                break;

            case RCC_I2C4CLKSOURCE_HSI:
                pclk = HSI_VALUE;
                break;
        }
    }
#endif
#endif //STM32H7
    else {
        // should not happend
        UART_Printf("I2C: unknown instance");
    }
    return pclk;
}

uint32_t I2C_Master_Get_Timing(hwI2C_Index index, hwI2C_Speed_Mode speed_mode)
{
    uint32_t tim = 0;

    uint32_t pclk = I2C_Get_PCLK(index);

    switch (pclk) {
#if defined (I2C_PCLK_32M)
        case I2C_PCLK_32M:
            switch (speed_mode) {
                case hwI2C_Standard_Mode:
                    tim = TIMING_VAL_32M_CLK_100KHZ;
                    break;
                case hwI2C_Fast_Mode:
                    tim = TIMING_VAL_32M_CLK_400KHZ;
                    break;
                case hwI2C_High_Speed_Mode:
                    tim = TIMING_VAL_32M_CLK_1MHZ;
                    break;
            }
            break;
#endif
#if defined (I2C_PCLK_48M)
        case I2C_PCLK_48M:
            switch (speed_mode) {
                case hwI2C_Standard_Mode:
                    tim = TIMING_VAL_48M_CLK_100KHZ;
                    break;
                case hwI2C_Fast_Mode:
                    tim = TIMING_VAL_48M_CLK_400KHZ;
                    break;
                case hwI2C_High_Speed_Mode:
                    tim = TIMING_VAL_48M_CLK_1MHZ;
                    break;
            }
            break;
#endif
#if defined (I2C_PCLK_54M)
        case I2C_PCLK_54M:
            switch (speed_mode) {
                case hwI2C_Standard_Mode:
                    tim = TIMING_VAL_54M_CLK_100KHZ;
                    break;
                case hwI2C_Fast_Mode:
                    tim = TIMING_VAL_54M_CLK_400KHZ;
                    break;
                case hwI2C_High_Speed_Mode:
                    tim = TIMING_VAL_54M_CLK_1MHZ;
                    break;
            }
            break;
#endif
#if defined(I2C_PCLK_64M)
        case I2C_PCLK_64M:
            switch (speed_mode) {
                case hwI2C_Standard_Mode:
                    tim = TIMING_VAL_64M_CLK_100KHZ;
                    break;
                case hwI2C_Fast_Mode:
                    tim = TIMING_VAL_64M_CLK_400KHZ;
                    break;
                case hwI2C_High_Speed_Mode:
                    tim = TIMING_VAL_64M_CLK_1MHZ;
                    break;
            }
            break;
#endif
#if defined (I2C_PCLK_72M)
        case I2C_PCLK_72M:
            switch (speed_mode) {
                case hwI2C_Standard_Mode:
                    tim = TIMING_VAL_72M_CLK_100KHZ;
                    break;
                case hwI2C_Fast_Mode:
                    tim = TIMING_VAL_72M_CLK_400KHZ;
                    break;
                case hwI2C_High_Speed_Mode:
                    tim = TIMING_VAL_72M_CLK_1MHZ;
                    break;
            }
            break;
#endif
#if defined (I2C_PCLK_80M)
        case I2C_PCLK_80M:
            switch (speed_mode) {
                case hwI2C_Standard_Mode:
                    tim = TIMING_VAL_80M_CLK_100KHZ;
                    break;
                case hwI2C_Fast_Mode:
                    tim = TIMING_VAL_80M_CLK_400KHZ;
                    break;
                case hwI2C_High_Speed_Mode:
                    tim = TIMING_VAL_80M_CLK_1MHZ;
                    break;
            }
            break;
#endif
#if defined (I2C_PCLK_100M)
        case I2C_PCLK_100M:
            switch (speed_mode) {
                case hwI2C_Standard_Mode:
                    tim = TIMING_VAL_100M_CLK_100KHZ;
                    break;
                case hwI2C_Fast_Mode:
                    tim = TIMING_VAL_100M_CLK_400KHZ;
                    break;
                case hwI2C_High_Speed_Mode:
                    tim = TIMING_VAL_100M_CLK_1MHZ;
                    break;
            }
            break;
#endif
#if defined (I2C_PCLK_110M)
        case I2C_PCLK_110M:
            switch (speed_mode) {
                case hwI2C_Standard_Mode:
                    tim = TIMING_VAL_110M_CLK_100KHZ;
                    break;
                case hwI2C_Fast_Mode:
                    tim = TIMING_VAL_110M_CLK_400KHZ;
                    break;
                case hwI2C_High_Speed_Mode:
                    tim = TIMING_VAL_110M_CLK_1MHZ;
                    break;
            }
            break;
#endif
#if defined (I2C_PCLK_112M5)
        case I2C_PCLK_112M5:
            switch (speed_mode) {
                case hwI2C_Standard_Mode:
                    tim = TIMING_VAL_112M5_CLK_100KHZ;
                    break;
                case hwI2C_Fast_Mode:
                    tim = TIMING_VAL_112M5_CLK_400KHZ;
                    break;
                case hwI2C_High_Speed_Mode:
                    tim = TIMING_VAL_112M5_CLK_1MHZ;
                    break;
            }
            break;
#endif
#if defined (I2C_PCLK_120M)
        case I2C_PCLK_120M:
            switch (speed_mode) {
                case hwI2C_Standard_Mode:
                    tim = TIMING_VAL_120M_CLK_100KHZ;
                    break;
                case hwI2C_Fast_Mode:
                    tim = TIMING_VAL_120M_CLK_400KHZ;
                    break;
                case hwI2C_High_Speed_Mode:
                    tim = TIMING_VAL_120M_CLK_1MHZ;
                    break;
            }
            break;
#endif
#if defined (I2C_PCLK_137M5)
        case I2C_PCLK_137M5:
            switch (speed_mode) {
                case hwI2C_Standard_Mode:
                    tim = TIMING_VAL_137M5_CLK_100KHZ;
                    break;
                case hwI2C_Fast_Mode:
                    tim = TIMING_VAL_137M5_CLK_400KHZ;
                    break;
                case hwI2C_High_Speed_Mode:
                    tim = TIMING_VAL_137M5_CLK_1MHZ;
                    break;
            }
            break;
#endif
#if defined (I2C_PCLK_140M)
        case I2C_PCLK_140M:
            switch (speed_mode) {
                case hwI2C_Standard_Mode:
                    tim = TIMING_VAL_140M_CLK_100KHZ;
                    break;
                case hwI2C_Fast_Mode:
                    tim = TIMING_VAL_140M_CLK_400KHZ;
                    break;
                case hwI2C_High_Speed_Mode:
                    tim = TIMING_VAL_140M_CLK_1MHZ;
                    break;
            }
            break;
#endif
#if defined (I2C_PCLK_160M)
        case I2C_PCLK_160M:
            switch (speed_mode) {
                case hwI2C_Standard_Mode:
                    tim = TIMING_VAL_160M_CLK_100KHZ;
                    break;
                case hwI2C_Fast_Mode:
                    tim = TIMING_VAL_160M_CLK_400KHZ;
                    break;
                case hwI2C_High_Speed_Mode:
                    tim = TIMING_VAL_160M_CLK_1MHZ;
                    break;
            }
            break;
#endif
    }
    
    return tim;
}
#endif //STM32_I2C_NEW_ARCH

uint32_t STM32_I2C_GetAF(hwI2C_Index I2C, hwGPIO_Pin pin)
{
    for (size_t i = 0; i < sizeof(I2C_Pin_AF_Map)/sizeof(I2C_Pin_AF_Map[0]); i++) {
        if (I2C_Pin_AF_Map[i].i2c == I2C &&
            I2C_Pin_AF_Map[i].pin  == pin)
            return I2C_Pin_AF_Map[i].af;
    }
    return 0;
}

static hwI2C_Index I2C_IndexFromHandle(I2C_HandleTypeDef *hI2C)
{
    for(int i=0;i<hwI2C_Index_MAX;i++){
        if(&g_i2c[i] == hI2C) return (hwI2C_Index)i;
    }
    return hwI2C_Index_MAX;
}

I2C_TypeDef * I2C_Map_Soc_Base(hwI2C_Index index)
{
    switch(index)
    {
        case hwI2C_Index_0: return I2C1;
        case hwI2C_Index_1: return I2C2;
        case hwI2C_Index_2: return I2C3;
#if defined (I2C4_BASE)
        case hwI2C_Index_3: return I2C4;
#endif
#if defined (I2C5_BASE)
        case hwI2C_Index_4: return I2C5;
#endif
        default: break;
    }
    return NULL;
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c_cb)
{
    for (int i = 0; i < hwI2C_Index_MAX; i++)
    {
        if (&g_i2c[i] == hi2c_cb)
        {
            NeonRTOS_SyncObjSignalFromISR(&I2C_Master_Done_SyncHandle[i]);
            break;
        }
    }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c_cb)
{
    for (int i = 0; i < hwI2C_Index_MAX; i++)
    {
        if (&g_i2c[i] == hi2c_cb)
        {
            NeonRTOS_SyncObjSignalFromISR(&I2C_Master_Done_SyncHandle[i]);
            break;
        }
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c_cb)
{
    for (int i = 0; i < hwI2C_Index_MAX; i++)
    {
        if (&g_i2c[i] == hi2c_cb)
        {
            NeonRTOS_SyncObjSignalFromISR(&I2C_Master_Done_SyncHandle[i]);
            break;
        }
    }
}

// UART IRQ 統一入口
static void I2C_HAL_EV_IRQHandler(hwI2C_Index index)
{
    HAL_I2C_EV_IRQHandler(&g_i2c[index]);
}

// UART IRQ 統一入口
static void I2C_HAL_ER_IRQHandler(hwI2C_Index index)
{
    HAL_I2C_ER_IRQHandler(&g_i2c[index]);
}

void I2C1_EV_IRQHandler(void){ I2C_HAL_EV_IRQHandler(hwI2C_Index_0); }
void I2C1_ER_IRQHandler(void){ I2C_HAL_ER_IRQHandler(hwI2C_Index_0); }
void I2C2_EV_IRQHandler(void){ I2C_HAL_EV_IRQHandler(hwI2C_Index_1); }
void I2C2_ER_IRQHandler(void){ I2C_HAL_ER_IRQHandler(hwI2C_Index_1); }
void I2C3_EV_IRQHandler(void){ I2C_HAL_EV_IRQHandler(hwI2C_Index_2); }
void I2C3_ER_IRQHandler(void){ I2C_HAL_ER_IRQHandler(hwI2C_Index_2); }
#if defined (I2C4_BASE)
void I2C4_EV_IRQHandler(void){ I2C_HAL_EV_IRQHandler(hwI2C_Index_3); }
void I2C4_ER_IRQHandler(void){ I2C_HAL_ER_IRQHandler(hwI2C_Index_3); }
#endif
#if defined (I2C5_BASE)
void I2C5_EV_IRQHandler(void){ I2C_HAL_EV_IRQHandler(hwI2C_Index_4); }
void I2C5_ER_IRQHandler(void){ I2C_HAL_ER_IRQHandler(hwI2C_Index_4); }
#endif

hwI2C_OpResult I2C_Master_Init(hwI2C_Index index, hwI2C_Speed_Mode speed_mode)
{
    if(index>=hwI2C_Index_MAX)
    {
        return hwI2C_InvalidParameter;
    }

    if(I2C_Master_Init_Status[index]==true)
    {
        return hwI2C_OK;
    }

    if(speed_mode>=hwI2C_Speed_Mode_MAX)
    {
        return hwI2C_InvalidParameter;
    }
    
#ifdef STM32_I2C_LEGACY_ARCH
    if(speed_mode==hwI2C_High_Speed_Mode)
    {
        return hwI2C_Unsupport;
    }
#endif // STM32_I2C_LEGACY_ARCH

    GPIO_TypeDef * sda_soc_base = GPIO_Map_Soc_Base(I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].sda_pin);
    GPIO_TypeDef * scl_soc_base = GPIO_Map_Soc_Base(I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].scl_pin);
    uint16_t sda_soc_pin = GPIO_Map_Soc_Pin(I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].sda_pin);
    uint16_t scl_soc_pin = GPIO_Map_Soc_Pin(I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].scl_pin);

    if(sda_soc_pin==0 || sda_soc_base==NULL || scl_soc_pin==0 || scl_soc_base==NULL)
    {
            return hwI2C_InvalidParameter;
    }

    uint32_t sda_af = STM32_UART_GetAF(index, I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].sda_pin);
    uint32_t scl_af = STM32_UART_GetAF(index, I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].scl_pin);

    if(sda_af==0 || scl_af==0)
    {
            return hwI2C_InvalidParameter;
    }

    I2C_TypeDef * i2c_soc_base = I2C_Map_Soc_Base(index);
    if(i2c_soc_base==NULL)
    {
            return hwI2C_InvalidParameter;
    }

    if(NeonRTOS_SyncObjCreate(&I2C_Master_Done_SyncHandle[index])!=NeonRTOS_OK)
    {
            return hwI2C_MemoryError;
    }

    GPIO_Enable_RCC_Clock(sda_soc_base);
    GPIO_Enable_RCC_Clock(scl_soc_base);

    GPIO_InitTypeDef g_i2c_sda = {0};
    g_i2c_sda.Pin       = sda_soc_pin;
    g_i2c_sda.Mode      = GPIO_MODE_AF_PP;
    g_i2c_sda.Pull      = GPIO_NOPULL;
    g_i2c_sda.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    g_i2c_sda.Alternate = sda_af;
    HAL_GPIO_Init(sda_soc_base, &g_i2c_sda);

    GPIO_InitTypeDef g_i2c_scl = {0};
    g_i2c_scl.Pin       = scl_soc_pin;
    g_i2c_scl.Mode      = GPIO_MODE_AF_PP;
    g_i2c_scl.Pull      = GPIO_NOPULL;
    g_i2c_scl.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    g_i2c_scl.Alternate = scl_af;
    HAL_GPIO_Init(scl_soc_base, &g_i2c_scl);

    switch(index)
    {
        case hwI2C_Index_0:
            __HAL_RCC_I2C1_CLK_ENABLE();
            break;
        case hwI2C_Index_1:
            __HAL_RCC_I2C2_CLK_ENABLE();
            break;
        case hwI2C_Index_2:
            __HAL_RCC_I2C3_CLK_ENABLE();
            break;
#if defined (I2C4_BASE)
        case hwI2C_Index_3:
            __HAL_RCC_I2C4_CLK_ENABLE();
            break;
#endif
#if defined (I2C5_BASE)
        case hwI2C_Index_4:
            __HAL_RCC_I2C5_CLK_ENABLE();
            break;
#endif
    }

    g_i2c[index].Instance = i2c_soc_base;
#ifdef STM32_I2C_LEGACY_ARCH
    switch(speed_mode)
    {
        case hwI2C_Standard_Mode:
            g_i2c[index].Init.ClockSpeed = I2C_MASTER_STANDARD_MODE_CLK_FREQUENCY;
            break;
        case hwI2C_Fast_Mode:
            g_i2c[index].Init.ClockSpeed = I2C_MASTER_FAST_MODE_CLK_FREQUENCY;
            break;
    }
#endif
#ifdef STM32_I2C_NEW_ARCH
    g_i2c[index].Init.Timing = I2C_Master_Get_Timing(index, speed_mode);
#endif // STM32_I2C_NEW_ARCH
    g_i2c[index].Init.OwnAddress1 = 0;
    g_i2c[index].Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    g_i2c[index].Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    g_i2c[index].Init.OwnAddress2 = 0;
    g_i2c[index].Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    g_i2c[index].Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if(HAL_I2C_Init(&g_i2c[index])!=HAL_OK)
    {
        NeonRTOS_SyncObjDelete(&I2C_Master_Done_SyncHandle[index]);
        return hwI2C_HwError;
    }

    switch(index)
    {       
        case hwI2C_Index_0:
            HAL_NVIC_SetPriority(I2C1_EV_IRQn, I2C_IRQ_NVIC_PRIORITY, I2C_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
            HAL_NVIC_SetPriority(I2C1_ER_IRQn, I2C_IRQ_NVIC_PRIORITY, I2C_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
            break;
        case hwI2C_Index_1:
            HAL_NVIC_SetPriority(I2C2_EV_IRQn, I2C_IRQ_NVIC_PRIORITY, I2C_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
            HAL_NVIC_SetPriority(I2C2_ER_IRQn, I2C_IRQ_NVIC_PRIORITY, I2C_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
            break;
        case hwI2C_Index_2:
            HAL_NVIC_SetPriority(I2C3_EV_IRQn, I2C_IRQ_NVIC_PRIORITY, I2C_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
            HAL_NVIC_SetPriority(I2C3_ER_IRQn, I2C_IRQ_NVIC_PRIORITY, I2C_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);
            break;
#if defined (I2C4_BASE)
        case hwI2C_Index_3:
            HAL_NVIC_SetPriority(I2C4_EV_IRQn, I2C_IRQ_NVIC_PRIORITY, I2C_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(I2C4_EV_IRQn);
            HAL_NVIC_SetPriority(I2C4_ER_IRQn, I2C_IRQ_NVIC_PRIORITY, I2C_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(I2C4_ER_IRQn);
            break;
#endif
#if defined (I2C5_BASE)
        case hwI2C_Index_4:
            HAL_NVIC_SetPriority(I2C5_EV_IRQn, I2C_IRQ_NVIC_PRIORITY, I2C_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(I2C5_EV_IRQn);
            HAL_NVIC_SetPriority(I2C5_ER_IRQn, I2C_IRQ_NVIC_PRIORITY, I2C_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(I2C5_ER_IRQn);
            break;
#endif
    }

    gpio_pin_init_status[I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].sda_pin] = true;
    gpio_pin_init_status[I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].scl_pin] = true;

    I2C_Master_Init_Status[index] = true;

    return hwI2C_OK;
}

hwI2C_OpResult I2C_Master_DeInit(hwI2C_Index index)
{
    if(index>=hwI2C_Index_MAX)
    {
        return hwI2C_InvalidParameter;
    }

    if(I2C_Master_Init_Status[index]==false)
    {
        return hwI2C_OK;
    }
    
    GPIO_TypeDef * sda_soc_base = GPIO_Map_Soc_Base(I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].sda_pin);
    GPIO_TypeDef * scl_soc_base = GPIO_Map_Soc_Base(I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].scl_pin);
    uint16_t sda_soc_pin = GPIO_Map_Soc_Pin(I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].sda_pin);
    uint16_t scl_soc_pin = GPIO_Map_Soc_Pin(I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].scl_pin);

    if(sda_soc_pin==0 || sda_soc_base==NULL || scl_soc_pin==0 || scl_soc_base==NULL)
    {
            return hwI2C_InvalidParameter;
    }

    switch(index)
    {       
        case hwI2C_Index_0:
            HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
            HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
            break;
        case hwI2C_Index_1:
            HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
            HAL_NVIC_DisableIRQ(I2C2_ER_IRQn);
            break;
        case hwI2C_Index_2:
            HAL_NVIC_DisableIRQ(I2C3_EV_IRQn);
            HAL_NVIC_DisableIRQ(I2C3_ER_IRQn);
            break;
#if defined (I2C4_BASE)
        case hwI2C_Index_3:
            HAL_NVIC_DisableIRQ(I2C4_EV_IRQn);
            HAL_NVIC_DisableIRQ(I2C4_ER_IRQn);
            break;
#endif
#if defined (I2C5_BASE)
        case hwI2C_Index_4:
            HAL_NVIC_DisableIRQ(I2C5_EV_IRQn);
            HAL_NVIC_DisableIRQ(I2C5_ER_IRQn);
            break;
#endif
    }

    HAL_I2C_DeInit(&g_i2c[index]);

    switch(index)
    {
        case hwI2C_Index_0:
            __HAL_RCC_I2C1_CLK_DISABLE();
            break;
        case hwI2C_Index_1:
            __HAL_RCC_I2C2_CLK_DISABLE();
            break;
        case hwI2C_Index_2:
            __HAL_RCC_I2C3_CLK_DISABLE();
            break;
#if defined (I2C4_BASE)
        case hwI2C_Index_3:
            __HAL_RCC_I2C4_CLK_DISABLE();
            break;
#endif
#if defined (I2C5_BASE)
        case hwI2C_Index_4:
            __HAL_RCC_I2C5_CLK_DISABLE();
            break;
#endif
    }

    NeonRTOS_SyncObjDelete(&I2C_Master_Done_SyncHandle[index]);

    HAL_GPIO_DeInit(sda_soc_base, sda_soc_pin);
    HAL_GPIO_DeInit(scl_soc_base, scl_soc_pin);

    gpio_pin_init_status[I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].sda_pin] = false;
    gpio_pin_init_status[I2C_Pin_Def_Table[index][I2C_Index_Map_Alt[index]].scl_pin] = false;

    I2C_Master_Init_Status[index] = false;

    return hwI2C_OK;
}


hwI2C_OpResult I2C_Master_Reset(hwI2C_Index index)
{
    if(index>=hwI2C_Index_MAX)
    {
        return hwI2C_InvalidParameter;
    }
    if(I2C_Master_Init_Status[index]==false){return hwI2C_NotInit;}
    
    hwI2C_OpResult op_status = hwI2C_OK;
    
    op_status = I2C_Master_DeInit(index);
    if(op_status<hwI2C_OK)
    {
            return op_status;
    }
    
    op_status = I2C_Master_Init(index, I2C_Clock_Speed_Mode[index]);
    if(op_status<hwI2C_OK)
    {
            return op_status;
    }
    
    return op_status;
}

hwI2C_OpResult I2C_Master_Read(hwI2C_Index index, uint8_t address, uint8_t* read_dat, uint8_t read_len, bool stop, NeonRTOS_Time_t timeoutMs)
{
    if(index>=hwI2C_Index_MAX)
    {
        return hwI2C_InvalidParameter;
    }

    if(I2C_Master_Init_Status[index]==false)
    {
        return hwI2C_NotInit;
    }
    
    if(read_dat==NULL && read_len>0)
    {
        return hwI2C_InvalidParameter;
    }
    
    if(read_len<=0)
    {
        return hwI2C_InvalidParameter;
    }
    
    if (HAL_I2C_Master_Receive_IT(&g_i2c[index], address << 1, read_dat, read_len) != HAL_OK)
    {
        return hwI2C_BusError;
    }

    if (NeonRTOS_SyncObjWait(&I2C_Master_Done_SyncHandle[index], timeoutMs) != NeonRTOS_OK)
    {
        HAL_I2C_Abort_IT(&g_i2c[index]);
        return hwI2C_SlaveTimeout;
    }

    return hwI2C_OK;
}

hwI2C_OpResult I2C_Master_Write(hwI2C_Index index, uint8_t address, uint8_t* write_dat, uint8_t write_len, bool stop, NeonRTOS_Time_t timeoutMs)
{
    if(index>=hwI2C_Index_MAX)
    {
        return hwI2C_InvalidParameter;
    }
    if(I2C_Master_Init_Status[index]==false){return hwI2C_NotInit;}
    
    if(write_dat==NULL && write_len>0)
    {
        return hwI2C_InvalidParameter;
    }
    
    if (HAL_I2C_Master_Transmit_IT(&g_i2c[index], address << 1, write_dat, write_len) != HAL_OK)
    {
        return hwI2C_BusError;
    }

    if (NeonRTOS_SyncObjWait(&I2C_Master_Done_SyncHandle[index], timeoutMs) != NeonRTOS_OK)
    {
        HAL_I2C_Abort_IT(&g_i2c[index]);
        return hwI2C_SlaveTimeout;
    }

    return hwI2C_OK;
}

bool I2C_Master_isInit(hwI2C_Index index)
{
    if(index>=hwI2C_Index_MAX)
    {
        return false;
    }
    return I2C_Master_Init_Status[index];
}

#endif //STM32