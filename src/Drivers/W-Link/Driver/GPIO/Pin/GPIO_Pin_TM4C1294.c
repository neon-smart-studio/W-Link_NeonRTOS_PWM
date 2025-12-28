
#include <stdbool.h>
#include <stdint.h>

#include "GPIO_Pin_TM4C1294.h"

#include "soc.h"

#ifdef DEVICE_TM4C1294

uint32_t GPIO_Map_Soc_Port_Base(hwGPIO_Pin pin)
{
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
          return GPIO_PORTA_BASE;
        case hwGPIO_Pin_B0:
        case hwGPIO_Pin_B1:
        case hwGPIO_Pin_B2:
        case hwGPIO_Pin_B3:
        case hwGPIO_Pin_B4:
        case hwGPIO_Pin_B5:
          return GPIO_PORTB_BASE;
        case hwGPIO_Pin_C0:
        case hwGPIO_Pin_C1:
        case hwGPIO_Pin_C2:
        case hwGPIO_Pin_C3:
        case hwGPIO_Pin_C4:
        case hwGPIO_Pin_C5:
        case hwGPIO_Pin_C6:
        case hwGPIO_Pin_C7:
          return GPIO_PORTC_BASE;
        case hwGPIO_Pin_D0:
        case hwGPIO_Pin_D1:
        case hwGPIO_Pin_D2:
        case hwGPIO_Pin_D3:
        case hwGPIO_Pin_D4:
        case hwGPIO_Pin_D5:
        case hwGPIO_Pin_D6:
        case hwGPIO_Pin_D7:
          return GPIO_PORTD_BASE;
        case hwGPIO_Pin_E0:
        case hwGPIO_Pin_E1:
        case hwGPIO_Pin_E2:
        case hwGPIO_Pin_E3:
        case hwGPIO_Pin_E4:
        case hwGPIO_Pin_E5:
          return GPIO_PORTE_BASE;
        case hwGPIO_Pin_F0:
        case hwGPIO_Pin_F1:
        case hwGPIO_Pin_F2:
        case hwGPIO_Pin_F3:
        case hwGPIO_Pin_F4:
          return GPIO_PORTF_BASE;
        case hwGPIO_Pin_G0:
        case hwGPIO_Pin_G1:
          return GPIO_PORTG_BASE;
        case hwGPIO_Pin_H0:
        case hwGPIO_Pin_H1:
        case hwGPIO_Pin_H2:
        case hwGPIO_Pin_H3:
          return GPIO_PORTH_BASE;
        case hwGPIO_Pin_J0:
        case hwGPIO_Pin_J1:
          return GPIO_PORTJ_BASE;
        case hwGPIO_Pin_K0:
        case hwGPIO_Pin_K1:
        case hwGPIO_Pin_K2:
        case hwGPIO_Pin_K3:
        case hwGPIO_Pin_K4:
        case hwGPIO_Pin_K5:
        case hwGPIO_Pin_K6:
        case hwGPIO_Pin_K7:
          return GPIO_PORTK_BASE;
        case hwGPIO_Pin_L0:
        case hwGPIO_Pin_L1:
        case hwGPIO_Pin_L2:
        case hwGPIO_Pin_L3:
        case hwGPIO_Pin_L4:
        case hwGPIO_Pin_L5:
        case hwGPIO_Pin_L6:
        case hwGPIO_Pin_L7:
          return GPIO_PORTL_BASE;
        case hwGPIO_Pin_M0:
        case hwGPIO_Pin_M1:
        case hwGPIO_Pin_M2:
        case hwGPIO_Pin_M3:
        case hwGPIO_Pin_M4:
        case hwGPIO_Pin_M5:
        case hwGPIO_Pin_M6:
        case hwGPIO_Pin_M7:
          return GPIO_PORTM_BASE;
        case hwGPIO_Pin_N0:
        case hwGPIO_Pin_N1:
        case hwGPIO_Pin_N2:
        case hwGPIO_Pin_N3:
        case hwGPIO_Pin_N4:
        case hwGPIO_Pin_N5:
          return GPIO_PORTN_BASE;
        case hwGPIO_Pin_P0:
        case hwGPIO_Pin_P1:
        case hwGPIO_Pin_P2:
        case hwGPIO_Pin_P3:
        case hwGPIO_Pin_P4:
        case hwGPIO_Pin_P5:
          return GPIO_PORTP_BASE;
        case hwGPIO_Pin_Q0:
        case hwGPIO_Pin_Q1:
        case hwGPIO_Pin_Q2:
        case hwGPIO_Pin_Q3:
        case hwGPIO_Pin_Q4:
          return GPIO_PORTQ_BASE;
    }

    return 0;
}

