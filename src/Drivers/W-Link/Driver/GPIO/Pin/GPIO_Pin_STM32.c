#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "soc.h"

#ifdef STM32

#include "GPIO_Pin_STM32.h"

GPIO_TypeDef * GPIO_Map_Soc_Base(hwGPIO_Pin pin)
{
    GPIO_TypeDef * socBase = NULL;
    switch(pin)
    {
        case hwGPIO_Pin_A0:
        case hwGPIO_Pin_A1:
        case hwGPIO_Pin_A2:
        case hwGPIO_Pin_A3:
        case hwGPIO_Pin_A4:
        case hwGPIO_Pin_A5:
        case hwGPIO_Pin_A6:
        case hwGPIO_Pin_A7:
        case hwGPIO_Pin_A8:
        case hwGPIO_Pin_A9:
        case hwGPIO_Pin_A10:
        case hwGPIO_Pin_A11:
        case hwGPIO_Pin_A12:
        case hwGPIO_Pin_A13:
        case hwGPIO_Pin_A14:
        case hwGPIO_Pin_A15:
            socBase = GPIOA;
            break;
        case hwGPIO_Pin_B0:
        case hwGPIO_Pin_B1:
        case hwGPIO_Pin_B2:
        case hwGPIO_Pin_B3:
        case hwGPIO_Pin_B4:
        case hwGPIO_Pin_B5:
        case hwGPIO_Pin_B6:
        case hwGPIO_Pin_B7:
        case hwGPIO_Pin_B8:
        case hwGPIO_Pin_B9:
        case hwGPIO_Pin_B10:
        case hwGPIO_Pin_B11:
        case hwGPIO_Pin_B12:
        case hwGPIO_Pin_B13:
        case hwGPIO_Pin_B14:
        case hwGPIO_Pin_B15:
            socBase = GPIOB;
            break;
        case hwGPIO_Pin_C0:
        case hwGPIO_Pin_C1:
        case hwGPIO_Pin_C2:
        case hwGPIO_Pin_C3:
        case hwGPIO_Pin_C4:
        case hwGPIO_Pin_C5:
        case hwGPIO_Pin_C6:
        case hwGPIO_Pin_C7:
        case hwGPIO_Pin_C8:
        case hwGPIO_Pin_C9:
        case hwGPIO_Pin_C10:
        case hwGPIO_Pin_C11:
        case hwGPIO_Pin_C12:
        case hwGPIO_Pin_C13:
        case hwGPIO_Pin_C14:
        case hwGPIO_Pin_C15:
            socBase = GPIOC;
            break;
        case hwGPIO_Pin_D0:
        case hwGPIO_Pin_D1:
        case hwGPIO_Pin_D2:
        case hwGPIO_Pin_D3:
        case hwGPIO_Pin_D4:
        case hwGPIO_Pin_D5:
        case hwGPIO_Pin_D6:
        case hwGPIO_Pin_D7:
        case hwGPIO_Pin_D8:
        case hwGPIO_Pin_D9:
        case hwGPIO_Pin_D10:
        case hwGPIO_Pin_D11:
        case hwGPIO_Pin_D12:
        case hwGPIO_Pin_D13:
        case hwGPIO_Pin_D14:
        case hwGPIO_Pin_D15:
            socBase = GPIOD;
            break;
        case hwGPIO_Pin_E0:
        case hwGPIO_Pin_E1:
        case hwGPIO_Pin_E2:
        case hwGPIO_Pin_E3:
        case hwGPIO_Pin_E4:
        case hwGPIO_Pin_E5:
        case hwGPIO_Pin_E6:
        case hwGPIO_Pin_E7:
        case hwGPIO_Pin_E8:
        case hwGPIO_Pin_E9:
        case hwGPIO_Pin_E10:
        case hwGPIO_Pin_E11:
        case hwGPIO_Pin_E12:
        case hwGPIO_Pin_E13:
        case hwGPIO_Pin_E14:
        case hwGPIO_Pin_E15:
            socBase = GPIOE;
            break;
        case hwGPIO_Pin_F0:
        case hwGPIO_Pin_F1:
        case hwGPIO_Pin_F2:
        case hwGPIO_Pin_F3:
        case hwGPIO_Pin_F4:
        case hwGPIO_Pin_F5:
        case hwGPIO_Pin_F6:
        case hwGPIO_Pin_F7:
        case hwGPIO_Pin_F8:
        case hwGPIO_Pin_F9:
        case hwGPIO_Pin_F10:
        case hwGPIO_Pin_F11:
        case hwGPIO_Pin_F12:
        case hwGPIO_Pin_F13:
        case hwGPIO_Pin_F14:
        case hwGPIO_Pin_F15:
            socBase = GPIOF;
            break;
        case hwGPIO_Pin_G0:
        case hwGPIO_Pin_G1:
        case hwGPIO_Pin_G2:
        case hwGPIO_Pin_G3:
        case hwGPIO_Pin_G4:
        case hwGPIO_Pin_G5:
        case hwGPIO_Pin_G6:
        case hwGPIO_Pin_G7:
        case hwGPIO_Pin_G8:
        case hwGPIO_Pin_G9:
        case hwGPIO_Pin_G10:
        case hwGPIO_Pin_G11:
        case hwGPIO_Pin_G12:
        case hwGPIO_Pin_G13:
        case hwGPIO_Pin_G15:
            socBase = GPIOG;
            break;
        case hwGPIO_Pin_H0:
        case hwGPIO_Pin_H1:
        case hwGPIO_Pin_H2:
        case hwGPIO_Pin_H3:
        case hwGPIO_Pin_H4:
        case hwGPIO_Pin_H5:
        case hwGPIO_Pin_H6:
        case hwGPIO_Pin_H7:
        case hwGPIO_Pin_H8:
        case hwGPIO_Pin_H9:
        case hwGPIO_Pin_H10:
        case hwGPIO_Pin_H11:
        case hwGPIO_Pin_H12:
        case hwGPIO_Pin_H13:
        case hwGPIO_Pin_H14:
        case hwGPIO_Pin_H15:
            socBase = GPIOH;
            break;
        case hwGPIO_Pin_I0:
        case hwGPIO_Pin_I1:
        case hwGPIO_Pin_I2:
        case hwGPIO_Pin_I3:
        case hwGPIO_Pin_I4:
        case hwGPIO_Pin_I5:
        case hwGPIO_Pin_I6:
        case hwGPIO_Pin_I7:
        case hwGPIO_Pin_I8:
        case hwGPIO_Pin_I9:
        case hwGPIO_Pin_I10:
        case hwGPIO_Pin_I11:
        case hwGPIO_Pin_I12:
        case hwGPIO_Pin_I13:
        case hwGPIO_Pin_I14:
        case hwGPIO_Pin_I15:
            socBase = GPIOI;
            break;
    }

    return socBase;
}
uint16_t GPIO_Map_Soc_Pin(hwGPIO_Pin pin)
{
    uint16_t socPin = 0;
    switch(pin)
    {
        case hwGPIO_Pin_A0:
        case hwGPIO_Pin_B0:
        case hwGPIO_Pin_C0:
        case hwGPIO_Pin_D0:
        case hwGPIO_Pin_E0:
        case hwGPIO_Pin_F0:
        case hwGPIO_Pin_G0:
        case hwGPIO_Pin_H0:
        case hwGPIO_Pin_I0:
            socPin = GPIO_PIN_0;
            break;
        case hwGPIO_Pin_A1:
        case hwGPIO_Pin_B1:
        case hwGPIO_Pin_C1:
        case hwGPIO_Pin_D1:
        case hwGPIO_Pin_E1:
        case hwGPIO_Pin_F1:
        case hwGPIO_Pin_G1:
        case hwGPIO_Pin_H1:
        case hwGPIO_Pin_I1:
            socPin = GPIO_PIN_1;
            break;
        case hwGPIO_Pin_A2:
        case hwGPIO_Pin_B2:
        case hwGPIO_Pin_C2:
        case hwGPIO_Pin_D2:
        case hwGPIO_Pin_E2:
        case hwGPIO_Pin_F2:
        case hwGPIO_Pin_G2:
        case hwGPIO_Pin_H2:
        case hwGPIO_Pin_I2:
            socPin = GPIO_PIN_2;
            break;
        case hwGPIO_Pin_A3:
        case hwGPIO_Pin_B3:
        case hwGPIO_Pin_C3:
        case hwGPIO_Pin_D3:
        case hwGPIO_Pin_E3:
        case hwGPIO_Pin_F3:
        case hwGPIO_Pin_G3:
        case hwGPIO_Pin_H3:
        case hwGPIO_Pin_I3:
            socPin = GPIO_PIN_3;
            break;
        case hwGPIO_Pin_A4:
        case hwGPIO_Pin_B4:
        case hwGPIO_Pin_C4:
        case hwGPIO_Pin_D4:
        case hwGPIO_Pin_E4:
        case hwGPIO_Pin_F4:
        case hwGPIO_Pin_G4:
        case hwGPIO_Pin_H4:
        case hwGPIO_Pin_I4:
            socPin = GPIO_PIN_4;
            break;
        case hwGPIO_Pin_A5:
        case hwGPIO_Pin_B5:
        case hwGPIO_Pin_C5:
        case hwGPIO_Pin_D5:
        case hwGPIO_Pin_E5:
        case hwGPIO_Pin_F5:
        case hwGPIO_Pin_G5:
        case hwGPIO_Pin_H5:
        case hwGPIO_Pin_I5:
            socPin = GPIO_PIN_5;
            break;
        case hwGPIO_Pin_A6:
        case hwGPIO_Pin_B6:
        case hwGPIO_Pin_C6:
        case hwGPIO_Pin_D6:
        case hwGPIO_Pin_E6:
        case hwGPIO_Pin_F6:
        case hwGPIO_Pin_G6:
        case hwGPIO_Pin_H6:
        case hwGPIO_Pin_I6:
            socPin = GPIO_PIN_6;
            break;
        case hwGPIO_Pin_A7:
        case hwGPIO_Pin_B7:
        case hwGPIO_Pin_C7:
        case hwGPIO_Pin_D7:
        case hwGPIO_Pin_E7:
        case hwGPIO_Pin_F7:
        case hwGPIO_Pin_G7:
        case hwGPIO_Pin_H7:
        case hwGPIO_Pin_I7:
            socPin = GPIO_PIN_7;
            break;
        case hwGPIO_Pin_A8:
        case hwGPIO_Pin_B8:
        case hwGPIO_Pin_C8:
        case hwGPIO_Pin_D8:
        case hwGPIO_Pin_E8:
        case hwGPIO_Pin_F8:
        case hwGPIO_Pin_G8:
        case hwGPIO_Pin_H8:
        case hwGPIO_Pin_I8:
            socPin = GPIO_PIN_8;
            break;
        case hwGPIO_Pin_A9:
        case hwGPIO_Pin_B9:
        case hwGPIO_Pin_C9:
        case hwGPIO_Pin_D9:
        case hwGPIO_Pin_E9:
        case hwGPIO_Pin_F9:
        case hwGPIO_Pin_G9:
        case hwGPIO_Pin_H9:
        case hwGPIO_Pin_I9:
            socPin = GPIO_PIN_9;
            break;
        case hwGPIO_Pin_A10:
        case hwGPIO_Pin_B10:
        case hwGPIO_Pin_C10:
        case hwGPIO_Pin_D10:
        case hwGPIO_Pin_E10:
        case hwGPIO_Pin_F10:
        case hwGPIO_Pin_G10:
        case hwGPIO_Pin_H10:
        case hwGPIO_Pin_I10:
            socPin = GPIO_PIN_10;
            break;
        case hwGPIO_Pin_A11:
        case hwGPIO_Pin_B11:
        case hwGPIO_Pin_C11:
        case hwGPIO_Pin_D11:
        case hwGPIO_Pin_E11:
        case hwGPIO_Pin_F11:
        case hwGPIO_Pin_G11:
        case hwGPIO_Pin_H11:
        case hwGPIO_Pin_I11:
            socPin = GPIO_PIN_11;
            break;
        case hwGPIO_Pin_A12:
        case hwGPIO_Pin_B12:
        case hwGPIO_Pin_C12:
        case hwGPIO_Pin_D12:
        case hwGPIO_Pin_E12:
        case hwGPIO_Pin_F12:
        case hwGPIO_Pin_G12:
        case hwGPIO_Pin_H12:
        case hwGPIO_Pin_I12:
            socPin = GPIO_PIN_12;
            break;
        case hwGPIO_Pin_A13:
        case hwGPIO_Pin_B13:
        case hwGPIO_Pin_C13:
        case hwGPIO_Pin_D13:
        case hwGPIO_Pin_E13:
        case hwGPIO_Pin_F13:
        case hwGPIO_Pin_G13:
        case hwGPIO_Pin_H13:
        case hwGPIO_Pin_I13:
            socPin = GPIO_PIN_13;
            break;
        case hwGPIO_Pin_A14:
        case hwGPIO_Pin_B14:
        case hwGPIO_Pin_C14:
        case hwGPIO_Pin_D14:
        case hwGPIO_Pin_E14:
        case hwGPIO_Pin_F14:
        case hwGPIO_Pin_G14:
        case hwGPIO_Pin_H14:
        case hwGPIO_Pin_I14:
            socPin = GPIO_PIN_14;
            break;
        case hwGPIO_Pin_A15:
        case hwGPIO_Pin_B15:
        case hwGPIO_Pin_C15:
        case hwGPIO_Pin_D15:
        case hwGPIO_Pin_E15:
        case hwGPIO_Pin_F15:
        case hwGPIO_Pin_G15:
        case hwGPIO_Pin_H15:
        case hwGPIO_Pin_I15:
            socPin = GPIO_PIN_15;
            break;
    }

    return socPin;
}

