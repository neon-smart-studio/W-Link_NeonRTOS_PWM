/**
  ******************************************************************************
  * @file    ethernetif.c
  * @author  MCD Application Team & Wi6Labs
  * @version V1.5.0
  * @date    20-june-2017
  * @brief   This file implements Ethernet network interface drivers for lwIP
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "soc.h"

#include "lwip/timeouts.h"
#include "lwip/etharp.h"
#include "ethernetif.h"
#include <string.h>
#include "lwip/igmp.h"

#include "NeonRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STM32

#include "GPIO/GPIO_STM32.h"

/* Interrupt priority */
#ifndef ETH_TIM_IRQ_PRIO
  #define ETH_TIM_IRQ_PRIO       15 // Warning: it should be lower prio (higher value) than Systick
#endif
#ifndef ETH_TIM_IRQ_SUBPRIO
  #define ETH_TIM_IRQ_SUBPRIO    0
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Network interface name */
#define IFNAME0 'e'
#define IFNAME1 '0'

/* Definition of PHY SPECIAL CONTROL/STATUS REGISTER bitfield Auto-negotiation done indication */
/* Placed in STM32Ethernet library instead of HAL conf to avoid compatibility dependence with Arduino_Core_STM32 */
/* Could be moved from this file once Generic PHY is implemented */
#define PHY_SR_AUTODONE ((uint16_t)0x1000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;/* Ethernet Rx MA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;/* Ethernet Tx DMA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_END; /* Ethernet Receive Buffer */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
__ALIGN_BEGIN uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_END; /* Ethernet Transmit Buffer */

static ETH_HandleTypeDef EthHandle;

static uint8_t macaddress[6] = { MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5 };

#if LWIP_IGMP
uint32_t ETH_HashTableHigh = 0x0;
uint32_t ETH_HashTableLow = 0x0;
#endif

#ifdef ETH_INPUT_USE_IT
#define HAL_ETH_TASK_STACK_SIZE  2048
#define HAL_ETH_TASK_PRIORITY    tskIDLE_PRIORITY + 2
static NeonRTOS_SyncObj_t hal_eth_sync;
#endif //ETH_INPUT_USE_IT

//#define PKT_TX_DUMP
//#define PKT_RX_DUMP
//#define PKT_DUMP_RAW

struct netif * pNetif = NULL;

#if defined(PKT_RX_DUMP) ||  defined(PKT_TX_DUMP)
static void packet_dump(const char * msg, const struct pbuf* p)
{
#ifdef PKT_DUMP_RAW    
    const struct pbuf* q;
    uint32_t i,j;
    uint8_t *ptr;

    UART_Printf("%s %d byte\n", msg, p->tot_len);

    i=0;
    for(q=p; q != NULL; q= q->next)
    {
        ptr = q->payload;

        for(j=0; j<q->len; j++)
        {
            if( (i%8) == 0 )
            {
                UART_Printf("  ");
            }
            if( (i%16) == 0 )
            {
                UART_Printf("\r\n");
            }
            UART_Printf("%02X ", *ptr);

            i++;
            ptr++;
        }
    }

    UART_Printf("\n\n");
#else /* DM9051_DUMP_RAW */
    uint8_t header[6 + 6 + 2];
    uint16_t type;

    pbuf_copy_partial(p, header, sizeof(header), 0);
    type = (header[12] << 8) | header[13];

    UART_Printf("%02X:%02X:%02X:%02X:%02X:%02X <== %02X:%02X:%02X:%02X:%02X:%02X ",
               header[0], header[1], header[2], header[3], header[4], header[5],
               header[6], header[7], header[8], header[9], header[10], header[11]);

    switch (type)
    {
    case 0x0800:
        UART_Printf("IPv4. ");
        break;

    case 0x0806:
        UART_Printf("ARP.  ");
        break;

    case 0x86DD:
        UART_Printf("IPv6. ");
        break;

    default:
        UART_Printf("%04X. ", type);
        break;
    }

    UART_Printf("%s %d byte. \n", msg, p->tot_len);
#endif /* DM9051_DUMP_RAW */
}
#else
#define packet_dump(...)
#endif /* dump */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
                       Ethernet MSP Routines