uint32_t GPIO_Map_Soc_Port_HighSpeed_Base(hwGPIO_Pin pin)
{
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
          return GPIO_PORTA_AHB_BASE;
        case hwGPIO_Pin_B0:
        case hwGPIO_Pin_B1:
        case hwGPIO_Pin_B2:
        case hwGPIO_Pin_B3:
        case hwGPIO_Pin_B4:
        case hwGPIO_Pin_B5:
          return GPIO_PORTB_AHB_BASE;
        case hwGPIO_Pin_C0:
        case hwGPIO_Pin_C1:
        case hwGPIO_Pin_C2:
        case hwGPIO_Pin_C3:
        case hwGPIO_Pin_C4:
        case hwGPIO_Pin_C5:
        case hwGPIO_Pin_C6:
        case hwGPIO_Pin_C7:
          return GPIO_PORTC_AHB_BASE;
        case hwGPIO_Pin_D0:
        case hwGPIO_Pin_D1:
        case hwGPIO_Pin_D2:
        case hwGPIO_Pin_D3:
        case hwGPIO_Pin_D4:
        case hwGPIO_Pin_D5:
        case hwGPIO_Pin_D6:
        case hwGPIO_Pin_D7:
          return GPIO_PORTD_AHB_BASE;
        case hwGPIO_Pin_E0:
        case hwGPIO_Pin_E1:
        case hwGPIO_Pin_E2:
        case hwGPIO_Pin_E3:
        case hwGPIO_Pin_E4:
        case hwGPIO_Pin_E5:
          return GPIO_PORTE_AHB_BASE;
        case hwGPIO_Pin_F0:
        case hwGPIO_Pin_F1:
        case hwGPIO_Pin_F2:
        case hwGPIO_Pin_F3:
        case hwGPIO_Pin_F4:
          return GPIO_PORTF_AHB_BASE;
        case hwGPIO_Pin_G0:
        case hwGPIO_Pin_G1:
          return GPIO_PORTG_AHB_BASE;
        case hwGPIO_Pin_H0:
        case hwGPIO_Pin_H1:
        case hwGPIO_Pin_H2:
        case hwGPIO_Pin_H3:
          return GPIO_PORTH_AHB_BASE;
        case hwGPIO_Pin_J0:
        case hwGPIO_Pin_J1:
          return GPIO_PORTJ_AHB_BASE;
    }

    return 0;
}

