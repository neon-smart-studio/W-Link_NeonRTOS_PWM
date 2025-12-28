#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "soc.h"

#include "NeonRTOS.h"

#include "GPIO.h"

#ifdef STM32

#include "GPIO_STM32.h"

#define GPIO_EXTI_NVIC_PRIORITY 5
#define GPIO_EXTI_NVIC_SUB_PRIORITY 0

typedef enum GPIO_EXTI_IRQn{
  GPIO_EXTI_IRQn_0,
  GPIO_EXTI_IRQn_1,
  GPIO_EXTI_IRQn_2,
  GPIO_EXTI_IRQn_3,
  GPIO_EXTI_IRQn_4,
  GPIO_EXTI_IRQn_5_9,
  GPIO_EXTI_IRQn_10_15,
  GPIO_EXTI_IRQn_MAX,
}GPIO_EXTI_IRQn;

typedef enum GPIO_EXTI_Line{
  GPIO_EXTI_Line_0,
  GPIO_EXTI_Line_1,
  GPIO_EXTI_Line_2,
  GPIO_EXTI_Line_3,
  GPIO_EXTI_Line_4,
  GPIO_EXTI_Line_5,
  GPIO_EXTI_Line_6,
  GPIO_EXTI_Line_7,
  GPIO_EXTI_Line_8,
  GPIO_EXTI_Line_9,
  GPIO_EXTI_Line_10,
  GPIO_EXTI_Line_11,
  GPIO_EXTI_Line_12,
  GPIO_EXTI_Line_13,
  GPIO_EXTI_Line_14,
  GPIO_EXTI_Line_15,
  GPIO_EXTI_Line_MAX,
}GPIO_EXTI_Line;

typedef struct GPIO_EXTI_Desc{
    hwGPIO_Int_Pin irq_pin;
    hwGPIO_Interrupt_Mode mode;
    EXTI_HandleTypeDef hexti;
} GPIO_EXTI_Desc;

bool gpio_pin_init_status[hwGPIO_Pin_MAX] = {false};

static hwGPIO_Direction gpio_current_dir[hwGPIO_Pin_MAX] = {hwGPIO_Direction_Input};
static hwGPIO_Pull_Mode gpio_current_mode[hwGPIO_Pin_MAX] = {hwGPIO_Pull_Mode_None};

static bool gpio_exti_enable_status[GPIO_EXTI_IRQn_MAX] = {false};
static GPIO_EXTI_Desc gpio_exti_desc[GPIO_EXTI_Line_MAX] = {0};
static GPIO_Interrupt_Event_Handler gpio_irq_handlers[hwGPIO_Int_Pin_MAX] = {NULL};

bool GPIO_Pin_is_Init(hwGPIO_Pin pin)
{
    if(pin>=hwGPIO_Pin_MAX)
    {
      return false;
    }
    
    return gpio_pin_init_status[pin];
}

void GPIO_Enable_RCC_Clock(GPIO_TypeDef * base)
{
  if(base==GPIOA)
  {
    __HAL_RCC_GPIOA_CLK_ENABLE();
  }
  else if(base==GPIOB)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
  }
  else if(base==GPIOC)
  {
    __HAL_RCC_GPIOC_CLK_ENABLE();
  }
  else if(base==GPIOD)
  {
    __HAL_RCC_GPIOD_CLK_ENABLE();
  }
  else if(base==GPIOE)
  {
    __HAL_RCC_GPIOE_CLK_ENABLE();
  }
  else if(base==GPIOF)
  {
    __HAL_RCC_GPIOF_CLK_ENABLE();
  }
  else if(base==GPIOG)
  {
    __HAL_RCC_GPIOG_CLK_ENABLE();
  }
  else if(base==GPIOH)
  {
    __HAL_RCC_GPIOH_CLK_ENABLE();
  }
}

void GPIO_Disable_RCC_Clock(GPIO_TypeDef * base)
{
  if(base==GPIOA)
  {
    __HAL_RCC_GPIOA_CLK_DISABLE();
  }
  else if(base==GPIOB)
  {
    __HAL_RCC_GPIOB_CLK_DISABLE();
  }
  else if(base==GPIOC)
  {
    __HAL_RCC_GPIOC_CLK_DISABLE();
  }
  else if(base==GPIOD)
  {
    __HAL_RCC_GPIOD_CLK_DISABLE();
  }
  else if(base==GPIOE)
  {
    __HAL_RCC_GPIOE_CLK_DISABLE();
  }
  else if(base==GPIOF)
  {
    __HAL_RCC_GPIOF_CLK_DISABLE();
  }
  else if(base==GPIOG)
  {
    __HAL_RCC_GPIOG_CLK_DISABLE();
  }
  else if(base==GPIOH)
  {
    __HAL_RCC_GPIOH_CLK_DISABLE();
  }
}

