#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "soc.h"

#include "NeonRTOS.h"

#include "CAN.h"

#ifdef STM32

#include "CAN_Pin.h"

#include "GPIO/GPIO_STM32.h"

#define CAN_IRQ_NVIC_PRIORITY 5
#define CAN_IRQ_NVIC_SUB_PRIORITY 0

static bool CAN_Init_Status[hwCAN_Index_MAX];
static NeonRTOS_SyncObj_t CAN_TxDone_Sync[hwCAN_Index_MAX];
static NeonRTOS_MsgQ_t   CAN_RxQueue[hwCAN_Index_MAX];

static CAN_HandleTypeDef g_can[hwCAN_Index_MAX];

uint32_t STM32_CAN_GetAF(hwCAN_Index CAN, hwGPIO_Pin pin)
{
    for (size_t i = 0; i < sizeof(CAN_Pin_AF_Map)/sizeof(CAN_Pin_AF_Map[0]); i++) {
        if (CAN_Pin_AF_Map[i].can == CAN &&
            CAN_Pin_AF_Map[i].pin  == pin)
            return CAN_Pin_AF_Map[i].af;
    }
    return 0;
}

static hwCAN_Index CAN_IndexFromHandle(CAN_HandleTypeDef *hCAN)
{
    for(int i=0;i<hwCAN_Index_MAX;i++){
        if(&g_can[i] == hCAN) return (hwCAN_Index)i;
    }
    return hwCAN_Index_MAX;
}

CAN_TypeDef * CAN_Map_Soc_Base(hwCAN_Index index)
{
    switch(index)
    {
        case hwCAN_Index_0: return CAN1;
        case hwCAN_Index_1: return CAN2;
#if defined (CAN3_BASE)
        case hwCAN_Index_2: return CAN3;
#endif
        default: break;
    }
    return NULL;
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
    for (int i = 0; i < hwCAN_Index_MAX; i++) {
        if (&g_can[i] == hcan) {
            NeonRTOS_SyncObjSignalFromISR(&CAN_TxDone_Sync[i]);
            break;
        }
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef hdr;
    uint8_t data[8];

    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hdr, data);

    for (int i = 0; i < hwCAN_Index_MAX; i++) {
        if (&g_can[i] == hcan) {
            NeonRTOS_MsgQWrite(&CAN_RxQueue[i], data, NEONRT_NO_WAIT);
            break;
        }
    }
}

static void CAN_HAL_IRQHandler(hwCAN_Index index)
{
    HAL_CAN_IRQHandler(&g_can[index]);
}

void CAN1_RX0_IRQHandler(void){ CAN_HAL_IRQHandler(hwCAN_Index_0); }
void CAN1_TX_IRQHandler(void){  CAN_HAL_IRQHandler(hwCAN_Index_0); }
void CAN1_SCE_IRQHandler(void){ CAN_HAL_IRQHandler(hwCAN_Index_0); }

void CAN2_RX0_IRQHandler(void){ CAN_HAL_IRQHandler(hwCAN_Index_1); }
void CAN2_TX_IRQHandler(void){  CAN_HAL_IRQHandler(hwCAN_Index_1); }
void CAN2_SCE_IRQHandler(void){ CAN_HAL_IRQHandler(hwCAN_Index_1); }