*******************************************************************************/
/**
  * @brief  Initializes the ETH MSP.
  * @param  heth: ETH handle
  * @retval None
  */
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    UNUSED(heth);

    /* ---------- Enable clocks ---------- */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /* ---------- GPIOA ---------- */
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    gpio_pin_init_status[hwGPIO_Pin_A1] = true;
    gpio_pin_init_status[hwGPIO_Pin_A2] = true;
    gpio_pin_init_status[hwGPIO_Pin_A7] = true;

    /* ---------- GPIOB ---------- */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    gpio_pin_init_status[hwGPIO_Pin_B13] = true;

    /* ---------- GPIOC ---------- */
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    gpio_pin_init_status[hwGPIO_Pin_C1] = true;
    gpio_pin_init_status[hwGPIO_Pin_C4] = true;
    gpio_pin_init_status[hwGPIO_Pin_C5] = true;

    /* ---------- GPIOG ---------- */
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_13;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    gpio_pin_init_status[hwGPIO_Pin_G11] = true;
    gpio_pin_init_status[hwGPIO_Pin_G13] = true;

#ifdef ETH_INPUT_USE_IT
    /* ---------- NVIC ---------- */
    HAL_NVIC_SetPriority(ETH_IRQn, ETH_TIM_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(ETH_IRQn);
#endif

    __HAL_RCC_ETH_CLK_ENABLE();
}

/*******************************************************************************
                       LL Driver Interface ( LwIP stack --> ETH)
*******************************************************************************/
/**
  * @brief In this function, the hardware should be initialized.
  * Called from ethernetif_init().
  *
  * @param netif the already initialized lwip network interface structure
  *        for this ethernetif
  */
static void low_level_init(struct netif *netif)
{
  uint32_t regvalue;

  EthHandle.Instance = ETH;
  EthHandle.Init.MACAddr = macaddress;
  EthHandle.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
  EthHandle.Init.Speed = ETH_SPEED_100M;
  EthHandle.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
#ifdef ETHERNET_RMII_MODE_CONFIGURATION
  EthHandle.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
#else
  EthHandle.Init.MediaInterface = ETH_MEDIA_INTERFACE_MII;
#endif /* ETHERNET_RMII_MODE_CONFIGURATION */
#ifdef ETH_INPUT_USE_IT
  EthHandle.Init.RxMode = ETH_RXINTERRUPT_MODE;
#else
  EthHandle.Init.RxMode = ETH_RXPOLLING_MODE;
#endif /* ETH_INPUT_USE_IT */
  EthHandle.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
  EthHandle.Init.PhyAddress = LAN8742A_PHY_ADDRESS;

  /* configure ethernet peripheral (GPIOs, clocks, MAC, DMA) */
  if (HAL_ETH_Init(&EthHandle) == HAL_OK) {
    /* Set netif link flag */
    netif->flags |= NETIF_FLAG_LINK_UP;
  }

  /* Initialize Tx Descriptors list: Chain Mode */
  HAL_ETH_DMATxDescListInit(&EthHandle, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);

  /* Initialize Rx Descriptors list: Chain Mode  */
  HAL_ETH_DMARxDescListInit(&EthHandle, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

  /* set MAC hardware address length */
  netif->hwaddr_len = ETH_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] =  macaddress[0];
  netif->hwaddr[1] =  macaddress[1];
  netif->hwaddr[2] =  macaddress[2];
  netif->hwaddr[3] =  macaddress[3];
  netif->hwaddr[4] =  macaddress[4];
  netif->hwaddr[5] =  macaddress[5];

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

  /* Enable MAC and DMA transmission and reception */
  HAL_ETH_Start(&EthHandle);
#if LWIP_IGMP
  netif_set_igmp_mac_filter(netif, igmp_mac_filter);
#endif
  /**** Configure PHY to generate an interrupt when Eth Link state changes ****/
  /* Read Register Configuration */
  HAL_ETH_ReadPHYRegister(&EthHandle, PHY_IMR, &regvalue);

  regvalue |= PHY_ISFR_INT4;

  /* Enable Interrupt on change of link status */
  HAL_ETH_WritePHYRegister(&EthHandle, PHY_IMR, regvalue);
#if LWIP_IGMP
  ETH_HashTableHigh = EthHandle.Instance->MACHTHR;
  ETH_HashTableLow = EthHandle.Instance->MACHTLR;
#endif
}

