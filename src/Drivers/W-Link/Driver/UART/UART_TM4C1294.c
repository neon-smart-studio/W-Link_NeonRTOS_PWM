
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "soc.h"

#include "NeonRTOS.h"

#include "UART.h"

#ifdef DEVICE_TM4C1294

#include "UART_Pin.h"

static int UART_BaudRate[hwUART_Index_MAX] = {0};

const uint32_t Map_Soc_UART_Base[hwUART_Index_MAX] = 
{
    UART0_BASE,
    UART1_BASE,
    UART2_BASE,
    UART3_BASE,
    UART4_BASE,
    UART5_BASE,
    UART6_BASE,
    UART7_BASE
};

const uint32_t Map_Soc_UART_SysCtrl_Periph[hwUART_Index_MAX] = 
{
    SYSCTL_PERIPH_UART0,
    SYSCTL_PERIPH_UART1,
    SYSCTL_PERIPH_UART2,
    SYSCTL_PERIPH_UART3,
    SYSCTL_PERIPH_UART4,
    SYSCTL_PERIPH_UART5,
    SYSCTL_PERIPH_UART6,
    SYSCTL_PERIPH_UART7
};

const uint32_t Map_Soc_UART_TxPin_Cfg[hwUART_Index_MAX] = 
{
    GPIO_PA1_U0TX,
    GPIO_PB1_U1TX,
    GPIO_PA7_U2TX,
    GPIO_PA5_U3TX,
    GPIO_PK1_U4TX,
    GPIO_PC7_U5TX,
    GPIO_PP1_U6TX,
    GPIO_PC5_U7TX
};

const uint32_t Map_Soc_UART_RxPin_Cfg[hwUART_Index_MAX] = 
{
    GPIO_PA0_U0RX,
    GPIO_PB0_U1RX,
    GPIO_PA6_U2RX,
    GPIO_PA4_U3RX,
    GPIO_PK0_U4RX,
    GPIO_PC6_U5RX,
    GPIO_PP0_U6RX,
    GPIO_PC4_U7RX
};

hwUART_OpResult UART_Open(hwUART_Index index, uint32_t baudrate, bool rts_cts)
{
    if(index>=hwUART_Index_MAX)
    {
        return hwUART_InvalidParameter;
    }
    
    return UART_Open_Specific_Format(index, baudrate, rts_cts, 8, UART_Parity_None, 1);
}

hwUART_OpResult UART_Open_Specific_Format(hwUART_Index index, uint32_t baudrate, bool rts_cts, uint8_t data_bits, UART_Parity parity, uint8_t stop_bits)
{
    if(index>=hwUART_Index_MAX)
    {
        return hwUART_InvalidParameter;
    }
  
    if(parity>=UART_Parity_MAX)
    {
        return hwUART_InvalidParameter;
    }
    
    if(stop_bits!=1 && stop_bits!=2)
    {
        return hwUART_InvalidParameter;
    }
    
    if(data_bits!=5 && data_bits!=6 && data_bits!=7 && data_bits!=8 && data_bits!=9)
    {
        return hwUART_InvalidParameter;
    }
    
    if(rts_cts)
    {
        return hwUART_Unsupport;
    }
    
    uint32_t txPortBase =  GPIO_Map_Soc_Port_Base(UART_TX_Pins[index]);
    uint32_t txPinMask = GPIO_Map_Soc_Pin_Mask(UART_TX_Pins[index]);
    uint32_t rxPortBase =  GPIO_Map_Soc_Port_Base(UART_RX_Pins[index]);
    uint32_t rxPinMask = GPIO_Map_Soc_Pin_Mask(UART_RX_Pins[index]);

    if(txPortBase==0 || txPinMask==0 || rxPortBase==0 || rxPinMask==0)
    {
      return hwUART_InvalidParameter;
    }

    if(txPortBase != rxPortBase)
    {
        GPIO_Enable_Port_Clock(txPortBase);
        GPIO_Enable_Port_Clock(rxPortBase);
    }
    else
    {
        GPIO_Enable_Port_Clock(txPortBase);
    }

    uint32_t uartBase = Map_Soc_UART_Base[index];
    uint32_t sysCtrlPeriph = Map_Soc_UART_SysCtrl_Periph[index];
    uint32_t txPinCfg = Map_Soc_UART_TxPin_Cfg[index];
    uint32_t rxPinCfg = Map_Soc_UART_RxPin_Cfg[index];

    MAP_SysCtlPeripheralEnable(sysCtrlPeriph);
    MAP_GPIOPinConfigure(txPinCfg);
    MAP_GPIOPinConfigure(rxPinCfg);

    MAP_GPIOPinTypeUART(txPortBase, txPinMask);
    MAP_GPIOPinTypeUART(rxPortBase, rxPinMask);

    uint32_t cfgFlag = 0;

    switch(data_bits)
    {
        case 5:
            cfgFlag |= UART_CONFIG_WLEN_5;
            break;
        case 6:
            cfgFlag |= UART_CONFIG_WLEN_6;
            break;
        case 7:
            cfgFlag |= UART_CONFIG_WLEN_7;
            break;
        case 8:
            cfgFlag |= UART_CONFIG_WLEN_8;
            break;
        case 9:
            UART9BitEnable(uartBase);
            break;
    }

    switch(parity)
    {
        case UART_Parity_None:
            cfgFlag |= UART_CONFIG_PAR_NONE;
            break;
        case UART_Parity_Odd:
            cfgFlag |= UART_CONFIG_PAR_ODD;
            break;
        case UART_Parity_Even:
            cfgFlag |= UART_CONFIG_PAR_EVEN;
            break;
    }

    switch(stop_bits)
    {
        case 1:
            cfgFlag |= UART_CONFIG_STOP_ONE;
            break;
        case 2:
            cfgFlag |= UART_CONFIG_STOP_TWO;
            break;
    }

    MAP_UARTConfigSetExpClk(uartBase, MAP_SysCtlClockGet(), baudrate, cfgFlag);

    MAP_UARTEnable(uartBase);

    UART_BaudRate[index] = baudrate;
    
    return hwUART_OK;
}