uint32_t GPIO_Map_Soc_Int_Port_Base(hwGPIO_Int_Pin pin)
{
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
          return GPIO_PORTA_BASE;
        case hwGPIO_Int_Pin_B0:
        case hwGPIO_Int_Pin_B1:
        case hwGPIO_Int_Pin_B2:
        case hwGPIO_Int_Pin_B3:
        case hwGPIO_Int_Pin_B4:
        case hwGPIO_Int_Pin_B5:
          return GPIO_PORTB_BASE;
        case hwGPIO_Int_Pin_C0:
        case hwGPIO_Int_Pin_C1:
        case hwGPIO_Int_Pin_C2:
        case hwGPIO_Int_Pin_C3:
        case hwGPIO_Int_Pin_C4:
        case hwGPIO_Int_Pin_C5:
        case hwGPIO_Int_Pin_C6:
        case hwGPIO_Int_Pin_C7:
          return GPIO_PORTC_BASE;
        case hwGPIO_Int_Pin_D0:
        case hwGPIO_Int_Pin_D1:
        case hwGPIO_Int_Pin_D2:
        case hwGPIO_Int_Pin_D3:
        case hwGPIO_Int_Pin_D4:
        case hwGPIO_Int_Pin_D5:
        case hwGPIO_Int_Pin_D6:
        case hwGPIO_Int_Pin_D7:
          return GPIO_PORTD_BASE;
        case hwGPIO_Int_Pin_E0:
        case hwGPIO_Int_Pin_E1:
        case hwGPIO_Int_Pin_E2:
        case hwGPIO_Int_Pin_E3:
        case hwGPIO_Int_Pin_E4:
        case hwGPIO_Int_Pin_E5:
          return GPIO_PORTE_BASE;
        case hwGPIO_Int_Pin_F0:
        case hwGPIO_Int_Pin_F1:
        case hwGPIO_Int_Pin_F2:
        case hwGPIO_Int_Pin_F3:
        case hwGPIO_Int_Pin_F4:
          return GPIO_PORTF_BASE;
        case hwGPIO_Int_Pin_G0:
        case hwGPIO_Int_Pin_G1:
          return GPIO_PORTG_BASE;
        case hwGPIO_Int_Pin_H0:
        case hwGPIO_Int_Pin_H1:
        case hwGPIO_Int_Pin_H2:
        case hwGPIO_Int_Pin_H3:
          return GPIO_PORTH_BASE;
        case hwGPIO_Int_Pin_J0:
        case hwGPIO_Int_Pin_J1:
          return GPIO_PORTJ_BASE;
        case hwGPIO_Int_Pin_K0:
        case hwGPIO_Int_Pin_K1:
        case hwGPIO_Int_Pin_K2:
        case hwGPIO_Int_Pin_K3:
        case hwGPIO_Int_Pin_K4:
        case hwGPIO_Int_Pin_K5:
        case hwGPIO_Int_Pin_K6:
        case hwGPIO_Int_Pin_K7:
          return GPIO_PORTK_BASE;
        case hwGPIO_Int_Pin_L0:
        case hwGPIO_Int_Pin_L1:
        case hwGPIO_Int_Pin_L2:
        case hwGPIO_Int_Pin_L3:
        case hwGPIO_Int_Pin_L4:
        case hwGPIO_Int_Pin_L5:
        case hwGPIO_Int_Pin_L6:
        case hwGPIO_Int_Pin_L7:
          return GPIO_PORTL_BASE;
        case hwGPIO_Int_Pin_M0:
        case hwGPIO_Int_Pin_M1:
        case hwGPIO_Int_Pin_M2:
        case hwGPIO_Int_Pin_M3:
        case hwGPIO_Int_Pin_M4:
        case hwGPIO_Int_Pin_M5:
        case hwGPIO_Int_Pin_M6:
        case hwGPIO_Int_Pin_M7:
          return GPIO_PORTM_BASE;
        case hwGPIO_Int_Pin_N0:
        case hwGPIO_Int_Pin_N1:
        case hwGPIO_Int_Pin_N2:
        case hwGPIO_Int_Pin_N3:
        case hwGPIO_Int_Pin_N4:
        case hwGPIO_Int_Pin_N5:
          return GPIO_PORTN_BASE;
        case hwGPIO_Int_Pin_P0:
        case hwGPIO_Int_Pin_P1:
        case hwGPIO_Int_Pin_P2:
        case hwGPIO_Int_Pin_P3:
        case hwGPIO_Int_Pin_P4:
        case hwGPIO_Int_Pin_P5:
          return GPIO_PORTP_BASE;
        case hwGPIO_Int_Pin_Q0:
        case hwGPIO_Int_Pin_Q1:
        case hwGPIO_Int_Pin_Q2:
        case hwGPIO_Int_Pin_Q3:
        case hwGPIO_Int_Pin_Q4:
          return GPIO_PORTQ_BASE;
    }

    return 0;
}