/**
  * @brief This function should do the actual transmission of the packet. The packet is
  * contained in the pbuf that is passed to the function. This pbuf
  * might be chained.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
  * @return ERR_OK if the packet could be sent
  *         an err_t value if the packet couldn't be sent
  *
  * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
  *       strange results. You might consider waiting for space in the DMA queue
  *       to become available since the stack doesn't retry to send a packet
  *       dropped because of memory failure (except for the TCP timers).
  */
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  err_t errval;
  struct pbuf *q;
  uint8_t *buffer = (uint8_t *)(EthHandle.TxDesc->Buffer1Addr);
  __IO ETH_DMADescTypeDef *DmaTxDesc;
  uint32_t framelength = 0;
  uint32_t bufferoffset = 0;
  uint32_t byteslefttocopy = 0;
  uint32_t payloadoffset = 0;

  UNUSED(netif);

#ifdef PKT_TX_DUMP
    packet_dump(__FUNCTION__, p);
#endif /* PKT_TX_DUMP */

  DmaTxDesc = EthHandle.TxDesc;
  bufferoffset = 0;

  /* copy frame from pbufs to driver buffers */
  for (q = p; q != NULL; q = q->next) {
    /* Is this buffer available? If not, goto error */
    if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET) {
      errval = ERR_USE;
      goto error;
    }

    /* Get bytes in current lwIP buffer */
    byteslefttocopy = q->len;
    payloadoffset = 0;

    /* Check if the length of data to copy is bigger than Tx buffer size*/
    while ((byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE) {
      /* Copy data to Tx buffer*/
      memcpy((uint8_t *)((uint8_t *)buffer + bufferoffset), (uint8_t *)((uint8_t *)q->payload + payloadoffset), (ETH_TX_BUF_SIZE - bufferoffset));

      /* Point to next descriptor */
      DmaTxDesc = (ETH_DMADescTypeDef *)(DmaTxDesc->Buffer2NextDescAddr);

      /* Check if the buffer is available */
      if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET) {
        errval = ERR_USE;
        goto error;
      }

      buffer = (uint8_t *)(DmaTxDesc->Buffer1Addr);

      byteslefttocopy = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
      payloadoffset = payloadoffset + (ETH_TX_BUF_SIZE - bufferoffset);
      framelength = framelength + (ETH_TX_BUF_SIZE - bufferoffset);
      bufferoffset = 0;
    }

    /* Copy the remaining bytes */
    memcpy((uint8_t *)((uint8_t *)buffer + bufferoffset), (uint8_t *)((uint8_t *)q->payload + payloadoffset), byteslefttocopy);
    bufferoffset = bufferoffset + byteslefttocopy;
    framelength = framelength + byteslefttocopy;
  }

  /* Prepare transmit descriptors to give to DMA */
  HAL_ETH_TransmitFrame(&EthHandle, framelength);

  errval = ERR_OK;

error:

  /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
  if ((EthHandle.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET) {
    /* Clear TUS ETHERNET DMA flag */
    EthHandle.Instance->DMASR = ETH_DMASR_TUS;

    /* Resume DMA transmission*/
    EthHandle.Instance->DMATPDR = 0;
  }
  return errval;
}

/**
  * @brief Should allocate a pbuf and transfer the bytes of the incoming
  * packet from the interface into the pbuf.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @return a pbuf filled with the received packet (including MAC header)
  *         NULL on memory error
  */
static struct pbuf *low_level_input(struct netif *netif)
{
  struct pbuf *p = NULL;
  struct pbuf *q;
  uint16_t len;
  uint8_t *buffer;
  __IO ETH_DMADescTypeDef *dmarxdesc;
  uint32_t bufferoffset = 0;
  uint32_t payloadoffset = 0;
  uint32_t byteslefttocopy = 0;
  uint32_t i = 0;

  UNUSED(netif);