GPIO_TypeDef * GPIO_Int_Map_Soc_Base(hwGPIO_Int_Pin pin)
{
    GPIO_TypeDef * socBase = NULL;
    switch(pin)
    {
        case hwGPIO_Int_Pin_A0:
        case hwGPIO_Int_Pin_A1:
        case hwGPIO_Int_Pin_A2:
        case hwGPIO_Int_Pin_A3:
        case hwGPIO_Int_Pin_A4:
        case hwGPIO_Int_Pin_A5:
        case hwGPIO_Int_Pin_A6:
        case hwGPIO_Int_Pin_A7:
        case hwGPIO_Int_Pin_A8:
        case hwGPIO_Int_Pin_A9:
        case hwGPIO_Int_Pin_A10:
        case hwGPIO_Int_Pin_A11:
        case hwGPIO_Int_Pin_A12:
        case hwGPIO_Int_Pin_A13:
        case hwGPIO_Int_Pin_A14:
        case hwGPIO_Int_Pin_A15:
            socBase = GPIOA;
            break;
        case hwGPIO_Int_Pin_B0:
        case hwGPIO_Int_Pin_B1:
        case hwGPIO_Int_Pin_B2:
        case hwGPIO_Int_Pin_B3:
        case hwGPIO_Int_Pin_B4:
        case hwGPIO_Int_Pin_B5:
        case hwGPIO_Int_Pin_B6:
        case hwGPIO_Int_Pin_B7:
        case hwGPIO_Int_Pin_B8:
        case hwGPIO_Int_Pin_B9:
        case hwGPIO_Int_Pin_B10:
        case hwGPIO_Int_Pin_B11:
        case hwGPIO_Int_Pin_B12:
        case hwGPIO_Int_Pin_B13:
        case hwGPIO_Int_Pin_B14:
        case hwGPIO_Int_Pin_B15:
            socBase = GPIOB;
            break;
        case hwGPIO_Int_Pin_C0:
        case hwGPIO_Int_Pin_C1:
        case hwGPIO_Int_Pin_C2:
        case hwGPIO_Int_Pin_C3:
        case hwGPIO_Int_Pin_C4:
        case hwGPIO_Int_Pin_C5:
        case hwGPIO_Int_Pin_C6:
        case hwGPIO_Int_Pin_C7:
        case hwGPIO_Int_Pin_C8:
        case hwGPIO_Int_Pin_C9:
        case hwGPIO_Int_Pin_C10:
        case hwGPIO_Int_Pin_C11:
        case hwGPIO_Int_Pin_C12:
        case hwGPIO_Int_Pin_C13:
        case hwGPIO_Int_Pin_C14:
        case hwGPIO_Int_Pin_C15:
            socBase = GPIOC;
            break;
        case hwGPIO_Int_Pin_D0:
        case hwGPIO_Int_Pin_D1:
        case hwGPIO_Int_Pin_D2:
        case hwGPIO_Int_Pin_D3:
        case hwGPIO_Int_Pin_D4:
        case hwGPIO_Int_Pin_D5:
        case hwGPIO_Int_Pin_D6:
        case hwGPIO_Int_Pin_D7:
        case hwGPIO_Int_Pin_D8:
        case hwGPIO_Int_Pin_D9:
        case hwGPIO_Int_Pin_D10:
        case hwGPIO_Int_Pin_D11:
        case hwGPIO_Int_Pin_D12:
        case hwGPIO_Int_Pin_D13:
        case hwGPIO_Int_Pin_D14:
        case hwGPIO_Int_Pin_D15:
            socBase = GPIOD;
            break;
        case hwGPIO_Int_Pin_E0:
        case hwGPIO_Int_Pin_E1:
        case hwGPIO_Int_Pin_E2:
        case hwGPIO_Int_Pin_E3:
        case hwGPIO_Int_Pin_E4:
        case hwGPIO_Int_Pin_E5:
        case hwGPIO_Int_Pin_E6:
        case hwGPIO_Int_Pin_E7:
        case hwGPIO_Int_Pin_E8:
        case hwGPIO_Int_Pin_E9:
        case hwGPIO_Int_Pin_E10:
        case hwGPIO_Int_Pin_E11:
        case hwGPIO_Int_Pin_E12:
        case hwGPIO_Int_Pin_E13:
        case hwGPIO_Int_Pin_E14:
        case hwGPIO_Int_Pin_E15:
            socBase = GPIOE;
            break;
        case hwGPIO_Int_Pin_F0:
        case hwGPIO_Int_Pin_F1:
        case hwGPIO_Int_Pin_F2:
        case hwGPIO_Int_Pin_F3:
        case hwGPIO_Int_Pin_F4:
        case hwGPIO_Int_Pin_F5:
        case hwGPIO_Int_Pin_F6:
        case hwGPIO_Int_Pin_F7:
        case hwGPIO_Int_Pin_F8:
        case hwGPIO_Int_Pin_F9:
        case hwGPIO_Int_Pin_F10:
        case hwGPIO_Int_Pin_F11:
        case hwGPIO_Int_Pin_F12:
        case hwGPIO_Int_Pin_F13:
        case hwGPIO_Int_Pin_F14:
        case hwGPIO_Int_Pin_F15:
            socBase = GPIOF;
            break;
        case hwGPIO_Int_Pin_G0:
        case hwGPIO_Int_Pin_G1:
        case hwGPIO_Int_Pin_G2:
        case hwGPIO_Int_Pin_G3:
        case hwGPIO_Int_Pin_G4:
        case hwGPIO_Int_Pin_G5:
        case hwGPIO_Int_Pin_G6:
        case hwGPIO_Int_Pin_G7:
        case hwGPIO_Int_Pin_G8:
        case hwGPIO_Int_Pin_G9:
        case hwGPIO_Int_Pin_G10:
        case hwGPIO_Int_Pin_G11:
        case hwGPIO_Int_Pin_G12:
        case hwGPIO_Int_Pin_G13:
        case hwGPIO_Int_Pin_G15:
            socBase = GPIOG;
            break;
        case hwGPIO_Int_Pin_H0:
        case hwGPIO_Int_Pin_H1:
        case hwGPIO_Int_Pin_H2:
        case hwGPIO_Int_Pin_H3:
        case hwGPIO_Int_Pin_H4:
        case hwGPIO_Int_Pin_H5:
        case hwGPIO_Int_Pin_H6:
        case hwGPIO_Int_Pin_H7:
        case hwGPIO_Int_Pin_H8:
        case hwGPIO_Int_Pin_H9:
        case hwGPIO_Int_Pin_H10:
        case hwGPIO_Int_Pin_H11:
        case hwGPIO_Int_Pin_H12:
        case hwGPIO_Int_Pin_H13:
        case hwGPIO_Int_Pin_H14:
        case hwGPIO_Int_Pin_H15:
            socBase = GPIOH;
            break;
        case hwGPIO_Int_Pin_I0:
        case hwGPIO_Int_Pin_I1:
        case hwGPIO_Int_Pin_I2:
        case hwGPIO_Int_Pin_I3:
        case hwGPIO_Int_Pin_I4:
        case hwGPIO_Int_Pin_I5:
        case hwGPIO_Int_Pin_I6:
        case hwGPIO_Int_Pin_I7:
        case hwGPIO_Int_Pin_I8:
        case hwGPIO_Int_Pin_I9:
        case hwGPIO_Int_Pin_I10:
        case hwGPIO_Int_Pin_I11:
        case hwGPIO_Int_Pin_I12:
        case hwGPIO_Int_Pin_I13:
        case hwGPIO_Int_Pin_I14:
        case hwGPIO_Int_Pin_I15:
            socBase = GPIOI;
            break;
    }

    return socBase;
}