hwGPIO_OpStatus GPIO_Pin_Init(hwGPIO_Pin pin, hwGPIO_Direction dir, hwGPIO_Pull_Mode pull_mode)
{
    if(pin>=hwGPIO_Pin_MAX)
    {
      return hwGPIO_InvalidParameter;
    }
    
    if(dir>=hwGPIO_Direction_MAX)
    {
      return hwGPIO_InvalidParameter;
    }
    
    if(pull_mode>=hwGPIO_Pull_Mode_MAX)
    {
      return hwGPIO_InvalidParameter;
    }
  
    if(gpio_pin_init_status[pin]==true)
    {
      return hwGPIO_PinConflict;
    }
    
    uint16_t soc_pin = GPIO_Map_Soc_Pin(pin);
    GPIO_TypeDef * soc_base = GPIO_Map_Soc_Base(pin);

    if(soc_pin==0 || soc_base==NULL)
    {
      return hwGPIO_InvalidParameter;
    }
    
    GPIO_Enable_RCC_Clock(soc_base);

    GPIO_InitTypeDef gpio_pin_init = {0};
    gpio_pin_init.Pin = soc_pin;

    switch(dir)
    {
        case hwGPIO_Direction_Input:
            gpio_pin_init.Mode = GPIO_MODE_INPUT;
            break;
        case hwGPIO_Direction_Output:
        case hwGPIO_Direction_Output_Only:
            switch(pull_mode)
            {
                case hwGPIO_Pull_Mode_None:
                    gpio_pin_init.Mode = GPIO_MODE_OUTPUT_PP;
                    gpio_pin_init.Pull = GPIO_NOPULL;
                    break;
                case hwGPIO_Pull_Mode_Up:
                    gpio_pin_init.Mode = GPIO_MODE_OUTPUT_PP;
                    gpio_pin_init.Pull = GPIO_PULLUP;
                    break;
                case hwGPIO_Pull_Mode_Down:
                    gpio_pin_init.Mode = GPIO_MODE_OUTPUT_PP;
                    gpio_pin_init.Pull = GPIO_PULLDOWN;
                    break;
                case hwGPIO_Pull_Mode_OpenDrain:
                    gpio_pin_init.Mode = GPIO_MODE_OUTPUT_OD;
                    break;
            }
            break;
    }
    
    gpio_pin_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    HAL_GPIO_Init(soc_base, &gpio_pin_init);

    gpio_current_dir[pin] = dir;
    gpio_current_mode[pin] = pull_mode;

    gpio_pin_init_status[pin] = true;
    
    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Pin_DeInit(hwGPIO_Pin pin)
{
    if(gpio_pin_init_status[pin]==false)
    {
      return hwGPIO_OK;
    }
    
    uint16_t soc_pin = GPIO_Map_Soc_Pin(pin);
    GPIO_TypeDef * soc_base = GPIO_Map_Soc_Base(pin);

    if(soc_pin==0 || soc_base==NULL)
    {
      return hwGPIO_InvalidParameter;
    }
    
    HAL_GPIO_DeInit(soc_base, soc_pin);
    
    gpio_current_dir[pin] = hwGPIO_Direction_Input;
    gpio_current_mode[pin] = hwGPIO_Pull_Mode_None;
    
    gpio_pin_init_status[pin] = false;
    
    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Pin_Set_Direction(hwGPIO_Pin pin, hwGPIO_Direction dir)
{
    if(pin>=hwGPIO_Pin_MAX)
    {
      return hwGPIO_InvalidParameter;
    }
    
    if(dir>=hwGPIO_Direction_MAX)
    {
      return hwGPIO_InvalidParameter;
    }
  
    if(gpio_current_dir[pin]==hwGPIO_Direction_Output_Only)
    {
      return hwGPIO_Unsupport;
    }

    uint16_t soc_pin = GPIO_Map_Soc_Pin(pin);
    GPIO_TypeDef * soc_base = GPIO_Map_Soc_Base(pin);

    if(soc_pin==0 || soc_base==NULL)
    {
      return hwGPIO_InvalidParameter;
    }
    
    GPIO_InitTypeDef gpio_pin_init = {0};
    gpio_pin_init.Pin = soc_pin;

    switch(dir)
    {
        case hwGPIO_Direction_Input:
            gpio_pin_init.Mode = GPIO_MODE_INPUT;
            break;
        case hwGPIO_Direction_Output:
        case hwGPIO_Direction_Output_Only:
            switch(gpio_current_mode[pin])
            {
                case hwGPIO_Pull_Mode_None:
                    gpio_pin_init.Mode = GPIO_MODE_OUTPUT_PP;
                    gpio_pin_init.Pull = GPIO_NOPULL;
                    break;
                case hwGPIO_Pull_Mode_Up:
                    gpio_pin_init.Mode = GPIO_MODE_OUTPUT_PP;
                    gpio_pin_init.Pull = GPIO_PULLUP;
                    break;
                case hwGPIO_Pull_Mode_Down:
                    gpio_pin_init.Mode = GPIO_MODE_OUTPUT_PP;
                    gpio_pin_init.Pull = GPIO_PULLDOWN;
                    break;
                case hwGPIO_Pull_Mode_OpenDrain:
                    gpio_pin_init.Mode = GPIO_MODE_OUTPUT_OD;
                    break;
            }
            break;
    }
    
    gpio_pin_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    HAL_GPIO_Init(soc_base, &gpio_pin_init);

    gpio_current_dir[pin] = dir;
    
    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Pin_Get_Direction(hwGPIO_Pin pin, hwGPIO_Direction* dir)
{
    if(pin>=hwGPIO_Pin_MAX)
    {
      return hwGPIO_InvalidParameter;
    }
    
    if(dir==NULL)
    {
      return hwGPIO_InvalidParameter;
    }

    *dir = gpio_current_dir[pin];
  
    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Pin_Set_PullMode(hwGPIO_Pin pin, hwGPIO_Pull_Mode pull_mode)
{
    if(pin>=hwGPIO_Pin_MAX)
    {
      return hwGPIO_InvalidParameter;
    }
    
    if(pull_mode>=hwGPIO_Pull_Mode_MAX)
    {
      return hwGPIO_InvalidParameter;
    }
  
    uint16_t soc_pin = GPIO_Map_Soc_Pin(pin);
    GPIO_TypeDef * soc_base = GPIO_Map_Soc_Base(pin);

    if(soc_pin==0 || soc_base==NULL)
    {
      return hwGPIO_InvalidParameter;
    }
    
    GPIO_InitTypeDef gpio_pin_init = {0};
    gpio_pin_init.Pin = soc_pin;

    switch(gpio_current_dir[pin])
    {
        case hwGPIO_Direction_Input:
            gpio_pin_init.Mode = GPIO_MODE_INPUT;
            break;
        case hwGPIO_Direction_Output:
        case hwGPIO_Direction_Output_Only:
            switch(pull_mode)
            {
                case hwGPIO_Pull_Mode_None:
                    gpio_pin_init.Mode = GPIO_MODE_OUTPUT_PP;
                    gpio_pin_init.Pull = GPIO_NOPULL;
                    break;
                case hwGPIO_Pull_Mode_Up:
                    gpio_pin_init.Mode = GPIO_MODE_OUTPUT_PP;
                    gpio_pin_init.Pull = GPIO_PULLUP;
                    break;
                case hwGPIO_Pull_Mode_Down:
                    gpio_pin_init.Mode = GPIO_MODE_OUTPUT_PP;
                    gpio_pin_init.Pull = GPIO_PULLDOWN;
                    break;
                case hwGPIO_Pull_Mode_OpenDrain:
                    gpio_pin_init.Mode = GPIO_MODE_OUTPUT_OD;
                    break;
            }
            break;
    }
    
    gpio_pin_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    HAL_GPIO_Init(soc_base, &gpio_pin_init);

    gpio_current_mode[pin] = pull_mode;
    
    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Pin_Get_PullMode(hwGPIO_Pin pin, hwGPIO_Pull_Mode* pull_mode)
{
    if(pin>=hwGPIO_Pin_MAX)
    {
      return hwGPIO_InvalidParameter;
    }
    
    if(pull_mode==NULL)
    {
      return hwGPIO_InvalidParameter;
    }
  
    *pull_mode = gpio_current_mode[pin];
  
    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Pin_Read(hwGPIO_Pin pin, bool* level)
{
    if(pin>=hwGPIO_Pin_MAX)
    {
      return hwGPIO_InvalidParameter;
    }
    
    if(level==NULL)
    {
      return hwGPIO_InvalidParameter;
    }
    
    if(gpio_current_dir[pin]==hwGPIO_Direction_Output_Only)
    {
      return hwGPIO_Unsupport;
    }

    uint16_t soc_pin = GPIO_Map_Soc_Pin(pin);
    GPIO_TypeDef * soc_base = GPIO_Map_Soc_Base(pin);

    if(soc_pin==0 || soc_base==NULL)
    {
      return hwGPIO_InvalidParameter;
    }
    
    *level = (HAL_GPIO_ReadPin(soc_base, soc_pin)==GPIO_PIN_SET) ? 1: 0;
    
    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Pin_Write(hwGPIO_Pin pin, bool level)
{
    if(pin>=hwGPIO_Pin_MAX)
    {
      return hwGPIO_InvalidParameter;
    }
  
    if(gpio_current_dir[pin]==hwGPIO_Direction_Input)
    {
      return hwGPIO_Unsupport;
    }

    uint16_t soc_pin = GPIO_Map_Soc_Pin(pin);
    GPIO_TypeDef * soc_base = GPIO_Map_Soc_Base(pin);

    if(soc_pin==0 || soc_base==NULL)
    {
      return hwGPIO_InvalidParameter;
    }
    
    if(level)
    {
      HAL_GPIO_WritePin(soc_base, soc_pin, GPIO_PIN_SET);
    }
    else
    {
      HAL_GPIO_WritePin(soc_base, soc_pin, GPIO_PIN_RESET);
    }
    
    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Pin_Toggle(hwGPIO_Pin pin)
{
    if(pin>=hwGPIO_Pin_MAX)
    {
      return hwGPIO_InvalidParameter;
    }
  
    if(gpio_current_dir[pin]==hwGPIO_Direction_Input)
    {
      return hwGPIO_Unsupport;
    }

    uint16_t soc_pin = GPIO_Map_Soc_Pin(pin);
    GPIO_TypeDef * soc_base = GPIO_Map_Soc_Base(pin);

    if(soc_pin==0 || soc_base==NULL)
    {
      return hwGPIO_InvalidParameter;
    }
    
    HAL_GPIO_TogglePin(soc_base, soc_pin);
    
    return hwGPIO_OK;
}

static void GPIO_EXTI_Dispatch(GPIO_EXTI_Line line)
{
    GPIO_EXTI_Desc *d = &gpio_exti_desc[line];

    uint16_t soc_pin = GPIO_Int_Map_Soc_Pin(d->irq_pin);
    GPIO_TypeDef * soc_base = GPIO_Int_Map_Soc_Base(d->irq_pin);

    if(soc_pin==0 || soc_base==NULL)
    {
      return;
    }
    
    if (__HAL_GPIO_EXTI_GET_IT(soc_pin) == RESET)
        return;

    __HAL_GPIO_EXTI_CLEAR_IT(soc_pin);

    if(gpio_irq_handlers[d->irq_pin]!=NULL)
    {
        switch(d->mode)
        {
          case hwGPIO_Interrupt_Mode_Falling_Edge:
            gpio_irq_handlers[d->irq_pin](d->irq_pin, hwGPIO_Interrupt_Action_Falling_Edge);
            break;
          case hwGPIO_Interrupt_Mode_Rising_Edge:
            gpio_irq_handlers[d->irq_pin](d->irq_pin, hwGPIO_Interrupt_Action_Rising_Edge);
            break;
          case hwGPIO_Interrupt_Mode_Both_Edge:
            gpio_irq_handlers[d->irq_pin](d->irq_pin, hwGPIO_Interrupt_Action_Toggle);
            break;
        }
    }
}

void EXTI0_IRQHandler(void) { GPIO_EXTI_Dispatch(GPIO_EXTI_Line_0); }
void EXTI1_IRQHandler(void) { GPIO_EXTI_Dispatch(GPIO_EXTI_Line_1); }
void EXTI2_IRQHandler(void) { GPIO_EXTI_Dispatch(GPIO_EXTI_Line_2); }
void EXTI3_IRQHandler(void) { GPIO_EXTI_Dispatch(GPIO_EXTI_Line_3); }
void EXTI4_IRQHandler(void) { GPIO_EXTI_Dispatch(GPIO_EXTI_Line_4); }
void EXTI9_5_IRQHandler(void)
{
  GPIO_EXTI_Dispatch(GPIO_EXTI_Line_5);
  GPIO_EXTI_Dispatch(GPIO_EXTI_Line_6);
  GPIO_EXTI_Dispatch(GPIO_EXTI_Line_7);
  GPIO_EXTI_Dispatch(GPIO_EXTI_Line_8);
  GPIO_EXTI_Dispatch(GPIO_EXTI_Line_9);
}
void EXTI15_10_IRQHandler(void)
{
  GPIO_EXTI_Dispatch(GPIO_EXTI_Line_10);
  GPIO_EXTI_Dispatch(GPIO_EXTI_Line_11);
  GPIO_EXTI_Dispatch(GPIO_EXTI_Line_12);
  GPIO_EXTI_Dispatch(GPIO_EXTI_Line_13);
  GPIO_EXTI_Dispatch(GPIO_EXTI_Line_14);
  GPIO_EXTI_Dispatch(GPIO_EXTI_Line_15);
}

static GPIO_EXTI_Line GPIO_Map_EXTI_Line_Index(uint32_t exti_line)
{
    switch(exti_line)
    {
        case EXTI_LINE_0:
            return GPIO_EXTI_Line_0;
        case EXTI_LINE_1:
            return GPIO_EXTI_Line_1;
        case EXTI_LINE_2:
            return GPIO_EXTI_Line_2;
        case EXTI_LINE_3:
            return GPIO_EXTI_Line_3;
        case EXTI_LINE_4:
            return GPIO_EXTI_Line_4;
        case EXTI_LINE_5:
            return GPIO_EXTI_Line_5;
        case EXTI_LINE_6:
            return GPIO_EXTI_Line_6;
        case EXTI_LINE_7:
            return GPIO_EXTI_Line_7;
        case EXTI_LINE_8:
            return GPIO_EXTI_Line_8;
        case EXTI_LINE_9:
            return GPIO_EXTI_Line_9;
        case EXTI_LINE_10:
            return GPIO_EXTI_Line_10;
        case EXTI_LINE_11:
            return GPIO_EXTI_Line_11;
        case EXTI_LINE_12:
            return GPIO_EXTI_Line_12;
        case EXTI_LINE_13:
            return GPIO_EXTI_Line_13;
        case EXTI_LINE_14:
            return GPIO_EXTI_Line_14;
        case EXTI_LINE_15:
            return GPIO_EXTI_Line_15;
    }
    return GPIO_EXTI_Line_MAX;
}

hwGPIO_OpStatus GPIO_Interrupt_Init(hwGPIO_Int_Pin irq_pin, hwGPIO_Interrupt_Mode mode)
{
    if(irq_pin>=hwGPIO_Int_Pin_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
  
    if(mode>=hwGPIO_Interrupt_Mode_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
  
    if(gpio_pin_init_status[irq_pin]==true)
    {
        return hwGPIO_PinConflict;
    }
    
    uint16_t soc_pin = GPIO_Int_Map_Soc_Pin(irq_pin);
    GPIO_TypeDef * soc_base = GPIO_Int_Map_Soc_Base(irq_pin);

    if(soc_pin==0 || soc_base==NULL)
    {
      return hwGPIO_InvalidParameter;
    }
    
    uint32_t exti_port = GPIO_Int_Map_Exti_Port(irq_pin);
    uint32_t exti_line = GPIO_Int_Map_Exti_Line(irq_pin);
    GPIO_EXTI_Line exti_line_idx = GPIO_Map_EXTI_Line_Index(exti_line);

    if(exti_line_idx>=GPIO_EXTI_Line_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
  
    GPIO_Enable_RCC_Clock(soc_base);

    GPIO_InitTypeDef gpio_irq_init = {0};

    switch(mode)
    {
        case hwGPIO_Interrupt_Mode_Falling_Edge:
            gpio_irq_init.Mode = GPIO_MODE_IT_FALLING;
            break;
        case hwGPIO_Interrupt_Mode_Rising_Edge:
            gpio_irq_init.Mode = GPIO_MODE_IT_RISING;
            break;
        case hwGPIO_Interrupt_Mode_Both_Edge:
            gpio_irq_init.Mode = GPIO_MODE_IT_RISING_FALLING;
            break;
        default:
            return hwGPIO_InvalidParameter;
    }
    
    gpio_irq_init.Pull = GPIO_NOPULL;
    gpio_irq_init.Pin = soc_pin;

    HAL_GPIO_Init(soc_base, &gpio_irq_init);
    
    EXTI_ConfigTypeDef cfg = {
        .Line = exti_line,
        .Mode = EXTI_MODE_INTERRUPT,
        .Trigger = EXTI_TRIGGER_NONE,
        .GPIOSel = exti_port,
    };

    switch(mode)
    {
        case hwGPIO_Interrupt_Mode_Falling_Edge:
            cfg.Trigger = EXTI_TRIGGER_FALLING;
            break;
        case hwGPIO_Interrupt_Mode_Rising_Edge:
            cfg.Trigger = EXTI_TRIGGER_RISING;
            break;
        case hwGPIO_Interrupt_Mode_Both_Edge:
            cfg.Trigger = EXTI_TRIGGER_RISING_FALLING;
            break;
        default:
            return hwGPIO_InvalidParameter;
    }
    
    gpio_exti_desc[exti_line_idx].irq_pin = irq_pin;
    gpio_exti_desc[exti_line_idx].mode = mode;

    HAL_EXTI_SetConfigLine(&gpio_exti_desc[exti_line_idx].hexti, &cfg);
    
    if(exti_line_idx==GPIO_EXTI_Line_0)
    {
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_0]==false)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_0] = true;
        HAL_NVIC_SetPriority(EXTI0_IRQn, GPIO_EXTI_NVIC_PRIORITY, GPIO_EXTI_NVIC_SUB_PRIORITY);
        HAL_NVIC_EnableIRQ(EXTI0_IRQn);
      }
    }

    if(exti_line_idx==GPIO_EXTI_Line_1)
    {
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_1]==false)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_1] = true;
        HAL_NVIC_SetPriority(EXTI1_IRQn, GPIO_EXTI_NVIC_PRIORITY, GPIO_EXTI_NVIC_SUB_PRIORITY);
        HAL_NVIC_EnableIRQ(EXTI1_IRQn);
      }
    }

    if(exti_line_idx==GPIO_EXTI_Line_2)
    {
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_2]==false)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_2] = true;
        HAL_NVIC_SetPriority(EXTI2_IRQn, GPIO_EXTI_NVIC_PRIORITY, GPIO_EXTI_NVIC_SUB_PRIORITY);
        HAL_NVIC_EnableIRQ(EXTI2_IRQn);
      }
    }

    if(exti_line_idx==GPIO_EXTI_Line_3)
    {
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_3]==false)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_3] = true;
        HAL_NVIC_SetPriority(EXTI3_IRQn, GPIO_EXTI_NVIC_PRIORITY, GPIO_EXTI_NVIC_SUB_PRIORITY);
        HAL_NVIC_EnableIRQ(EXTI3_IRQn);
      }
    }

    if(exti_line_idx==GPIO_EXTI_Line_4)
    {
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_4]==false)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_4] = true;
        HAL_NVIC_SetPriority(EXTI4_IRQn, GPIO_EXTI_NVIC_PRIORITY, GPIO_EXTI_NVIC_SUB_PRIORITY);
        HAL_NVIC_EnableIRQ(EXTI4_IRQn);
      }
    }

    if(exti_line_idx>=GPIO_EXTI_Line_5 && exti_line_idx<=GPIO_EXTI_Line_9)
    {
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_5_9]==false)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_5_9] = true;
        HAL_NVIC_SetPriority(EXTI9_5_IRQn, GPIO_EXTI_NVIC_PRIORITY, GPIO_EXTI_NVIC_SUB_PRIORITY);
        HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
      }
    }

    if(exti_line_idx>=GPIO_EXTI_Line_10 && exti_line_idx<=GPIO_EXTI_Line_15)
    {
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_10_15]==false)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_10_15] = true;
        HAL_NVIC_SetPriority(EXTI15_10_IRQn, GPIO_EXTI_NVIC_PRIORITY, GPIO_EXTI_NVIC_SUB_PRIORITY);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
      }
    }

    gpio_pin_init_status[irq_pin] = true;
    
    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Interrupt_DeInit(hwGPIO_Int_Pin irq_pin)
{
    if(irq_pin>=hwGPIO_Int_Pin_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
    
    if(gpio_pin_init_status[irq_pin]==false)
    {
        return hwGPIO_OK;
    }
    
    uint16_t soc_pin = GPIO_Int_Map_Soc_Pin(irq_pin);
    GPIO_TypeDef * soc_base = GPIO_Int_Map_Soc_Base(irq_pin);

    if(soc_pin==0 || soc_base==NULL)
    {
      return hwGPIO_InvalidParameter;
    }
    
    uint32_t exti_line = GPIO_Int_Map_Exti_Line(irq_pin);
    GPIO_EXTI_Line exti_line_idx = GPIO_Map_EXTI_Line_Index(exti_line);

    if(exti_line_idx>=GPIO_EXTI_Line_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
  
    gpio_exti_desc[exti_line_idx].irq_pin = hwGPIO_Int_Pin_NC;

    if(exti_line_idx==GPIO_EXTI_Line_0)
    {
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_0]==true)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_0] = false;
        HAL_NVIC_DisableIRQ(EXTI0_IRQn);
      }
    }

    if(exti_line_idx==GPIO_EXTI_Line_1)
    {
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_1]==true)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_1] = false;
        HAL_NVIC_DisableIRQ(EXTI1_IRQn);
      }
    }

    if(exti_line_idx==GPIO_EXTI_Line_2)
    {
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_2]==true)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_2] = false;
        HAL_NVIC_DisableIRQ(EXTI2_IRQn);
      }
    }

    if(exti_line_idx==GPIO_EXTI_Line_3)
    {
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_3]==true)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_3] = false;
        HAL_NVIC_DisableIRQ(EXTI3_IRQn);
      }
    }

    if(exti_line_idx==GPIO_EXTI_Line_4)
    {
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_4]==true)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_4] = false;
        HAL_NVIC_DisableIRQ(EXTI4_IRQn);
      }
    }

    if(exti_line_idx>=GPIO_EXTI_Line_5 && exti_line_idx<=GPIO_EXTI_Line_9)
    {
      bool irq_5_9_used = false;
      for(GPIO_EXTI_Line idx = GPIO_EXTI_Line_5; idx<=GPIO_EXTI_Line_9; idx++)
      {
        if(gpio_exti_desc[exti_line_idx].irq_pin!=hwGPIO_Int_Pin_NC)
        {
          irq_5_9_used = true;
        }
      }
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_5_9]==true && irq_5_9_used==false)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_5_9] = false;
        HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
      }
    }

    if(exti_line_idx>=GPIO_EXTI_Line_10 && exti_line_idx<=GPIO_EXTI_Line_15)
    {
      bool irq_10_15_used = false;
      for(GPIO_EXTI_Line idx = GPIO_EXTI_Line_10; idx<=GPIO_EXTI_Line_15; idx++)
      {
        if(gpio_exti_desc[exti_line_idx].irq_pin!=hwGPIO_Int_Pin_NC)
        {
          irq_10_15_used = true;
        }
      }
      if(gpio_exti_enable_status[GPIO_EXTI_IRQn_10_15]==false && irq_10_15_used==false)
      {
        gpio_exti_enable_status[GPIO_EXTI_IRQn_10_15] = false;
        HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
      }
    }

    HAL_EXTI_ClearConfigLine(&gpio_exti_desc[exti_line_idx].hexti);
    
    HAL_GPIO_DeInit(soc_base, soc_pin);
    
    gpio_pin_init_status[irq_pin] = false;

    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Config_Interrupt_Mode(hwGPIO_Int_Pin irq_pin, hwGPIO_Interrupt_Mode mode)
{
    if(irq_pin>=hwGPIO_Int_Pin_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
  
    if(mode>=hwGPIO_Interrupt_Mode_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
  
    uint32_t exti_port = GPIO_Int_Map_Exti_Port(irq_pin);
    uint32_t exti_line = GPIO_Int_Map_Exti_Line(irq_pin);
    GPIO_EXTI_Line exti_line_idx = GPIO_Map_EXTI_Line_Index(exti_line);

    if(exti_line_idx>=GPIO_EXTI_Line_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
  
    EXTI_ConfigTypeDef cfg = {
        .Line = exti_line,
        .Mode = EXTI_MODE_INTERRUPT,
        .Trigger = EXTI_TRIGGER_NONE,
        .GPIOSel = exti_port,
    };

    switch(mode)
    {
        case hwGPIO_Interrupt_Mode_Falling_Edge:
            cfg.Trigger = EXTI_TRIGGER_FALLING;
            break;
        case hwGPIO_Interrupt_Mode_Rising_Edge:
            cfg.Trigger = EXTI_TRIGGER_RISING;
            break;
        case hwGPIO_Interrupt_Mode_Both_Edge:
            cfg.Trigger = EXTI_TRIGGER_RISING_FALLING;
            break;
        default:
            return hwGPIO_InvalidParameter;
    }
    
    gpio_exti_desc[exti_line_idx].mode = mode;

    HAL_EXTI_SetConfigLine(&gpio_exti_desc[exti_line_idx].hexti, &cfg);
    
    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Register_Interrupt_Handler(hwGPIO_Int_Pin irq_pin, GPIO_Interrupt_Event_Handler handler)
{
    if(irq_pin>=hwGPIO_Int_Pin_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
  
    if(handler==NULL)
    {
        return hwGPIO_InvalidParameter;
    }
  
    gpio_irq_handlers[irq_pin] = handler;

    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Unregister_Interrupt_Handler(hwGPIO_Int_Pin irq_pin)
{
    if(irq_pin>=hwGPIO_Int_Pin_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
  
    gpio_irq_handlers[irq_pin] = NULL;

    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Interrupt_Enable(hwGPIO_Int_Pin irq_pin)
{
    if(irq_pin>=hwGPIO_Int_Pin_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
  
    uint32_t exti_port = GPIO_Int_Map_Exti_Port(irq_pin);
    uint32_t exti_line = GPIO_Int_Map_Exti_Line(irq_pin);
    GPIO_EXTI_Line exti_line_idx = GPIO_Map_EXTI_Line_Index(exti_line);

    if(exti_line_idx>=GPIO_EXTI_Line_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
  
    EXTI_ConfigTypeDef cfg = {
        .Line = exti_line,
        .Mode = EXTI_MODE_INTERRUPT,
        .Trigger = EXTI_TRIGGER_NONE,
        .GPIOSel = exti_port,
    };

    switch(gpio_current_mode[irq_pin])
    {
        case hwGPIO_Interrupt_Mode_Falling_Edge:
            cfg.Trigger = EXTI_TRIGGER_FALLING;
            break;
        case hwGPIO_Interrupt_Mode_Rising_Edge:
            cfg.Trigger = EXTI_TRIGGER_RISING;
            break;
        case hwGPIO_Interrupt_Mode_Both_Edge:
            cfg.Trigger = EXTI_TRIGGER_RISING_FALLING;
            break;
        default:
            return hwGPIO_InvalidParameter;
    }
    
    HAL_EXTI_SetConfigLine(&gpio_exti_desc[exti_line_idx].hexti, &cfg);
    
    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Interrupt_Disable(hwGPIO_Int_Pin irq_pin)
{
    if(irq_pin>=hwGPIO_Int_Pin_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
  
    uint32_t exti_port = GPIO_Int_Map_Exti_Port(irq_pin);
    uint32_t exti_line = GPIO_Int_Map_Exti_Line(irq_pin);
    GPIO_EXTI_Line exti_line_idx = GPIO_Map_EXTI_Line_Index(exti_line);

    if(exti_line_idx>=GPIO_EXTI_Line_MAX)
    {
        return hwGPIO_InvalidParameter;
    }
  
    EXTI_ConfigTypeDef cfg = {
        .Line = exti_line,
        .Mode = EXTI_MODE_INTERRUPT,
        .Trigger = EXTI_TRIGGER_NONE,
        .GPIOSel = exti_port,
    };

    HAL_EXTI_SetConfigLine(&gpio_exti_desc[exti_line_idx].hexti, &cfg);
    
    return hwGPIO_OK;
}

hwGPIO_OpStatus GPIO_Interrupt_Pin_Read(hwGPIO_Int_Pin pin, bool* level)
{
    if(pin>=hwGPIO_Int_Pin_MAX)
    {
      return hwGPIO_InvalidParameter;
    }
    
    if(level==NULL)
    {
      return hwGPIO_InvalidParameter;
    }
  
    uint16_t soc_pin = GPIO_Map_Soc_Pin(pin);
    GPIO_TypeDef * soc_base = GPIO_Map_Soc_Base(pin);

    if(soc_pin==0 || soc_base==NULL)
    {
      return hwGPIO_InvalidParameter;
    }
    
    *level = (HAL_GPIO_ReadPin(soc_base, soc_pin)==GPIO_PIN_SET) ? 1: 0;
    
    return hwGPIO_OK;
}

#endif //STM32