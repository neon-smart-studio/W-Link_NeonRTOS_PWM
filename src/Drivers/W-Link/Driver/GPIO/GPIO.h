
#ifndef GPIO_H
#define GPIO_H

#include "soc.h"

#include "Pin/GPIO_Pin.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum hwGPIO_OpStatus_t
{
  hwGPIO_OK = 0,
  hwGPIO_InvalidParameter = -1,
  hwGPIO_PinConflict = -2,
  hwGPIO_HW_Error = -3,
  hwGPIO_Unsupport = -4
} hwGPIO_OpStatus;

typedef enum hwGPIO_Direction_t
{
  hwGPIO_Direction_Input = 0,
  hwGPIO_Direction_Output = 1,
  hwGPIO_Direction_Output_Only = 2,
  hwGPIO_Direction_MAX = 3
} hwGPIO_Direction;

typedef enum hwGPIO_Pull_Mode_t
{
  hwGPIO_Pull_Mode_None = 0,
  hwGPIO_Pull_Mode_Up = 1,
  hwGPIO_Pull_Mode_Down = 2,
  hwGPIO_Pull_Mode_OpenDrain = 3,
  hwGPIO_Pull_Mode_MAX = 4
} hwGPIO_Pull_Mode;

typedef enum hwGPIO_Interrupt_Mode_t
{
  hwGPIO_Interrupt_Mode_Falling_Edge = 0,
  hwGPIO_Interrupt_Mode_Rising_Edge = 1,
  hwGPIO_Interrupt_Mode_Both_Edge = 2,
  hwGPIO_Interrupt_Mode_MAX = 3
} hwGPIO_Interrupt_Mode;

typedef enum hwGPIO_Interrupt_Action_t
{
  hwGPIO_Interrupt_Action_Disbled = 0,
  hwGPIO_Interrupt_Action_Falling_Edge = 1,
  hwGPIO_Interrupt_Action_Rising_Edge = 2,
  hwGPIO_Interrupt_Action_Toggle = 3,
  hwGPIO_Interrupt_Action_MAX = 4
} hwGPIO_Interrupt_Action;

#ifdef STM32
//internal use
extern bool gpio_pin_init_status[hwGPIO_Pin_MAX];
void GPIO_Enable_RCC_Clock(GPIO_TypeDef * base);
void GPIO_Disable_RCC_Clock(GPIO_TypeDef * base);
#endif

bool GPIO_Pin_is_Init(hwGPIO_Pin pin);
hwGPIO_OpStatus GPIO_Pin_Init(hwGPIO_Pin pin, hwGPIO_Direction dir, hwGPIO_Pull_Mode pull_mode);
hwGPIO_OpStatus GPIO_Pin_DeInit(hwGPIO_Pin pin);
hwGPIO_OpStatus GPIO_Pin_Set_Direction(hwGPIO_Pin pin, hwGPIO_Direction dir);
hwGPIO_OpStatus GPIO_Pin_Get_Direction(hwGPIO_Pin pin, hwGPIO_Direction* dir);
hwGPIO_OpStatus GPIO_Pin_Set_PullMode(hwGPIO_Pin pin, hwGPIO_Pull_Mode pull_mode);
hwGPIO_OpStatus GPIO_Pin_Get_PullMode(hwGPIO_Pin pin, hwGPIO_Pull_Mode* pull_mode);
hwGPIO_OpStatus GPIO_Pin_Read(hwGPIO_Pin pin, bool* level);
hwGPIO_OpStatus GPIO_Pin_Write(hwGPIO_Pin pin, bool level);
hwGPIO_OpStatus GPIO_Pin_Toggle(hwGPIO_Pin pin);

typedef void (*GPIO_Interrupt_Event_Handler)(hwGPIO_Int_Pin pin, hwGPIO_Interrupt_Action action);

hwGPIO_OpStatus GPIO_Interrupt_Init(hwGPIO_Int_Pin irq_pin, hwGPIO_Interrupt_Mode mode);
hwGPIO_OpStatus GPIO_Interrupt_DeInit(hwGPIO_Int_Pin irq_pin);
hwGPIO_OpStatus GPIO_Config_Interrupt_Mode(hwGPIO_Int_Pin irq_pin, hwGPIO_Interrupt_Mode mode);
hwGPIO_OpStatus GPIO_Register_Interrupt_Handler(hwGPIO_Int_Pin pin, GPIO_Interrupt_Event_Handler handler);
hwGPIO_OpStatus GPIO_Unregister_Interrupt_Handler(hwGPIO_Int_Pin pin);
bool GPIO_Interrupt_Is_Enabled(hwGPIO_Int_Pin irq_pin);
hwGPIO_OpStatus GPIO_Interrupt_Enable(hwGPIO_Int_Pin pin);
hwGPIO_OpStatus GPIO_Interrupt_Disable(hwGPIO_Int_Pin pin);
hwGPIO_OpStatus GPIO_Interrupt_Pin_Read(hwGPIO_Int_Pin pin, bool* level);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif //GPIO_H