  if (HAL_ETH_GetReceivedFrame_IT(&EthHandle) != HAL_OK) {
    return NULL;
  }

  /* Obtain the size of the packet and put it into the "len" variable. */
  len = EthHandle.RxFrameInfos.length;
  buffer = (uint8_t *)EthHandle.RxFrameInfos.buffer;

  if (len > 0) {
    /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
  }

  if (p != NULL) {
    dmarxdesc = EthHandle.RxFrameInfos.FSRxDesc;
    bufferoffset = 0;

    for (q = p; q != NULL; q = q->next) {
      byteslefttocopy = q->len;
      payloadoffset = 0;

      /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size */
      while ((byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE) {
        /* Copy data to pbuf */
        memcpy((uint8_t *)((uint8_t *)q->payload + payloadoffset), (uint8_t *)((uint8_t *)buffer + bufferoffset), (ETH_RX_BUF_SIZE - bufferoffset));

        /* Point to next descriptor */
        dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
        buffer = (uint8_t *)(dmarxdesc->Buffer1Addr);

        byteslefttocopy = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
        payloadoffset = payloadoffset + (ETH_RX_BUF_SIZE - bufferoffset);
        bufferoffset = 0;
      }

      /* Copy remaining data in pbuf */
      memcpy((uint8_t *)((uint8_t *)q->payload + payloadoffset), (uint8_t *)((uint8_t *)buffer + bufferoffset), byteslefttocopy);
      bufferoffset = bufferoffset + byteslefttocopy;
    }
  }

  /* Release descriptors to DMA */
  /* Point to first descriptor */
  dmarxdesc = EthHandle.RxFrameInfos.FSRxDesc;
  /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
  for (i = 0; i < EthHandle.RxFrameInfos.SegCount; i++) {
    dmarxdesc->Status |= ETH_DMARXDESC_OWN;
    dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
  }

  /* Clear Segment_Count */
  EthHandle.RxFrameInfos.SegCount = 0;

  /* When Rx Buffer unavailable flag is set: clear it and resume reception */
  if ((EthHandle.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET) {
    /* Clear RBUS ETHERNET DMA flag */
    EthHandle.Instance->DMASR = ETH_DMASR_RBUS;
    /* Resume DMA reception */
    EthHandle.Instance->DMARPDR = 0;
  }
  
#ifdef PKT_RX_DUMP
    packet_dump(__FUNCTION__, p);
#endif /* PKT_RX_DUMP */

  return p;
}
/**
  * @brief This function should be called when a packet is ready to be read
  * from the interface. It uses the function low_level_input() that
  * should handle the actual reception of bytes from the network
  * interface. Then the type of the received packet is determined and
  * the appropriate input function is called.
  *
  * @param netif the lwip network interface structure for this ethernetif
  */
void ethernetif_input(struct netif *netif)
{
  err_t err;
  struct pbuf *p;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);

  /* no packet could be read, silently ignore this */
  if (p == NULL) {
    return;
  }

  /* entry point to the LwIP stack */
  err = netif->input(p, netif);

  if (err != ERR_OK) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
    pbuf_free(p);
    p = NULL;
  }
}

/**
  * @brief Returns the current state
  *
  * @param None
  * @return 0 not initialized else 1
  */
uint8_t ethernetif_is_init(void)
{
  return (EthHandle.State != HAL_ETH_STATE_RESET);
}

#ifdef ETH_INPUT_USE_IT
void HAL_ETH_Task(void *pValue) {
    while (1) {
        if(NeonRTOS_SyncObjWait(&hal_eth_sync, NEONRT_WAIT_FOREVER)==NeonRTOS_OK)
        {
            low_level_input(pNetif);
        }
    }
}
#endif

/**
  * @brief Should be called at the beginning of the program to set up the
  * network interface. It calls the function low_level_init() to do the
  * actual setup of the hardware.
  *
  * This function should be passed as a parameter to netif_add().
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @return ERR_OK if the loopif is initialized
  *         ERR_MEM if private data couldn't be allocated
  *         any other err_t on error
  */
err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

  pNetif = netif;