hwUART_OpResult UART_Close(hwUART_Index index)
{
    uint32_t txPortBase =  GPIO_Map_Soc_Port_Base(UART_TX_Pins[index]);
    uint32_t txPinMask = GPIO_Map_Soc_Pin_Mask(UART_TX_Pins[index]);
    uint32_t rxPortBase =  GPIO_Map_Soc_Port_Base(UART_RX_Pins[index]);
    uint32_t rxPinMask = GPIO_Map_Soc_Pin_Mask(UART_RX_Pins[index]);

    if(txPortBase==0 || txPinMask==0 || rxPortBase==0 || rxPinMask==0)
    {
      return hwUART_InvalidParameter;
    }

    uint32_t uartBase = Map_Soc_UART_Base[index];
    uint32_t sysCtrlPeriph = Map_Soc_UART_SysCtrl_Periph[index];

    MAP_UARTDisable(uartBase);

    MAP_SysCtlPeripheralDisable(sysCtrlPeriph);

    MAP_GPIOPinTypeGPIOInput(txPortBase, txPinMask);
    MAP_GPIOPinTypeGPIOInput(rxPortBase, rxPinMask);

    return hwUART_OK;
}

hwUART_OpResult UART_Read(hwUART_Index index, uint8_t* data_rd, size_t size, uint32_t timeoutMs)
{
    if(index>=hwUART_Index_MAX)
    {
        return hwUART_InvalidParameter;
    }
    
    if(data_rd==NULL)
    {
        return hwUART_InvalidParameter;
    }
    
    uint32_t uartBase = Map_Soc_UART_Base[index];

    uint16_t recv_bytes = 0;
    NeonRTOS_Time_t wait_ms;
    
    while(recv_bytes<size)
    {
        char recv = 0;

        wait_ms = 0;
        do
        {
            recv = MAP_UARTCharGetNonBlocking(uartBase);

            if(recv<0)
            {
                NeonRTOS_Sleep(1);
                wait_ms++;

                if(timeoutMs>=wait_ms)
                {
                    return hwUART_Busy;
                }
            }

        }while(1);
        
        data_rd[recv_bytes] = recv;

        recv_bytes++;
    }

    return recv_bytes;
}

hwUART_OpResult UART_GetChar(hwUART_Index index, uint8_t* char_rd, uint32_t timeoutMs)
{
    if(index>=hwUART_Index_MAX)
    {
        return hwUART_InvalidParameter;
    }
    
    if(char_rd==NULL)
    {
        return hwUART_InvalidParameter;
    }
    
    return UART_Read(index, char_rd, 1, timeoutMs);
}

hwUART_OpResult UART_Write(hwUART_Index index, uint8_t* data_wr, size_t size, uint32_t timeoutMs)
{
    if(index>=hwUART_Index_MAX)
    {
        return hwUART_InvalidParameter;
    }

    uint32_t uartBase = Map_Soc_UART_Base[index];

    uint16_t send_bytes = 0;
    NeonRTOS_Time_t wait_ms;
    
    while(send_bytes<size)
    {
        wait_ms = 0;
        while(MAP_UARTCharPutNonBlocking(uartBase, data_wr[send_bytes])==false)
        {
            NeonRTOS_Sleep(1);
            wait_ms++;

            if(timeoutMs>=wait_ms)
            {
                return hwUART_Busy;
            }
        }

        send_bytes++;
    }

    return send_bytes;
}

hwUART_OpResult UART_PutChar(hwUART_Index index, uint8_t char_wr, uint32_t timeoutMs)
{
    if(index>=hwUART_Index_MAX)
    {
        return hwUART_InvalidParameter;
    }
    
    return UART_Write(index, &char_wr, 1, timeoutMs);
}

void UART_Printf(const char *format, ...) {
    char buffer[256];
    va_list args;

    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    uint32_t uartBase = Map_Soc_UART_Base[LOG_UART_INDEX];

    NeonRTOS_EnterCritical();

    for(int i = 0; i<len; i++)
    {
        while(MAP_UARTCharPutNonBlocking(uartBase, buffer[i])==false)
        {
            NeonRTOS_Sleep(1);
        }
    }

    NeonRTOS_ExitCritical(0);
}

#endif //DEVICE_TM4C1294