uint32_t GPIO_Map_Soc_Int_Port_HighSpeed_Base(hwGPIO_Int_Pin pin)
{
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
          return GPIO_PORTA_AHB_BASE;
        case hwGPIO_Int_Pin_B0:
        case hwGPIO_Int_Pin_B1:
        case hwGPIO_Int_Pin_B2:
        case hwGPIO_Int_Pin_B3:
        case hwGPIO_Int_Pin_B4:
        case hwGPIO_Int_Pin_B5:
          return GPIO_PORTB_AHB_BASE;
        case hwGPIO_Int_Pin_C0:
        case hwGPIO_Int_Pin_C1:
        case hwGPIO_Int_Pin_C2:
        case hwGPIO_Int_Pin_C3:
        case hwGPIO_Int_Pin_C4:
        case hwGPIO_Int_Pin_C5:
        case hwGPIO_Int_Pin_C6:
        case hwGPIO_Int_Pin_C7:
          return GPIO_PORTC_AHB_BASE;
        case hwGPIO_Int_Pin_D0:
        case hwGPIO_Int_Pin_D1:
        case hwGPIO_Int_Pin_D2:
        case hwGPIO_Int_Pin_D3:
        case hwGPIO_Int_Pin_D4:
        case hwGPIO_Int_Pin_D5:
        case hwGPIO_Int_Pin_D6:
        case hwGPIO_Int_Pin_D7:
          return GPIO_PORTD_AHB_BASE;
        case hwGPIO_Int_Pin_E0:
        case hwGPIO_Int_Pin_E1:
        case hwGPIO_Int_Pin_E2:
        case hwGPIO_Int_Pin_E3:
        case hwGPIO_Int_Pin_E4:
        case hwGPIO_Int_Pin_E5:
          return GPIO_PORTE_AHB_BASE;
        case hwGPIO_Int_Pin_F0:
        case hwGPIO_Int_Pin_F1:
        case hwGPIO_Int_Pin_F2:
        case hwGPIO_Int_Pin_F3:
        case hwGPIO_Int_Pin_F4:
          return GPIO_PORTF_AHB_BASE;
        case hwGPIO_Int_Pin_G0:
        case hwGPIO_Int_Pin_G1:
          return GPIO_PORTG_AHB_BASE;
        case hwGPIO_Int_Pin_H0:
        case hwGPIO_Int_Pin_H1:
        case hwGPIO_Int_Pin_H2:
        case hwGPIO_Int_Pin_H3:
          return GPIO_PORTH_AHB_BASE;
        case hwGPIO_Int_Pin_J0:
        case hwGPIO_Int_Pin_J1:
          return GPIO_PORTJ_AHB_BASE;
    }

    return 0;
}