#ifdef ETH_INPUT_USE_IT
    NeonRTOS_ReturnVal_e ret;

    ret = NeonRTOS_SyncObjCreate(&hal_eth_sync);
    if (ret != NeonRTOS_OK) {
        printf("Failed to create hal_eth_sync\n");
        return ERR_MEM;
    }
    NeonRTOS_SyncObjClear(&hal_eth_sync);

    // 創建 NeonRTOS 網絡任務
    ret = NeonRTOS_TaskCreate(
        HAL_ETH_Task,                 // 任務入口函數
        "HAL_ETH_Task",               // 任務名稱
        HAL_ETH_TASK_STACK_SIZE,     // 堆疊大小
        NULL,                        // 任務參數
        HAL_ETH_TASK_PRIORITY,       // 任務優先級
        NULL                            // 任務句柄
    );

    if (ret != NeonRTOS_OK) {
        NeonRTOS_SyncObjDelete(&hal_eth_sync);
        printf("Failed to create HAL_ETH_Task\n");
        return ERR_MEM;
    }
#endif

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "NeonRT";
#endif /* LWIP_NETIF_HOSTNAME */

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

/**
  * @brief  Returns the current time in milliseconds
  *         when LWIP_TIMERS == 1 and NO_SYS == 1
  * @param  None
  * @retval Current Time value
  */
u32_t sys_now(void)
{
  return HAL_GetTick();
}

/**
  * @brief  This function sets the netif link status.
  * @param  netif: the network interface
  * @retval None
  */
void ethernetif_set_link(struct netif *netif)
{
  uint32_t regvalue = 0;

  /* Read PHY_MISR*/
  HAL_ETH_ReadPHYRegister(&EthHandle, PHY_ISFR, &regvalue);

  /* Check whether the link interrupt has occurred or not */
  if ((regvalue & PHY_ISFR_INT4) != (uint16_t)RESET) {
    netif_set_link_down(netif);
  }

  HAL_ETH_ReadPHYRegister(&EthHandle, PHY_BSR, &regvalue);

  if ((regvalue & PHY_LINKED_STATUS) != (uint16_t)RESET) {
#if LWIP_IGMP
    if (!(netif->flags & NETIF_FLAG_IGMP)) {
      netif->flags |= NETIF_FLAG_IGMP;
      igmp_init();
      igmp_start(netif);
    }
#endif
    netif_set_link_up(netif);
  }
}

/**
  * @brief  Link callback function, this function is called on change of link status
  *         to update low level driver configuration.
  * @param  netif: The network interface
  * @retval None
  */
void ethernetif_update_config(struct netif *netif)
{
  uint32_t regvalue = 0;

  if (netif_is_link_up(netif)) {
    /* Restart the auto-negotiation */
    if (EthHandle.Init.AutoNegotiation != ETH_AUTONEGOTIATION_DISABLE) {

      /* Check Auto negotiation */
      HAL_ETH_ReadPHYRegister(&EthHandle, PHY_SR, &regvalue);
      if ((regvalue & PHY_SR_AUTODONE) != PHY_SR_AUTODONE) {
        goto error;
      }

      /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
      if ((regvalue & PHY_DUPLEX_STATUS) != (uint32_t)RESET) {
        /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
        EthHandle.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
      } else {
        /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
        EthHandle.Init.DuplexMode = ETH_MODE_HALFDUPLEX;
      }
      /* Configure the MAC with the speed fixed by the auto-negotiation process */
      if (regvalue & PHY_SPEED_STATUS) {
        /* Set Ethernet speed to 10M following the auto-negotiation */
        EthHandle.Init.Speed = ETH_SPEED_10M;
      } else {
        /* Set Ethernet speed to 100M following the auto-negotiation */
        EthHandle.Init.Speed = ETH_SPEED_100M;
      }
    } else { /* AutoNegotiation Disable */
error :
      /* Check parameters */
      assert_param(IS_ETH_SPEED(EthHandle.Init.Speed));
      assert_param(IS_ETH_DUPLEX_MODE(EthHandle.Init.DuplexMode));

      /* Set MAC Speed and Duplex Mode to PHY */
      HAL_ETH_WritePHYRegister(&EthHandle, PHY_BCR, ((uint16_t)(EthHandle.Init.DuplexMode >> 3) |
                                                     (uint16_t)(EthHandle.Init.Speed >> 1)));
    }

    /* ETHERNET MAC Re-Configuration */
    HAL_ETH_ConfigMAC(&EthHandle, (ETH_MACInitTypeDef *) NULL);

    /* Restart MAC interface */
    HAL_ETH_Start(&EthHandle);
  } else {
    /* Stop MAC interface */
    HAL_ETH_Stop(&EthHandle);
  }

  ethernetif_notify_conn_changed(netif);
}