uint16_t GPIO_Int_Map_Soc_Pin(hwGPIO_Int_Pin pin)
{
    uint16_t socPin = 0;
    switch(pin)
    {
        case hwGPIO_Int_Pin_A0:
        case hwGPIO_Int_Pin_B0:
        case hwGPIO_Int_Pin_C0:
        case hwGPIO_Int_Pin_D0:
        case hwGPIO_Int_Pin_E0:
        case hwGPIO_Int_Pin_F0:
        case hwGPIO_Int_Pin_G0:
        case hwGPIO_Int_Pin_H0:
        case hwGPIO_Int_Pin_I0:
            socPin = GPIO_PIN_0;
            break;
        case hwGPIO_Int_Pin_A1:
        case hwGPIO_Int_Pin_B1:
        case hwGPIO_Int_Pin_C1:
        case hwGPIO_Int_Pin_D1:
        case hwGPIO_Int_Pin_E1:
        case hwGPIO_Int_Pin_F1:
        case hwGPIO_Int_Pin_G1:
        case hwGPIO_Int_Pin_H1:
        case hwGPIO_Int_Pin_I1:
            socPin = GPIO_PIN_1;
            break;
        case hwGPIO_Int_Pin_A2:
        case hwGPIO_Int_Pin_B2:
        case hwGPIO_Int_Pin_C2:
        case hwGPIO_Int_Pin_D2:
        case hwGPIO_Int_Pin_E2:
        case hwGPIO_Int_Pin_F2:
        case hwGPIO_Int_Pin_G2:
        case hwGPIO_Int_Pin_H2:
        case hwGPIO_Int_Pin_I2:
            socPin = GPIO_PIN_2;
            break;
        case hwGPIO_Int_Pin_A3:
        case hwGPIO_Int_Pin_B3:
        case hwGPIO_Int_Pin_C3:
        case hwGPIO_Int_Pin_D3:
        case hwGPIO_Int_Pin_E3:
        case hwGPIO_Int_Pin_F3:
        case hwGPIO_Int_Pin_G3:
        case hwGPIO_Int_Pin_H3:
        case hwGPIO_Int_Pin_I3:
            socPin = GPIO_PIN_3;
            break;
        case hwGPIO_Int_Pin_A4:
        case hwGPIO_Int_Pin_B4:
        case hwGPIO_Int_Pin_C4:
        case hwGPIO_Int_Pin_D4:
        case hwGPIO_Int_Pin_E4:
        case hwGPIO_Int_Pin_F4:
        case hwGPIO_Int_Pin_G4:
        case hwGPIO_Int_Pin_H4:
        case hwGPIO_Int_Pin_I4:
            socPin = GPIO_PIN_4;
            break;
        case hwGPIO_Int_Pin_A5:
        case hwGPIO_Int_Pin_B5:
        case hwGPIO_Int_Pin_C5:
        case hwGPIO_Int_Pin_D5:
        case hwGPIO_Int_Pin_E5:
        case hwGPIO_Int_Pin_F5:
        case hwGPIO_Int_Pin_G5:
        case hwGPIO_Int_Pin_H5:
        case hwGPIO_Int_Pin_I5:
            socPin = GPIO_PIN_5;
            break;
        case hwGPIO_Int_Pin_A6:
        case hwGPIO_Int_Pin_B6:
        case hwGPIO_Int_Pin_C6:
        case hwGPIO_Int_Pin_D6:
        case hwGPIO_Int_Pin_E6:
        case hwGPIO_Int_Pin_F6:
        case hwGPIO_Int_Pin_G6:
        case hwGPIO_Int_Pin_H6:
        case hwGPIO_Int_Pin_I6:
            socPin = GPIO_PIN_6;
            break;
        case hwGPIO_Int_Pin_A7:
        case hwGPIO_Int_Pin_B7:
        case hwGPIO_Int_Pin_C7:
        case hwGPIO_Int_Pin_D7:
        case hwGPIO_Int_Pin_E7:
        case hwGPIO_Int_Pin_F7:
        case hwGPIO_Int_Pin_G7:
        case hwGPIO_Int_Pin_H7:
        case hwGPIO_Int_Pin_I7:
            socPin = GPIO_PIN_7;
            break;
        case hwGPIO_Int_Pin_A8:
        case hwGPIO_Int_Pin_B8:
        case hwGPIO_Int_Pin_C8:
        case hwGPIO_Int_Pin_D8:
        case hwGPIO_Int_Pin_E8:
        case hwGPIO_Int_Pin_F8:
        case hwGPIO_Int_Pin_G8:
        case hwGPIO_Int_Pin_H8:
        case hwGPIO_Int_Pin_I8:
            socPin = GPIO_PIN_8;
            break;
        case hwGPIO_Int_Pin_A9:
        case hwGPIO_Int_Pin_B9:
        case hwGPIO_Int_Pin_C9:
        case hwGPIO_Int_Pin_D9:
        case hwGPIO_Int_Pin_E9:
        case hwGPIO_Int_Pin_F9:
        case hwGPIO_Int_Pin_G9:
        case hwGPIO_Int_Pin_H9:
        case hwGPIO_Int_Pin_I9:
            socPin = GPIO_PIN_9;
            break;
        case hwGPIO_Int_Pin_A10:
        case hwGPIO_Int_Pin_B10:
        case hwGPIO_Int_Pin_C10:
        case hwGPIO_Int_Pin_D10:
        case hwGPIO_Int_Pin_E10:
        case hwGPIO_Int_Pin_F10:
        case hwGPIO_Int_Pin_G10:
        case hwGPIO_Int_Pin_H10:
        case hwGPIO_Int_Pin_I10:
            socPin = GPIO_PIN_10;
            break;
        case hwGPIO_Int_Pin_A11:
        case hwGPIO_Int_Pin_B11:
        case hwGPIO_Int_Pin_C11:
        case hwGPIO_Int_Pin_D11:
        case hwGPIO_Int_Pin_E11:
        case hwGPIO_Int_Pin_F11:
        case hwGPIO_Int_Pin_G11:
        case hwGPIO_Int_Pin_H11:
        case hwGPIO_Int_Pin_I11:
            socPin = GPIO_PIN_11;
            break;
        case hwGPIO_Int_Pin_A12:
        case hwGPIO_Int_Pin_B12:
        case hwGPIO_Int_Pin_C12:
        case hwGPIO_Int_Pin_D12:
        case hwGPIO_Int_Pin_E12:
        case hwGPIO_Int_Pin_F12:
        case hwGPIO_Int_Pin_G12:
        case hwGPIO_Int_Pin_H12:
        case hwGPIO_Int_Pin_I12:
            socPin = GPIO_PIN_12;
            break;
        case hwGPIO_Int_Pin_A13:
        case hwGPIO_Int_Pin_B13:
        case hwGPIO_Int_Pin_C13:
        case hwGPIO_Int_Pin_D13:
        case hwGPIO_Int_Pin_E13:
        case hwGPIO_Int_Pin_F13:
        case hwGPIO_Int_Pin_G13:
        case hwGPIO_Int_Pin_H13:
        case hwGPIO_Int_Pin_I13:
            socPin = GPIO_PIN_13;
            break;
        case hwGPIO_Int_Pin_A14:
        case hwGPIO_Int_Pin_B14:
        case hwGPIO_Int_Pin_C14:
        case hwGPIO_Int_Pin_D14:
        case hwGPIO_Int_Pin_E14:
        case hwGPIO_Int_Pin_F14:
        case hwGPIO_Int_Pin_G14:
        case hwGPIO_Int_Pin_H14:
        case hwGPIO_Int_Pin_I14:
            socPin = GPIO_PIN_14;
            break;
        case hwGPIO_Int_Pin_A15:
        case hwGPIO_Int_Pin_B15:
        case hwGPIO_Int_Pin_C15:
        case hwGPIO_Int_Pin_D15:
        case hwGPIO_Int_Pin_E15:
        case hwGPIO_Int_Pin_F15:
        case hwGPIO_Int_Pin_G15:
        case hwGPIO_Int_Pin_H15:
        case hwGPIO_Int_Pin_I15:
            socPin = GPIO_PIN_15;
            break;
    }

    return socPin;
}