uint32_t GPIO_Map_Soc_Pin_Mask(hwGPIO_Pin pin)
{
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
        case hwGPIO_Pin_J0:
        case hwGPIO_Pin_K0:
        case hwGPIO_Pin_L0:
        case hwGPIO_Pin_M0:
        case hwGPIO_Pin_N0:
        case hwGPIO_Pin_P0:
        case hwGPIO_Pin_Q0:
          return GPIO_PIN_0;
        case hwGPIO_Pin_A1:
        case hwGPIO_Pin_B1:
        case hwGPIO_Pin_C1:
        case hwGPIO_Pin_D1:
        case hwGPIO_Pin_E1:
        case hwGPIO_Pin_F1:
        case hwGPIO_Pin_G1:
        case hwGPIO_Pin_H1:
        case hwGPIO_Pin_J1:
        case hwGPIO_Pin_K1:
        case hwGPIO_Pin_L1:
        case hwGPIO_Pin_M1:
        case hwGPIO_Pin_N1:
        case hwGPIO_Pin_P1:
        case hwGPIO_Pin_Q1:
          return GPIO_PIN_1;
        case hwGPIO_Pin_A2:
        case hwGPIO_Pin_B2:
        case hwGPIO_Pin_C2:
        case hwGPIO_Pin_D2:
        case hwGPIO_Pin_E2:
        case hwGPIO_Pin_F2:
        case hwGPIO_Pin_H2:
        case hwGPIO_Pin_K2:
        case hwGPIO_Pin_L2:
        case hwGPIO_Pin_M2:
        case hwGPIO_Pin_N2:
        case hwGPIO_Pin_P2:
        case hwGPIO_Pin_Q2:
          return GPIO_PIN_2;
        case hwGPIO_Pin_A3:
        case hwGPIO_Pin_B3:
        case hwGPIO_Pin_C3:
        case hwGPIO_Pin_D3:
        case hwGPIO_Pin_E3:
        case hwGPIO_Pin_F3:
        case hwGPIO_Pin_H3:
        case hwGPIO_Pin_K3:
        case hwGPIO_Pin_L3:
        case hwGPIO_Pin_M3:
        case hwGPIO_Pin_N3:
        case hwGPIO_Pin_P3:
        case hwGPIO_Pin_Q3:
          return GPIO_PIN_3;
        case hwGPIO_Pin_A4:
        case hwGPIO_Pin_B4:
        case hwGPIO_Pin_C4:
        case hwGPIO_Pin_D4:
        case hwGPIO_Pin_E4:
        case hwGPIO_Pin_F4:
        case hwGPIO_Pin_K4:
        case hwGPIO_Pin_L4:
        case hwGPIO_Pin_M4:
        case hwGPIO_Pin_N4:
        case hwGPIO_Pin_P4:
        case hwGPIO_Pin_Q4:
          return GPIO_PIN_4;
        case hwGPIO_Pin_A5:
        case hwGPIO_Pin_B5:
        case hwGPIO_Pin_C5:
        case hwGPIO_Pin_D5:
        case hwGPIO_Pin_E5:
        case hwGPIO_Pin_K5:
        case hwGPIO_Pin_L5:
        case hwGPIO_Pin_M5:
        case hwGPIO_Pin_N5:
        case hwGPIO_Pin_P5:
          return GPIO_PIN_5;
        case hwGPIO_Pin_A6:
        case hwGPIO_Pin_C6:
        case hwGPIO_Pin_D6:
        case hwGPIO_Pin_K6:
        case hwGPIO_Pin_L6:
        case hwGPIO_Pin_M6:
          return GPIO_PIN_6;
        case hwGPIO_Pin_A7:
        case hwGPIO_Pin_C7:
        case hwGPIO_Pin_D7:
        case hwGPIO_Pin_K7:
        case hwGPIO_Pin_L7:
        case hwGPIO_Pin_M7:
          return GPIO_PIN_7;
    }

    return 0;
}