/**
  * @brief  This function notify user about link status changement.
  * @param  netif: the network interface
  * @retval None
  */
__weak void ethernetif_notify_conn_changed(struct netif *netif)
{
  /* NOTE : This is function clould be implemented in user file
            when the callback is needed,
  */
  UNUSED(netif);
}

/**
  * @brief  This function set a custom MAC address. This function must be called
  *         before ethernetif_init().
  * @param  mac: mac address
  * @retval None
  */
void ethernetif_set_mac_addr(const uint8_t *mac)
{
  if (mac != NULL) {
    memcpy(macaddress, mac, 6);
  }
}

#if LWIP_IGMP
err_t igmp_mac_filter(struct netif *netif, const ip4_addr_t *ip4_addr, netif_mac_filter_action action)
{
  uint8_t mac[6];
  const uint8_t *p = (const uint8_t *)ip4_addr;

  mac[0] = 0x01;
  mac[1] = 0x00;
  mac[2] = 0x5E;
  mac[3] = *(p + 1) & 0x7F;
  mac[4] = *(p + 2);
  mac[5] = *(p + 3);

  register_multicast_address(mac);

  return 0;
}

#ifndef HASH_BITS
#define HASH_BITS 6 /* #bits in hash */
#endif

uint32_t ethcrc(const uint8_t *data, size_t length)
{
  uint32_t crc = 0xffffffff;
  size_t i;
  int j;

  for (i = 0; i < length; i++) {
    for (j = 0; j < 8; j++) {
      if (((crc >> 31) ^ (data[i] >> j)) & 0x01) {
        /* x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1 */
        crc = (crc << 1) ^ 0x04C11DB7;
      } else {
        crc = crc << 1;
      }
    }
  }
  return ~crc;
}

void register_multicast_address(const uint8_t *mac)
{
  uint32_t crc;
  uint8_t hash;

  /* Calculate crc32 value of mac address */
  crc = ethcrc(mac, HASH_BITS);

  /*
   * Only upper HASH_BITS are used
   * which point to specific bit in the hash registers
   */
  hash = (crc >> 26) & 0x3F;

  if (hash > 31) {
    ETH_HashTableHigh |= 1 << (hash - 32);
    EthHandle.Instance->MACHTHR = ETH_HashTableHigh;
  } else {
    ETH_HashTableLow |= 1 << hash;
    EthHandle.Instance->MACHTLR = ETH_HashTableLow;
  }
}
#endif /* LWIP_IGMP */


#ifdef ETH_INPUT_USE_IT
/**
  * @brief  Ethernet Rx Transfer completed callback
  * @param  heth: ETH handle
  * @retval None
  */
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
  NeonRTOS_SyncObjSignalFromISR(&hal_eth_sync);
}

/**
  * @brief  This function handles Ethernet interrupt request.
  * @param  None
  * @retval None
  */
void ETH_IRQHandler(void)
{
  HAL_ETH_IRQHandler(&EthHandle);
}
#endif /* ETH_INPUT_USE_IT */

uint32_t ethernetif_get_tick(void)
{
  return sys_now();
}

void get_hardware_mac(uint8_t *mac) {
    // 使用 STM32 的唯一 ID 生成 MAC 地址
    uint32_t baseUID = *(uint32_t *)UID_BASE;
    mac[0] = 0x00;
    mac[1] = 0x80;
    mac[2] = 0xE1;
    mac[3] = (baseUID & 0x00FF0000) >> 16;
    mac[4] = (baseUID & 0x0000FF00) >> 8;
    mac[5] = (baseUID & 0x000000FF);
}

#endif //STM32

#ifdef __cplusplus
}
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