uint32_t GPIO_Int_Map_Exti_Port(hwGPIO_Int_Pin pin)
{
    uint32_t extiPort = 0;
    switch(pin)
    {
        case hwGPIO_Int_Pin_A0:
        case hwGPIO_Int_Pin_A1:
        case hwGPIO_Int_Pin_A2:
        case hwGPIO_Int_Pin_A3:
        case hwGPIO_Int_Pin_A4:
        case hwGPIO_Int_Pin_A5:
        case hwGPIO_Int_Pin_A6:
        case hwGPIO_Int_Pin_A7:
        case hwGPIO_Int_Pin_A8:
        case hwGPIO_Int_Pin_A9:
        case hwGPIO_Int_Pin_A10:
        case hwGPIO_Int_Pin_A11:
        case hwGPIO_Int_Pin_A12:
        case hwGPIO_Int_Pin_A13:
        case hwGPIO_Int_Pin_A14:
        case hwGPIO_Int_Pin_A15:
            extiPort = EXTI_GPIOA;
            break;
        case hwGPIO_Int_Pin_B0:
        case hwGPIO_Int_Pin_B1:
        case hwGPIO_Int_Pin_B2:
        case hwGPIO_Int_Pin_B3:
        case hwGPIO_Int_Pin_B4:
        case hwGPIO_Int_Pin_B5:
        case hwGPIO_Int_Pin_B6:
        case hwGPIO_Int_Pin_B7:
        case hwGPIO_Int_Pin_B8:
        case hwGPIO_Int_Pin_B9:
        case hwGPIO_Int_Pin_B10:
        case hwGPIO_Int_Pin_B11:
        case hwGPIO_Int_Pin_B12:
        case hwGPIO_Int_Pin_B13:
        case hwGPIO_Int_Pin_B14:
        case hwGPIO_Int_Pin_B15:
            extiPort = EXTI_GPIOB;
            break;
        case hwGPIO_Int_Pin_C0:
        case hwGPIO_Int_Pin_C1:
        case hwGPIO_Int_Pin_C2:
        case hwGPIO_Int_Pin_C3:
        case hwGPIO_Int_Pin_C4:
        case hwGPIO_Int_Pin_C5:
        case hwGPIO_Int_Pin_C6:
        case hwGPIO_Int_Pin_C7:
        case hwGPIO_Int_Pin_C8:
        case hwGPIO_Int_Pin_C9:
        case hwGPIO_Int_Pin_C10:
        case hwGPIO_Int_Pin_C11:
        case hwGPIO_Int_Pin_C12:
        case hwGPIO_Int_Pin_C13:
        case hwGPIO_Int_Pin_C14:
        case hwGPIO_Int_Pin_C15:
            extiPort = EXTI_GPIOC;
            break;
        case hwGPIO_Int_Pin_D0:
        case hwGPIO_Int_Pin_D1:
        case hwGPIO_Int_Pin_D2:
        case hwGPIO_Int_Pin_D3:
        case hwGPIO_Int_Pin_D4:
        case hwGPIO_Int_Pin_D5:
        case hwGPIO_Int_Pin_D6:
        case hwGPIO_Int_Pin_D7:
        case hwGPIO_Int_Pin_D8:
        case hwGPIO_Int_Pin_D9:
        case hwGPIO_Int_Pin_D10:
        case hwGPIO_Int_Pin_D11:
        case hwGPIO_Int_Pin_D12:
        case hwGPIO_Int_Pin_D13:
        case hwGPIO_Int_Pin_D14:
        case hwGPIO_Int_Pin_D15:
            extiPort = EXTI_GPIOD;
            break;
        case hwGPIO_Int_Pin_E0:
        case hwGPIO_Int_Pin_E1:
        case hwGPIO_Int_Pin_E2:
        case hwGPIO_Int_Pin_E3:
        case hwGPIO_Int_Pin_E4:
        case hwGPIO_Int_Pin_E5:
        case hwGPIO_Int_Pin_E6:
        case hwGPIO_Int_Pin_E7:
        case hwGPIO_Int_Pin_E8:
        case hwGPIO_Int_Pin_E9:
        case hwGPIO_Int_Pin_E10:
        case hwGPIO_Int_Pin_E11:
        case hwGPIO_Int_Pin_E12:
        case hwGPIO_Int_Pin_E13:
        case hwGPIO_Int_Pin_E14:
        case hwGPIO_Int_Pin_E15:
            extiPort = EXTI_GPIOE;
            break;
        case hwGPIO_Int_Pin_F0:
        case hwGPIO_Int_Pin_F1:
        case hwGPIO_Int_Pin_F2:
        case hwGPIO_Int_Pin_F3:
        case hwGPIO_Int_Pin_F4:
        case hwGPIO_Int_Pin_F5:
        case hwGPIO_Int_Pin_F6:
        case hwGPIO_Int_Pin_F7:
        case hwGPIO_Int_Pin_F8:
        case hwGPIO_Int_Pin_F9:
        case hwGPIO_Int_Pin_F10:
        case hwGPIO_Int_Pin_F11:
        case hwGPIO_Int_Pin_F12:
        case hwGPIO_Int_Pin_F13:
        case hwGPIO_Int_Pin_F14:
        case hwGPIO_Int_Pin_F15:
            extiPort = EXTI_GPIOF;
            break;
        case hwGPIO_Int_Pin_G0:
        case hwGPIO_Int_Pin_G1:
        case hwGPIO_Int_Pin_G2:
        case hwGPIO_Int_Pin_G3:
        case hwGPIO_Int_Pin_G4:
        case hwGPIO_Int_Pin_G5:
        case hwGPIO_Int_Pin_G6:
        case hwGPIO_Int_Pin_G7:
        case hwGPIO_Int_Pin_G8:
        case hwGPIO_Int_Pin_G9:
        case hwGPIO_Int_Pin_G10:
        case hwGPIO_Int_Pin_G11:
        case hwGPIO_Int_Pin_G12:
        case hwGPIO_Int_Pin_G13:
        case hwGPIO_Int_Pin_G15:
            extiPort = EXTI_GPIOG;
            break;
        case hwGPIO_Int_Pin_H0:
        case hwGPIO_Int_Pin_H1:
        case hwGPIO_Int_Pin_H2:
        case hwGPIO_Int_Pin_H3:
        case hwGPIO_Int_Pin_H4:
        case hwGPIO_Int_Pin_H5:
        case hwGPIO_Int_Pin_H6:
        case hwGPIO_Int_Pin_H7:
        case hwGPIO_Int_Pin_H8:
        case hwGPIO_Int_Pin_H9:
        case hwGPIO_Int_Pin_H10:
        case hwGPIO_Int_Pin_H11:
        case hwGPIO_Int_Pin_H12:
        case hwGPIO_Int_Pin_H13:
        case hwGPIO_Int_Pin_H14:
        case hwGPIO_Int_Pin_H15:
            extiPort = EXTI_GPIOH;
            break;
        case hwGPIO_Int_Pin_I0:
        case hwGPIO_Int_Pin_I1:
        case hwGPIO_Int_Pin_I2:
        case hwGPIO_Int_Pin_I3:
        case hwGPIO_Int_Pin_I4:
        case hwGPIO_Int_Pin_I5:
        case hwGPIO_Int_Pin_I6:
        case hwGPIO_Int_Pin_I7:
        case hwGPIO_Int_Pin_I8:
        case hwGPIO_Int_Pin_I9:
        case hwGPIO_Int_Pin_I10:
        case hwGPIO_Int_Pin_I11:
        case hwGPIO_Int_Pin_I12:
        case hwGPIO_Int_Pin_I13:
        case hwGPIO_Int_Pin_I14:
        case hwGPIO_Int_Pin_I15:
            extiPort = EXTI_GPIOI;
            break;
    }

    return extiPort;
}