uint32_t GPIO_Map_Soc_Int_Pin_Mask(hwGPIO_Int_Pin pin)
{
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
        case hwGPIO_Int_Pin_J0:
        case hwGPIO_Int_Pin_K0:
        case hwGPIO_Int_Pin_L0:
        case hwGPIO_Int_Pin_M0:
        case hwGPIO_Int_Pin_N0:
        case hwGPIO_Int_Pin_P0:
        case hwGPIO_Int_Pin_Q0:
          return GPIO_PIN_0;
        case hwGPIO_Int_Pin_A1:
        case hwGPIO_Int_Pin_B1:
        case hwGPIO_Int_Pin_C1:
        case hwGPIO_Int_Pin_D1:
        case hwGPIO_Int_Pin_E1:
        case hwGPIO_Int_Pin_F1:
        case hwGPIO_Int_Pin_G1:
        case hwGPIO_Int_Pin_H1:
        case hwGPIO_Int_Pin_J1:
        case hwGPIO_Int_Pin_K1:
        case hwGPIO_Int_Pin_L1:
        case hwGPIO_Int_Pin_M1:
        case hwGPIO_Int_Pin_N1:
        case hwGPIO_Int_Pin_P1:
        case hwGPIO_Int_Pin_Q1:
          return GPIO_PIN_1;
        case hwGPIO_Int_Pin_A2:
        case hwGPIO_Int_Pin_B2:
        case hwGPIO_Int_Pin_C2:
        case hwGPIO_Int_Pin_D2:
        case hwGPIO_Int_Pin_E2:
        case hwGPIO_Int_Pin_F2:
        case hwGPIO_Int_Pin_H2:
        case hwGPIO_Int_Pin_K2:
        case hwGPIO_Int_Pin_L2:
        case hwGPIO_Int_Pin_M2:
        case hwGPIO_Int_Pin_N2:
        case hwGPIO_Int_Pin_P2:
        case hwGPIO_Int_Pin_Q2:
          return GPIO_PIN_2;
        case hwGPIO_Int_Pin_A3:
        case hwGPIO_Int_Pin_B3:
        case hwGPIO_Int_Pin_C3:
        case hwGPIO_Int_Pin_D3:
        case hwGPIO_Int_Pin_E3:
        case hwGPIO_Int_Pin_F3:
        case hwGPIO_Int_Pin_H3:
        case hwGPIO_Int_Pin_K3:
        case hwGPIO_Int_Pin_L3:
        case hwGPIO_Int_Pin_M3:
        case hwGPIO_Int_Pin_N3:
        case hwGPIO_Int_Pin_P3:
        case hwGPIO_Int_Pin_Q3:
          return GPIO_PIN_3;
        case hwGPIO_Int_Pin_A4:
        case hwGPIO_Int_Pin_B4:
        case hwGPIO_Int_Pin_C4:
        case hwGPIO_Int_Pin_D4:
        case hwGPIO_Int_Pin_E4:
        case hwGPIO_Int_Pin_F4:
        case hwGPIO_Int_Pin_K4:
        case hwGPIO_Int_Pin_L4:
        case hwGPIO_Int_Pin_M4:
        case hwGPIO_Int_Pin_N4:
        case hwGPIO_Int_Pin_P4:
        case hwGPIO_Int_Pin_Q4:
          return GPIO_PIN_4;
        case hwGPIO_Int_Pin_A5:
        case hwGPIO_Int_Pin_B5:
        case hwGPIO_Int_Pin_C5:
        case hwGPIO_Int_Pin_D5:
        case hwGPIO_Int_Pin_E5:
        case hwGPIO_Int_Pin_K5:
        case hwGPIO_Int_Pin_L5:
        case hwGPIO_Int_Pin_M5:
        case hwGPIO_Int_Pin_N5:
        case hwGPIO_Int_Pin_P5:
          return GPIO_PIN_5;
        case hwGPIO_Int_Pin_A6:
        case hwGPIO_Int_Pin_C6:
        case hwGPIO_Int_Pin_D6:
        case hwGPIO_Int_Pin_K6:
        case hwGPIO_Int_Pin_L6:
        case hwGPIO_Int_Pin_M6:
          return GPIO_PIN_6;
        case hwGPIO_Int_Pin_A7:
        case hwGPIO_Int_Pin_C7:
        case hwGPIO_Int_Pin_D7:
        case hwGPIO_Int_Pin_K7:
        case hwGPIO_Int_Pin_L7:
        case hwGPIO_Int_Pin_M7:
          return GPIO_PIN_7;
    }

    return 0;
}

uint32_t GPIO_Map_Pin_Int_Mask(uint32_t pinMask)
{
    switch(pinMask)
    {
        case GPIO_PIN_0:
          return GPIO_INT_PIN_0;
        case GPIO_PIN_1:
          return GPIO_INT_PIN_1;
        case GPIO_PIN_2:
          return GPIO_INT_PIN_2;
        case GPIO_PIN_3:
          return GPIO_INT_PIN_3;
        case GPIO_PIN_4:
          return GPIO_INT_PIN_4;
        case GPIO_PIN_5:
          return GPIO_INT_PIN_5;
        case GPIO_PIN_6:
          return GPIO_INT_PIN_6;
        case GPIO_PIN_7:
          return GPIO_INT_PIN_7;
    }

    return 0;
}