hwCAN_OpResult CAN_Init(hwCAN_Index index)
{
    if (index >= hwCAN_Index_MAX)
    {
        return hwCAN_InvalidParameter;
    }

    if (CAN_Init_Status[index])
    {
        return hwCAN_OK;
    }

    GPIO_TypeDef *tx_soc_base = GPIO_Map_Soc_Base(CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].tx_pin);
    GPIO_TypeDef *rx_soc_base = GPIO_Map_Soc_Base(CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].rx_pin);
    uint16_t tx_soc_pin = GPIO_Map_Soc_Pin(CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].tx_pin);
    uint16_t rx_soc_pin = GPIO_Map_Soc_Pin(CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].rx_pin);

    if(tx_soc_pin==0 || tx_soc_base==NULL || rx_soc_pin==0 || rx_soc_base==NULL)
    {
            return hwCAN_InvalidParameter;
    }

    uint32_t tx_af = STM32_CAN_GetAF(index, CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].tx_pin);
    uint32_t rx_af = STM32_CAN_GetAF(index, CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].rx_pin);

    if(tx_af==0 || rx_af==0)
    {
            return hwCAN_InvalidParameter;
    }

    CAN_TypeDef * can_soc_base = CAN_Map_Soc_Base(index);
    if(can_soc_base==NULL)
    {
            return hwCAN_InvalidParameter;
    }

    if (NeonRTOS_SyncObjCreate(&CAN_TxDone_Sync[index]) != NeonRTOS_OK)
    {
        return hwCAN_MemoryError;
    }

    if (NeonRTOS_MsgQCreate(&CAN_RxQueue[index], "can_rx", 8, 8) != NeonRTOS_OK)
    {
        NeonRTOS_SyncObjDelete(&CAN_TxDone_Sync[index]);
        return hwCAN_MemoryError;
    }

    GPIO_Enable_RCC_Clock(tx_soc_base);
    GPIO_Enable_RCC_Clock(rx_soc_base);

    GPIO_InitTypeDef g_can_tx = {0};
    g_can_tx.Pin       = tx_soc_pin;
    g_can_tx.Mode      = GPIO_MODE_AF_PP;
    g_can_tx.Pull      = GPIO_NOPULL;
    g_can_tx.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    g_can_tx.Alternate = tx_af;
    HAL_GPIO_Init(tx_soc_base, &g_can_tx);

    GPIO_InitTypeDef g_can_rx = {0};
    g_can_rx.Pin       = rx_soc_pin;
    g_can_rx.Mode      = GPIO_MODE_AF_PP;
    g_can_rx.Pull      = GPIO_NOPULL;
    g_can_rx.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    g_can_rx.Alternate = rx_af;
    HAL_GPIO_Init(rx_soc_base, &g_can_rx);

    /* CAN */
    switch(index)
    {
        case hwCAN_Index_0:
            __HAL_RCC_CAN1_CLK_ENABLE();
            break;
        case hwCAN_Index_1:
            __HAL_RCC_CAN2_CLK_ENABLE();
            break;
#if defined (CAN3_BASE)
        case hwCAN_Index_2:
            __HAL_RCC_CAN3_CLK_ENABLE();
            break;
#endif
    }

    g_can[index].Instance = can_soc_base;

    g_can[index].Init.Mode = CAN_MODE_NORMAL;
    g_can[index].Init.AutoBusOff = ENABLE;
    g_can[index].Init.AutoRetransmission = ENABLE;

    if(HAL_CAN_Init(&g_can[index])!=HAL_OK)
    {
        NeonRTOS_SyncObjDelete(&CAN_TxDone_Sync[index]);
        NeonRTOS_MsgQDelete(&CAN_RxQueue[index]);

        return hwCAN_HwError;
    }

    if(HAL_CAN_Start(&g_can[index])!=HAL_OK)
    {
        NeonRTOS_SyncObjDelete(&CAN_TxDone_Sync[index]);
        NeonRTOS_MsgQDelete(&CAN_RxQueue[index]);

        return hwCAN_HwError;
    }

    if(HAL_CAN_ActivateNotification(&g_can[index], CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)
    {
        NeonRTOS_SyncObjDelete(&CAN_TxDone_Sync[index]);
        NeonRTOS_MsgQDelete(&CAN_RxQueue[index]);

        return hwCAN_HwError;
    }

    switch(index)
    {       
        case hwCAN_Index_0:
            HAL_NVIC_SetPriority(CAN1_TX_IRQn, CAN_IRQ_NVIC_PRIORITY, CAN_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
            HAL_NVIC_SetPriority(CAN1_RX0_IRQn, CAN_IRQ_NVIC_PRIORITY, CAN_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
            HAL_NVIC_SetPriority(CAN1_RX1_IRQn, CAN_IRQ_NVIC_PRIORITY, CAN_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
            HAL_NVIC_SetPriority(CAN1_SCE_IRQn, CAN_IRQ_NVIC_PRIORITY, CAN_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
            break;
        case hwCAN_Index_1:
            HAL_NVIC_SetPriority(CAN2_TX_IRQn, CAN_IRQ_NVIC_PRIORITY, CAN_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(CAN2_TX_IRQn);
            HAL_NVIC_SetPriority(CAN2_RX0_IRQn, CAN_IRQ_NVIC_PRIORITY, CAN_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
            HAL_NVIC_SetPriority(CAN2_RX1_IRQn, CAN_IRQ_NVIC_PRIORITY, CAN_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(CAN2_RX1_IRQn);
            HAL_NVIC_SetPriority(CAN2_SCE_IRQn, CAN_IRQ_NVIC_PRIORITY, CAN_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(CAN2_SCE_IRQn);
            break;
#if defined (CAN3_BASE)
        case hwCAN_Index_2:
            HAL_NVIC_SetPriority(CAN3_TX_IRQn, CAN_IRQ_NVIC_PRIORITY, CAN_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(CAN3_TX_IRQn);
            HAL_NVIC_SetPriority(CAN3_RX0_IRQn, CAN_IRQ_NVIC_PRIORITY, CAN_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(CAN3_RX0_IRQn);
            HAL_NVIC_SetPriority(CAN3_RX1_IRQn, CAN_IRQ_NVIC_PRIORITY, CAN_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(CAN3_RX1_IRQn);
            HAL_NVIC_SetPriority(CAN3_SCE_IRQn, CAN_IRQ_NVIC_PRIORITY, CAN_IRQ_NVIC_SUB_PRIORITY);
            HAL_NVIC_EnableIRQ(CAN3_SCE_IRQn);
            break;
#endif
    }

    gpio_pin_init_status[CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].tx_pin] = true;
    gpio_pin_init_status[CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].rx_pin] = true;

    CAN_Init_Status[index] = true;

    return hwCAN_OK;
}

hwCAN_OpResult CAN_DeInit(hwCAN_Index index)
{
    if(index>=hwCAN_Index_MAX)
    {
        return hwCAN_InvalidParameter;
    }

    if(CAN_Init_Status[index]==false)
    {
        return hwCAN_OK;
    }
    
    GPIO_TypeDef *tx_soc_base = GPIO_Map_Soc_Base(CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].tx_pin);
    GPIO_TypeDef *rx_soc_base = GPIO_Map_Soc_Base(CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].rx_pin);
    uint16_t tx_soc_pin = GPIO_Map_Soc_Pin(CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].tx_pin);
    uint16_t rx_soc_pin = GPIO_Map_Soc_Pin(CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].rx_pin);

    if(tx_soc_pin==0 || tx_soc_base==NULL || rx_soc_pin==0 || rx_soc_base==NULL)
    {
            return hwCAN_InvalidParameter;
    }

    uint32_t tx_af = STM32_CAN_GetAF(index, CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].tx_pin);
    uint32_t rx_af = STM32_CAN_GetAF(index, CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].rx_pin);

    if(tx_af==0 || rx_af==0)
    {
            return hwCAN_InvalidParameter;
    }

    CAN_TypeDef * can_soc_base = CAN_Map_Soc_Base(index);
    if(can_soc_base==NULL)
    {
            return hwCAN_InvalidParameter;
    }

    switch(index)
    {       
        case hwCAN_Index_0:
            HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
            HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
            HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
            HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
            break;
        case hwCAN_Index_1:
            HAL_NVIC_DisableIRQ(CAN2_TX_IRQn);
            HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
            HAL_NVIC_DisableIRQ(CAN2_RX1_IRQn);
            HAL_NVIC_DisableIRQ(CAN2_SCE_IRQn);
            break;
#if defined (CAN3_BASE)
        case hwCAN_Index_2:
            HAL_NVIC_DisableIRQ(CAN3_TX_IRQn);
            HAL_NVIC_DisableIRQ(CAN3_RX0_IRQn);
            HAL_NVIC_DisableIRQ(CAN3_RX1_IRQn);
            HAL_NVIC_DisableIRQ(CAN3_SCE_IRQn);
            break;
#endif
    }

    HAL_CAN_DeInit(&g_can[index]);

    switch(index)
    {
        case hwCAN_Index_0:
            __HAL_RCC_CAN1_CLK_DISABLE();
            break;
        case hwCAN_Index_1:
            __HAL_RCC_CAN2_CLK_DISABLE();
            break;
#if defined (CAN3_BASE)
        case hwCAN_Index_2:
            __HAL_RCC_CAN3_CLK_DISABLE();
            break;
#endif
    }

    HAL_GPIO_DeInit(tx_soc_base, tx_soc_pin);
    HAL_GPIO_DeInit(rx_soc_base, rx_soc_pin);

    NeonRTOS_SyncObjDelete(&CAN_TxDone_Sync[index]);
    NeonRTOS_MsgQDelete(&CAN_RxQueue[index]);

    gpio_pin_init_status[CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].tx_pin] = false;
    gpio_pin_init_status[CAN_Pin_Def_Table[index][CAN_Index_Map_Alt[index]].rx_pin] = false;

    CAN_Init_Status[index] = false;

    return hwCAN_OK;
}

hwCAN_OpResult CAN_Read(hwCAN_Index index, uint8_t *buf, uint32_t timeout)
{
    if (NeonRTOS_QueueReceive(&CAN_RxQueue[index], buf, timeout) != NeonRTOS_OK)
        return hwCAN_Timeout;

    return hwCAN_OK;
}

hwCAN_OpResult CAN_Write(hwCAN_Index index, uint32_t id, uint8_t *data, uint8_t len, uint32_t timeout)
{
    CAN_TxHeaderTypeDef hdr = {
        .StdId = id,
        .IDE = CAN_ID_STD,
        .RTR = CAN_RTR_DATA,
        .DLC = len
    };

    uint32_t mailbox;
    HAL_CAN_AddTxMessage(&g_can[index], &hdr, data, &mailbox);

    if (NeonRTOS_SyncObjWait(&CAN_TxDone_Sync[index], timeout) != NeonRTOS_OK)
        return hwCAN_Timeout;

    return hwCAN_OK;
}

bool CAN_isInit(hwCAN_Index index)
{
    if(index>=hwCAN_Index_MAX)
    {
        return false;
    }
    return CAN_Init_Status[index];
}

#endif //STM32