uint32_t GPIO_Int_Map_Exti_Line(hwGPIO_Int_Pin pin)
{
    uint32_t extiLine = 0;
    switch(pin)
    {
        case hwGPIO_Int_Pin_A0:
        case hwGPIO_Int_Pin_B0:
        case hwGPIO_Int_Pin_C0:
        case hwGPIO_Int_Pin_D0:
        case hwGPIO_Int_Pin_E0:
        case hwGPIO_Int_Pin_F0:
        case hwGPIO_Int_Pin_G0:
        case hwGPIO_Int_Pin_H0:
        case hwGPIO_Int_Pin_I0:
            extiLine = EXTI_LINE_0;
            break;
        case hwGPIO_Int_Pin_A1:
        case hwGPIO_Int_Pin_B1:
        case hwGPIO_Int_Pin_C1:
        case hwGPIO_Int_Pin_D1:
        case hwGPIO_Int_Pin_E1:
        case hwGPIO_Int_Pin_F1:
        case hwGPIO_Int_Pin_G1:
        case hwGPIO_Int_Pin_H1:
        case hwGPIO_Int_Pin_I1:
            extiLine = EXTI_LINE_1;
            break;
        case hwGPIO_Int_Pin_A2:
        case hwGPIO_Int_Pin_B2:
        case hwGPIO_Int_Pin_C2:
        case hwGPIO_Int_Pin_D2:
        case hwGPIO_Int_Pin_E2:
        case hwGPIO_Int_Pin_F2:
        case hwGPIO_Int_Pin_G2:
        case hwGPIO_Int_Pin_H2:
        case hwGPIO_Int_Pin_I2:
            extiLine = EXTI_LINE_2;
            break;
        case hwGPIO_Int_Pin_A3:
        case hwGPIO_Int_Pin_B3:
        case hwGPIO_Int_Pin_C3:
        case hwGPIO_Int_Pin_D3:
        case hwGPIO_Int_Pin_E3:
        case hwGPIO_Int_Pin_F3:
        case hwGPIO_Int_Pin_G3:
        case hwGPIO_Int_Pin_H3:
        case hwGPIO_Int_Pin_I3:
            extiLine = EXTI_LINE_3;
            break;
        case hwGPIO_Int_Pin_A4:
        case hwGPIO_Int_Pin_B4:
        case hwGPIO_Int_Pin_C4:
        case hwGPIO_Int_Pin_D4:
        case hwGPIO_Int_Pin_E4:
        case hwGPIO_Int_Pin_F4:
        case hwGPIO_Int_Pin_G4:
        case hwGPIO_Int_Pin_H4:
        case hwGPIO_Int_Pin_I4:
            extiLine = EXTI_LINE_4;
            break;
        case hwGPIO_Int_Pin_A5:
        case hwGPIO_Int_Pin_B5:
        case hwGPIO_Int_Pin_C5:
        case hwGPIO_Int_Pin_D5:
        case hwGPIO_Int_Pin_E5:
        case hwGPIO_Int_Pin_F5:
        case hwGPIO_Int_Pin_G5:
        case hwGPIO_Int_Pin_H5:
        case hwGPIO_Int_Pin_I5:
            extiLine = EXTI_LINE_5;
            break;
        case hwGPIO_Int_Pin_A6:
        case hwGPIO_Int_Pin_B6:
        case hwGPIO_Int_Pin_C6:
        case hwGPIO_Int_Pin_D6:
        case hwGPIO_Int_Pin_E6:
        case hwGPIO_Int_Pin_F6:
        case hwGPIO_Int_Pin_G6:
        case hwGPIO_Int_Pin_H6:
        case hwGPIO_Int_Pin_I6:
            extiLine = EXTI_LINE_6;
            break;
        case hwGPIO_Int_Pin_A7:
        case hwGPIO_Int_Pin_B7:
        case hwGPIO_Int_Pin_C7:
        case hwGPIO_Int_Pin_D7:
        case hwGPIO_Int_Pin_E7:
        case hwGPIO_Int_Pin_F7:
        case hwGPIO_Int_Pin_G7:
        case hwGPIO_Int_Pin_H7:
        case hwGPIO_Int_Pin_I7:
            extiLine = EXTI_LINE_7;
            break;
        case hwGPIO_Int_Pin_A8:
        case hwGPIO_Int_Pin_B8:
        case hwGPIO_Int_Pin_C8:
        case hwGPIO_Int_Pin_D8:
        case hwGPIO_Int_Pin_E8:
        case hwGPIO_Int_Pin_F8:
        case hwGPIO_Int_Pin_G8:
        case hwGPIO_Int_Pin_H8:
        case hwGPIO_Int_Pin_I8:
            extiLine = EXTI_LINE_8;
            break;
        case hwGPIO_Int_Pin_A9:
        case hwGPIO_Int_Pin_B9:
        case hwGPIO_Int_Pin_C9:
        case hwGPIO_Int_Pin_D9:
        case hwGPIO_Int_Pin_E9:
        case hwGPIO_Int_Pin_F9:
        case hwGPIO_Int_Pin_G9:
        case hwGPIO_Int_Pin_H9:
        case hwGPIO_Int_Pin_I9:
            extiLine = EXTI_LINE_9;
            break;
        case hwGPIO_Int_Pin_A10:
        case hwGPIO_Int_Pin_B10:
        case hwGPIO_Int_Pin_C10:
        case hwGPIO_Int_Pin_D10:
        case hwGPIO_Int_Pin_E10:
        case hwGPIO_Int_Pin_F10:
        case hwGPIO_Int_Pin_G10:
        case hwGPIO_Int_Pin_H10:
        case hwGPIO_Int_Pin_I10:
            extiLine = EXTI_LINE_10;
            break;
        case hwGPIO_Int_Pin_A11:
        case hwGPIO_Int_Pin_B11:
        case hwGPIO_Int_Pin_C11:
        case hwGPIO_Int_Pin_D11:
        case hwGPIO_Int_Pin_E11:
        case hwGPIO_Int_Pin_F11:
        case hwGPIO_Int_Pin_G11:
        case hwGPIO_Int_Pin_H11:
        case hwGPIO_Int_Pin_I11:
            extiLine = EXTI_LINE_11;
            break;
        case hwGPIO_Int_Pin_A12:
        case hwGPIO_Int_Pin_B12:
        case hwGPIO_Int_Pin_C12:
        case hwGPIO_Int_Pin_D12:
        case hwGPIO_Int_Pin_E12:
        case hwGPIO_Int_Pin_F12:
        case hwGPIO_Int_Pin_G12:
        case hwGPIO_Int_Pin_H12:
        case hwGPIO_Int_Pin_I12:
            extiLine = EXTI_LINE_12;
            break;
        case hwGPIO_Int_Pin_A13:
        case hwGPIO_Int_Pin_B13:
        case hwGPIO_Int_Pin_C13:
        case hwGPIO_Int_Pin_D13:
        case hwGPIO_Int_Pin_E13:
        case hwGPIO_Int_Pin_F13:
        case hwGPIO_Int_Pin_G13:
        case hwGPIO_Int_Pin_H13:
        case hwGPIO_Int_Pin_I13:
            extiLine = EXTI_LINE_13;
            break;
        case hwGPIO_Int_Pin_A14:
        case hwGPIO_Int_Pin_B14:
        case hwGPIO_Int_Pin_C14:
        case hwGPIO_Int_Pin_D14:
        case hwGPIO_Int_Pin_E14:
        case hwGPIO_Int_Pin_F14:
        case hwGPIO_Int_Pin_G14:
        case hwGPIO_Int_Pin_H14:
        case hwGPIO_Int_Pin_I14:
            extiLine = EXTI_LINE_14;
            break;
        case hwGPIO_Int_Pin_A15:
        case hwGPIO_Int_Pin_B15:
        case hwGPIO_Int_Pin_C15:
        case hwGPIO_Int_Pin_D15:
        case hwGPIO_Int_Pin_E15:
        case hwGPIO_Int_Pin_F15:
        case hwGPIO_Int_Pin_G15:
        case hwGPIO_Int_Pin_H15:
        case hwGPIO_Int_Pin_I15:
            extiLine = EXTI_LINE_15;
            break;
    }

    return extiLine;
}

#endif //STM32