hwGPIO_Int_Pin GPIO_Map_Int_Pin_By_Mask(uint32_t portBase, uint32_t intMask)
{
    switch(portBase)
    {
        case GPIO_PORTA_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_A0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_A1;
              case GPIO_INT_PIN_2:
                return hwGPIO_Int_Pin_A2;
              case GPIO_INT_PIN_3:
                return hwGPIO_Int_Pin_A3;
              case GPIO_INT_PIN_4:
                return hwGPIO_Int_Pin_A4;
              case GPIO_INT_PIN_5:
                return hwGPIO_Int_Pin_A5;
              case GPIO_INT_PIN_6:
                return hwGPIO_Int_Pin_A6;
              case GPIO_INT_PIN_7:
                return hwGPIO_Int_Pin_A7;
          }
          break;
        case GPIO_PORTB_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_B0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_B1;
              case GPIO_INT_PIN_2:
                return hwGPIO_Int_Pin_B2;
              case GPIO_INT_PIN_3:
                return hwGPIO_Int_Pin_B3;
              case GPIO_INT_PIN_4:
                return hwGPIO_Int_Pin_B4;
              case GPIO_INT_PIN_5:
                return hwGPIO_Int_Pin_B5;
          }
          break;
        case GPIO_PORTC_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_C0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_C1;
              case GPIO_INT_PIN_2:
                return hwGPIO_Int_Pin_C2;
              case GPIO_INT_PIN_3:
                return hwGPIO_Int_Pin_C3;
              case GPIO_INT_PIN_4:
                return hwGPIO_Int_Pin_C4;
              case GPIO_INT_PIN_5:
                return hwGPIO_Int_Pin_C5;
              case GPIO_INT_PIN_6:
                return hwGPIO_Int_Pin_C6;
              case GPIO_INT_PIN_7:
                return hwGPIO_Int_Pin_C7;
          }
          break;
        case GPIO_PORTD_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_D0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_D1;
              case GPIO_INT_PIN_2:
                return hwGPIO_Int_Pin_D2;
              case GPIO_INT_PIN_3:
                return hwGPIO_Int_Pin_D3;
              case GPIO_INT_PIN_4:
                return hwGPIO_Int_Pin_D4;
              case GPIO_INT_PIN_5:
                return hwGPIO_Int_Pin_D5;
              case GPIO_INT_PIN_6:
                return hwGPIO_Int_Pin_D6;
              case GPIO_INT_PIN_7:
                return hwGPIO_Int_Pin_D7;
          }
          break;
        case GPIO_PORTE_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_E0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_E1;
              case GPIO_INT_PIN_2:
                return hwGPIO_Int_Pin_E2;
              case GPIO_INT_PIN_3:
                return hwGPIO_Int_Pin_E3;
              case GPIO_INT_PIN_4:
                return hwGPIO_Int_Pin_E4;
              case GPIO_INT_PIN_5:
                return hwGPIO_Int_Pin_E5;
          }
          break;
        case GPIO_PORTF_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_F0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_F1;
              case GPIO_INT_PIN_2:
                return hwGPIO_Int_Pin_F2;
              case GPIO_INT_PIN_3:
                return hwGPIO_Int_Pin_F3;
              case GPIO_INT_PIN_4:
                return hwGPIO_Int_Pin_F4;
          }
          break;
        case GPIO_PORTG_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_G0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_G1;
          }
          break;
        case GPIO_PORTH_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_H0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_H1;
              case GPIO_INT_PIN_2:
                return hwGPIO_Int_Pin_H2;
              case GPIO_INT_PIN_3:
                return hwGPIO_Int_Pin_H3;
          }
          break;
        case GPIO_PORTJ_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_J0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_J1;
          }
          break;
        case GPIO_PORTK_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_K0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_K1;
              case GPIO_INT_PIN_2:
                return hwGPIO_Int_Pin_K2;
              case GPIO_INT_PIN_3:
                return hwGPIO_Int_Pin_K3;
              case GPIO_INT_PIN_4:
                return hwGPIO_Int_Pin_K4;
              case GPIO_INT_PIN_5:
                return hwGPIO_Int_Pin_K5;
              case GPIO_INT_PIN_6:
                return hwGPIO_Int_Pin_K6;
              case GPIO_INT_PIN_7:
                return hwGPIO_Int_Pin_K7;
          }
          break;
        case GPIO_PORTL_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_L0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_L1;
              case GPIO_INT_PIN_2:
                return hwGPIO_Int_Pin_L2;
              case GPIO_INT_PIN_3:
                return hwGPIO_Int_Pin_L3;
              case GPIO_INT_PIN_4:
                return hwGPIO_Int_Pin_L4;
              case GPIO_INT_PIN_5:
                return hwGPIO_Int_Pin_L5;
              case GPIO_INT_PIN_6:
                return hwGPIO_Int_Pin_L6;
              case GPIO_INT_PIN_7:
                return hwGPIO_Int_Pin_L7;
          }
          break;
        case GPIO_PORTM_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_M0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_M1;
              case GPIO_INT_PIN_2:
                return hwGPIO_Int_Pin_M2;
              case GPIO_INT_PIN_3:
                return hwGPIO_Int_Pin_M3;
              case GPIO_INT_PIN_4:
                return hwGPIO_Int_Pin_M4;
              case GPIO_INT_PIN_5:
                return hwGPIO_Int_Pin_M5;
              case GPIO_INT_PIN_6:
                return hwGPIO_Int_Pin_M6;
              case GPIO_INT_PIN_7:
                return hwGPIO_Int_Pin_M7;
          }
          break;
        case GPIO_PORTN_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_N0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_N1;
              case GPIO_INT_PIN_2:
                return hwGPIO_Int_Pin_N2;
              case GPIO_INT_PIN_3:
                return hwGPIO_Int_Pin_N3;
              case GPIO_INT_PIN_4:
                return hwGPIO_Int_Pin_N4;
              case GPIO_INT_PIN_5:
                return hwGPIO_Int_Pin_N5;
          }
          break;
        case GPIO_PORTP_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_P0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_P1;
              case GPIO_INT_PIN_2:
                return hwGPIO_Int_Pin_P2;
              case GPIO_INT_PIN_3:
                return hwGPIO_Int_Pin_P3;
              case GPIO_INT_PIN_4:
                return hwGPIO_Int_Pin_P4;
              case GPIO_INT_PIN_5:
                return hwGPIO_Int_Pin_P5;
          }
          break;
        case GPIO_PORTQ_BASE:
          switch(intMask)
          {
              case GPIO_INT_PIN_0:
                return hwGPIO_Int_Pin_Q0;
              case GPIO_INT_PIN_1:
                return hwGPIO_Int_Pin_Q1;
              case GPIO_INT_PIN_2:
                return hwGPIO_Int_Pin_Q2;
              case GPIO_INT_PIN_3:
                return hwGPIO_Int_Pin_Q3;
              case GPIO_INT_PIN_4:
                return hwGPIO_Int_Pin_Q4;
          }
          break;
    }

    return hwGPIO_Int_Pin_NC;
}

uint32_t GPIO_Enable_Port_Clock(uint32_t portBase)
{
    switch(portBase)
    {
      case GPIO_PORTA_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
        break;
      case GPIO_PORTB_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
        break;
      case GPIO_PORTC_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
        break;
      case GPIO_PORTD_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
        break;
      case GPIO_PORTE_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
        break;
      case GPIO_PORTF_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
        break;
      case GPIO_PORTG_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
        break;
      case GPIO_PORTH_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
        break;
      case GPIO_PORTJ_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
        break;
      case GPIO_PORTK_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
        break;
      case GPIO_PORTL_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
        break;
      case GPIO_PORTM_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
        break;
      case GPIO_PORTN_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
        break;
      case GPIO_PORTP_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
        break;
      case GPIO_PORTQ_BASE:
        MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
        break;
    }
}

#endif //DEVICE_TM4C1294
