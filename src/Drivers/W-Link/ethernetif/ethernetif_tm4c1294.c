/**
 * @file - tivaif.c
 * lwIP Ethernet interface for Stellaris LM4F Devices
 *
 */

/**
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICui32AR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/**
 * Copyright (c) 2008-2012 Texas Instruments Incorporated
 *
 * This file is derived from the ``ethernetif.c'' skeleton Ethernet network
 * interface driver for lwIP.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "soc.h"

#include "lwip/opt.h"
#include "lwip/timeouts.h"
#include "lwip/etharp.h"
#include "lwip/pbuf.h"
#include "lwip/mem.h"
#include "lwip/snmp.h"
#include "netif/ethernet.h"
#include "ethernetif.h"
#include "lwip/igmp.h"

#include "NeonRTOS.h"

#include "SysCtrl/SysCtrl.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEVICE_TM4C1294

/*---------------------------------------------------------------------------*/
/* Configuration Macros                                                      */
/*---------------------------------------------------------------------------*/
#define IFNAME0 'e'
#define IFNAME1 '0'

#ifndef EMAC_PHY_CONFIG
#define EMAC_PHY_CONFIG  (EMAC_PHY_TYPE_INTERNAL | EMAC_PHY_INT_MDIX_EN | \
                          EMAC_PHY_AN_100B_T_FULL_DUPLEX)
#endif

#ifndef PHY_PHYS_ADDR
#define PHY_PHYS_ADDR 0
#endif

/* Descriptor counts (可依需求調整) */
#ifndef NUM_RX_DESCRIPTORS
#define NUM_RX_DESCRIPTORS 20
#endif

#ifndef NUM_TX_DESCRIPTORS
#define NUM_TX_DESCRIPTORS 20
#endif

/* 若使用中斷輸入模式 */
#ifdef ETH_INPUT_USE_IT
#define ETHERNET_INT_PRIORITY   0xC0      /* 可調整 (低數值 = 高優先權) */
#define ETH_TASK_STACK_SIZE     2048
#define ETH_TASK_PRIORITY       (tskIDLE_PRIORITY + 2)
static NeonRTOS_SyncObj_t s_ethSync;
#endif

#if LWIP_IGMP
uint32_t ETH_HashTableHigh = 0x0;
uint32_t ETH_HashTableLow = 0x0;
#endif

//#define PKT_TX_DUMP
//#define PKT_RX_DUMP
//#define PKT_DUMP_RAW

/**
 *  Helper struct holding a DMA descriptor and the pbuf it currently refers
 *  to.
 */
typedef struct {
  tEMACDMADescriptor Desc;
  struct pbuf *pBuf;
} tDescriptor;

typedef struct {
    tDescriptor *pDescriptors;
    uint32_t ui32NumDescs;
    uint32_t ui32Read;
    uint32_t ui32Write;
} tDescriptorList;

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
typedef struct {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
  tDescriptorList *pTxDescList;
  tDescriptorList *pRxDescList;
} tStellarisIF;

/**
 * Global variable for this interface's private data.  Needed to allow
 * the interrupt handlers access to this information outside of the
 * context of the lwIP netif.
 *
 */
tDescriptor g_pTxDescriptors[NUM_TX_DESCRIPTORS];
tDescriptor g_pRxDescriptors[NUM_RX_DESCRIPTORS];
tDescriptorList g_TxDescList = {
    g_pTxDescriptors, NUM_TX_DESCRIPTORS, 0, 0
};
tDescriptorList g_RxDescList = {
    g_pRxDescriptors, NUM_RX_DESCRIPTORS, 0, 0
};
static tStellarisIF g_StellarisIFData = {
    0, &g_TxDescList, &g_RxDescList
};

static uint8_t macaddress[6] = {
    MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5
};

/**
 * Interrupt counters (for debug purposes).
 */
volatile uint32_t g_ui32NormalInts;
volatile uint32_t g_ui32AbnormalInts;

struct netif * pNetif = NULL;

/**
 * Status flag for EEE link established
 */
#if EEE_SUPPORT
volatile bool g_bEEELinkActive;
#endif

/**
 * A macro which determines whether a pointer is within the SRAM address
 * space and, hence, points to a buffer that the Ethernet MAC can directly
 * DMA from.
 */
#define PTR_SAFE_FOR_EMAC_DMA(ptr) (((uint32_t)(ptr) >= 0x20000000) &&   \
                                    ((uint32_t)(ptr) < 0x20070000))

/* System clock（假設 SysCtrl 已提供） */
extern uint32_t g_ui32SysClock;

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

/*---------------------------------------------------------------------------*/
/* Utility: CRC6 Hash (IGMP)                                                 */
/*---------------------------------------------------------------------------*/
#if LWIP_IGMP
#ifndef HASH_BITS
#define HASH_BITS 6
#endif

static uint32_t eth_crc32(const uint8_t *data, size_t len)
{
    uint32_t crc = 0xFFFFFFFF;
    for(size_t i=0;i<len;i++) {
        uint8_t b = data[i];
        for(int j=0;j<8;j++) {
            uint32_t mix = (crc ^ b) & 1;
            crc >>= 1;
            if(mix) crc ^= 0xEDB88320;
            b >>= 1;
        }
    }
    return ~crc;
}

static void mac_set_multicast_hash(const uint8_t *mac)
{
    /* Compute 6-bit hash (using upper bits of CRC32) */
    uint32_t crc = eth_crc32(mac, 6);
    uint8_t hash = (crc >> 26) & 0x3F;
    if(hash >= 32) {
        ETH_HashTableHigh |= (1U << (hash - 32));
    } else {
        ETH_HashTableLow  |= (1U << hash);
    }
    /* 寫入硬體 Hash Table */
    HWREG(EMAC0_BASE + EMAC_O_HASHTBLH) = ETH_HashTableHigh;
    HWREG(EMAC0_BASE + EMAC_O_HASHTBLL) = ETH_HashTableLow;
}

err_t igmp_mac_filter(struct netif *netif, const ip4_addr_t *ip4_addr,
                      netif_mac_filter_action action)
{
    LWIP_UNUSED_ARG(netif);
    if(action != NETIF_ADD_MAC_FILTER) {
        /* 簡化: 不處理刪除（要刪除需重建 hash 表） */
        return ERR_OK;
    }
    const uint8_t *p = (const uint8_t*)ip4_addr;
    uint8_t mac[6];
    mac[0]=0x01; mac[1]=0x00; mac[2]=0x5E;
    mac[3]=p[1] & 0x7F;
    mac[4]=p[2];
    mac[5]=p[3];
    mac_set_multicast_hash(mac);
    return ERR_OK;
}
#endif /* LWIP_IGMP */

/**
 * Initialize the transmit and receive DMA descriptor lists.
 */
void tivaif_initDMADescriptors(void)
{
    uint32_t ui32Loop;

    /* Transmit list -  mark all descriptors as not owned by the hardware */
   for(ui32Loop = 0; ui32Loop < NUM_TX_DESCRIPTORS; ui32Loop++)
   {
       g_pTxDescriptors[ui32Loop].pBuf = (struct pbuf *)0;
       g_pTxDescriptors[ui32Loop].Desc.ui32Count = 0;
       g_pTxDescriptors[ui32Loop].Desc.pvBuffer1 = 0;
       g_pTxDescriptors[ui32Loop].Desc.DES3.pLink =
               ((ui32Loop == (NUM_TX_DESCRIPTORS - 1)) ?
               &g_pTxDescriptors[0].Desc : &g_pTxDescriptors[ui32Loop + 1].Desc);
       g_pTxDescriptors[ui32Loop].Desc.ui32CtrlStatus = DES0_TX_CTRL_INTERRUPT |
               DES0_TX_CTRL_CHAINED | DES0_TX_CTRL_IP_ALL_CKHSUMS;

   }

   g_TxDescList.ui32Read = 0;
   g_TxDescList.ui32Write = 0;


   /* Receive list -  tag each descriptor with a pbuf and set all fields to
    * allow packets to be received.
    */
  for(ui32Loop = 0; ui32Loop < NUM_RX_DESCRIPTORS; ui32Loop++)
  {
      g_pRxDescriptors[ui32Loop].pBuf = pbuf_alloc(PBUF_RAW, PBUF_POOL_BUFSIZE,
                                                 PBUF_POOL);
      g_pRxDescriptors[ui32Loop].Desc.ui32Count = DES1_RX_CTRL_CHAINED;
      if(g_pRxDescriptors[ui32Loop].pBuf)
      {
          /* Set the DMA to write directly into the pbuf payload. */
          g_pRxDescriptors[ui32Loop].Desc.pvBuffer1 =
                  g_pRxDescriptors[ui32Loop].pBuf->payload;
          g_pRxDescriptors[ui32Loop].Desc.ui32Count |=
             (g_pRxDescriptors[ui32Loop].pBuf->len << DES1_RX_CTRL_BUFF1_SIZE_S);
          g_pRxDescriptors[ui32Loop].Desc.ui32CtrlStatus = DES0_RX_CTRL_OWN;
      }
      else
      {
          LWIP_DEBUGF(NETIF_DEBUG, ("tivaif_init: pbuf_alloc error\n"));

          /* No pbuf available so leave the buffer pointer empty. */
          g_pRxDescriptors[ui32Loop].Desc.pvBuffer1 = 0;
          g_pRxDescriptors[ui32Loop].Desc.ui32CtrlStatus = 0;
      }
      g_pRxDescriptors[ui32Loop].Desc.DES3.pLink =
              ((ui32Loop == (NUM_RX_DESCRIPTORS - 1)) ?
              &g_pRxDescriptors[0].Desc : &g_pRxDescriptors[ui32Loop + 1].Desc);
  }

  g_TxDescList.ui32Read = 0;
  g_TxDescList.ui32Write = 0;

  //
  // Set the descriptor pointers in the hardware.
  //
  MAP_EMACRxDMADescriptorListSet(EMAC0_BASE, &g_pRxDescriptors[0].Desc);
  MAP_EMACTxDMADescriptorListSet(EMAC0_BASE, &g_pTxDescriptors[0].Desc);
}

/**
 * In this function, the hardware should be initialized.
 * Called from tivaif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void tivaif_hwinit(struct netif *psNetif)
{
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    MAP_GPIOPinConfigure(GPIO_PF0_EN0LED0);
    MAP_GPIOPinConfigure(GPIO_PF4_EN0LED1);

    MAP_GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    //
    // Enable the ethernet peripheral.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_EMAC0);
    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_EMAC0);

    //
    // Enable the internal PHY if it's present and we're being
    // asked to use it.
    //
    if((EMAC_PHY_CONFIG & EMAC_PHY_TYPE_MASK) == EMAC_PHY_TYPE_INTERNAL)
    {
        //
        // We've been asked to configure for use with the internal
        // PHY.  Is it present?
        //
        if(MAP_SysCtlPeripheralPresent(SYSCTL_PERIPH_EPHY0))
        {
            //
            // Yes - enable and reset it.
            //
            MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_EPHY0);
            MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_EPHY0);
        }
        else
        {
            //
            // Internal PHY is not present on this part so hang here.
            //
            while(1)
            {
            }
        }
    }

    //
    // Wait for the MAC to come out of reset.
    //
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_EMAC0))
    {
    }

    //
    // Configure for use with whichever PHY the user requires.
    //
    MAP_EMACPHYConfigSet(EMAC0_BASE, EMAC_PHY_CONFIG);

    //
    // Initialize the MAC and set the DMA mode.
    //
    MAP_EMACInit(EMAC0_BASE, g_ui32SysClock,
                 EMAC_BCONFIG_MIXED_BURST | EMAC_BCONFIG_PRIORITY_FIXED,
                 4, 4, 0);

    //
    // Set MAC configuration options.
    //
    MAP_EMACConfigSet(EMAC0_BASE, (EMAC_CONFIG_FULL_DUPLEX |
                                   EMAC_CONFIG_CHECKSUM_OFFLOAD |
                                   EMAC_CONFIG_7BYTE_PREAMBLE |
                                   EMAC_CONFIG_IF_GAP_96BITS |
                                   EMAC_CONFIG_USE_MACADDR0 |
                                   EMAC_CONFIG_SA_FROM_DESCRIPTOR |
                                   EMAC_CONFIG_BO_LIMIT_1024),
                      (EMAC_MODE_RX_STORE_FORWARD |
                       EMAC_MODE_TX_STORE_FORWARD |
                       EMAC_MODE_TX_THRESHOLD_64_BYTES |
                       EMAC_MODE_RX_THRESHOLD_64_BYTES), 0);

    uint16_t ui16Val;

  /* clear the EEE Link Active flag */
#if EEE_SUPPORT
    g_bEEELinkActive = false;
#endif

    /* Set MAC hardware address length */
    psNetif->hwaddr_len = ETHARP_HWADDR_LEN;
        
    /* Set MAC hardware address */
    memcpy(psNetif->hwaddr, macaddress, 6);
    MAP_EMACAddrSet(EMAC0_BASE, 0, &(psNetif->hwaddr[0]));

    /* Maximum transfer unit */
    psNetif->mtu = 1500;

    /* Device capabilities */
    psNetif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    /* Initialize the DMA descriptors. */
    tivaif_initDMADescriptors();

#if defined(EMAC_PHY_IS_EXT_MII) || defined(EMAC_PHY_IS_EXT_RMII)
    /* If PHY is external then reset the PHY before configuring it */
    EMACPHYWrite(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_BMCR,
            EPHY_BMCR_MIIRESET);

    while((EMACPHYRead(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_BMCR) &
            EPHY_BMCR_MIIRESET) == EPHY_BMCR_MIIRESET);
#endif

    /* Clear any stray PHY interrupts that may be set. */
    ui16Val = MAP_EMACPHYRead(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_MISR1);
    ui16Val = MAP_EMACPHYRead(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_MISR2);

    /* Configure and enable the link status change interrupt in the PHY. */
    ui16Val = MAP_EMACPHYRead(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_SCR);
    ui16Val |= (EPHY_SCR_INTEN_EXT | EPHY_SCR_INTOE_EXT);
    MAP_EMACPHYWrite(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_SCR, ui16Val);
    MAP_EMACPHYWrite(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_MISR1, (EPHY_MISR1_LINKSTATEN |
                EPHY_MISR1_SPEEDEN | EPHY_MISR1_DUPLEXMEN | EPHY_MISR1_ANCEN));

    /* Read the PHY interrupt status to clear any stray events. */
    ui16Val = MAP_EMACPHYRead(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_MISR1);

    /**
     * Set MAC filtering options.  We receive all broadcast and mui32ticast
     * packets along with those addressed specifically for us.
     */
    MAP_EMACFrameFilterSet(EMAC0_BASE, (EMAC_FRMFILTER_HASH_AND_PERFECT |
                        EMAC_FRMFILTER_PASS_MULTICAST));

#if LWIP_PTPD
    //
    // Enable timestamping on all received packets.
    //
    // We set the fine clock adjustment mode and configure the subsecond
    // increment to half the 25MHz PTPD clock.  This will give us maximum control
    // over the clock rate adjustment and keep the arithmetic easy later.  It
    // should be possible to synchronize with higher accuracy than this with
    // appropriate juggling of the subsecond increment count and the addend
    // register value, though.
    //
    MAP_EMACTimestampConfigSet(EMAC0_BASE, (EMAC_TS_ALL_RX_FRAMES |
                            EMAC_TS_DIGITAL_ROLLOVER |
                            EMAC_TS_PROCESS_IPV4_UDP | EMAC_TS_ALL |
                            EMAC_TS_PTP_VERSION_1 | EMAC_TS_UPDATE_FINE),
                            (1000000000 / (25000000 / 2)));
    MAP_EMACTimestampAddendSet(EMAC0_BASE, 0x80000000);
    MAP_EMACTimestampEnable(EMAC0_BASE);
#endif

    /* Clear any pending MAC interrupts. */
    MAP_EMACIntClear(EMAC0_BASE, EMACIntStatus(EMAC0_BASE, false));

    /* Enable the Ethernet MAC transmitter and receiver. */
    MAP_EMACTxEnable(EMAC0_BASE);
    MAP_EMACRxEnable(EMAC0_BASE);

    /* Enable the Ethernet RX and TX interrupt source. */
    MAP_EMACIntEnable(EMAC0_BASE, (EMAC_INT_RECEIVE | EMAC_INT_TRANSMIT |
                    EMAC_INT_TX_STOPPED | EMAC_INT_RX_NO_BUFFER |
                    EMAC_INT_RX_STOPPED | EMAC_INT_PHY));

#ifdef ETH_INPUT_USE_IT
  /* Enable the Ethernet interrupt. */
    IntEnable(INT_EMAC0);
#endif

    /* Tell the PHY to start an auto-negotiation cycle. */
#if defined(EMAC_PHY_IS_EXT_MII) || defined(EMAC_PHY_IS_EXT_RMII)
    MAP_EMACPHYWrite(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_BMCR, (EPHY_BMCR_SPEED |
                EPHY_BMCR_DUPLEXM | EPHY_BMCR_ANEN | EPHY_BMCR_RESTARTAN));
#else
    MAP_EMACPHYWrite(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_BMCR, (EPHY_BMCR_ANEN |
                EPHY_BMCR_RESTARTAN));
#endif
}

#ifdef DEBUG
/**
 * Dump the chain of pbuf pointers to the debug output.
 */
void tivaif_trace_pbuf(const char *pcTitle, struct pbuf *p)
{
    LWIP_DEBUGF(NETIF_DEBUG, ("%s %08x (%d, %d)", pcTitle, p, p->tot_len,
                p->len));

    do
    {
        p = p->next;
        if(p)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("->%08x(%d)", p, p->len));
        }
        else
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("->%08x", p));
        }

    } while((p != NULL) && (p->tot_len != p->len));

    LWIP_DEBUGF(NETIF_DEBUG, ("\n"));
}
#endif

/**
 * This function is used to check whether a passed pbuf contains only buffers
 * resident in regions of memory that the Ethernet MAC can access.  If any
 * buffers in the chain are outside a directly-DMAable section of memory,
 * the pbuf is copied to SRAM and a different pointer returned.  If all
 * buffers are safe, the pbuf reference count is incremented and the original
 * pointer returned.
 */
static struct pbuf * tivaif_check_pbuf(struct pbuf *p)
{
    struct pbuf *pBuf;
    err_t Err;

    pBuf = p;

#ifdef DEBUG
    tivaif_trace_pbuf("Original:", p);
#endif

    /* Walk the list of buffers in the pbuf checking each. */
    do
    {
        /* Does this pbuf's payload reside in memory that the Ethernet DMA
         * can access?
         */
        if(!PTR_SAFE_FOR_EMAC_DMA(pBuf->payload))
        {
            /* This buffer is outside the DMA-able memory space so we need
             * to copy the pbuf.
             */
            pBuf = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_POOL);

            /* If we got a new pbuf... */
            if(pBuf)
            {
                /* ...copy the old pbuf into the new one. */
                Err = pbuf_copy(pBuf, p);

                /* If we failed to copy the pbuf, free the newly allocated one
                 * and make sure we return a NULL to show a problem.
                 */
                if(Err != ERR_OK)
                {
                    pbuf_free(pBuf);
                    pBuf = NULL;
                }
                else
                {
#ifdef DEBUG
                    tivaif_trace_pbuf("Copied:", pBuf);
#endif
                }
            }

            /* Reduce the reference count on the original pbuf since we're not
             * going to hold on to it after returning from tivaif_transmit.
             * Note that we already bumped the reference count at the top of
             * tivaif_transmit.
             */
            pbuf_free(p);

            /* Send back the new pbuf pointer or NULL if an error occurred. */
            return(pBuf);
        }

        /* Move on to the next buffer in the queue */
        pBuf = pBuf->next;
    }
    while(pBuf);

    /**
     * If we get here, the passed pbuf can be safely used without needing to
     * be copied.
     */
    return(p);
}

/**
 * This function will process all transmit descriptors and free pbufs attached
 * to any that have been transmitted since we last checked.
 *
 * This function is called only from the Ethernet interrupt handler.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return None.
 */
static void tivaif_process_transmit(tStellarisIF *pIF)
{
    tDescriptorList *pDescList;
    uint32_t ui32NumDescs;

    /* Get a pointer to the transmit descriptor list. */
    pDescList = pIF->pTxDescList;

    /* Walk the list until we have checked all descriptors or we reach the
     * write pointer or find a descriptor that the hardware is still working
     * on.
     */
    for(ui32NumDescs = 0; ui32NumDescs < pDescList->ui32NumDescs; ui32NumDescs++)
    {
        /* Has the buffer attached to this descriptor been transmitted? */
        if(pDescList->pDescriptors[pDescList->ui32Read].Desc.ui32CtrlStatus &
           DES0_TX_CTRL_OWN)
        {
            /* No - we're finished. */
            break;
        }

        /* Does this descriptor have a buffer attached to it? */
        if(pDescList->pDescriptors[pDescList->ui32Read].pBuf)
        {
            /* Yes - free it if it's not marked as an intermediate pbuf */
            if(!((uint32_t)(pDescList->pDescriptors[pDescList->ui32Read].pBuf) & 1))
            {
                pbuf_free(pDescList->pDescriptors[pDescList->ui32Read].pBuf);
            }
            pDescList->pDescriptors[pDescList->ui32Read].pBuf = NULL;
        }
        else
        {
            /* If the descriptor has no buffer, we are finished. */
            break;
        }

        /* Move on to the next descriptor. */
        pDescList->ui32Read++;
        if(pDescList->ui32Read == pDescList->ui32NumDescs)
        {
            pDescList->ui32Read = 0;
        }
    }
}

/**
 * This function will process all receive descriptors that contain newly read
 * data and pass complete frames up the lwIP stack as they are found.  The
 * timestamp of the packet will be placed into the pbuf structure if PTPD is
 * enabled.
 *
 * This function is called only from the Ethernet interrupt handler.
 *
 * @param psNetif the lwip network interface structure for this ethernetif
 * @return None.
 */
static void
tivaif_receive(struct netif *psNetif)
{
  tDescriptorList *pDescList;
  tStellarisIF *pIF;
  static struct pbuf *pBuf = NULL;
  uint32_t ui32DescEnd;

  /* Get a pointer to our state data */
  pIF = (tStellarisIF *)(psNetif->state);

  /* Get a pointer to the receive descriptor list. */
  pDescList = pIF->pRxDescList;

  /* Start with a NULL pbuf so that we don't try to link chain the first
   * time round.
   */
  //pBuf = NULL;

  /* Determine where we start and end our walk of the descriptor list */
  ui32DescEnd = pDescList->ui32Read ? (pDescList->ui32Read - 1) : (pDescList->ui32NumDescs - 1);

  /* Step through the descriptors that are marked for CPU attention. */
  while(pDescList->ui32Read != ui32DescEnd)
  {
      /* Does the current descriptor have a buffer attached to it? */
      if(pDescList->pDescriptors[pDescList->ui32Read].pBuf)
      {
          /* Yes - determine if the host has filled it yet. */
          if(pDescList->pDescriptors[pDescList->ui32Read].Desc.ui32CtrlStatus &
             DES0_RX_CTRL_OWN)
          {
              /* The DMA engine still owns the descriptor so we are finished */
              break;
          }

          /* If this descriptor contains the end of the packet, fix up the
           * buffer size accordingly.
           */
          if(pDescList->pDescriptors[pDescList->ui32Read].Desc.ui32CtrlStatus &
             DES0_RX_STAT_LAST_DESC)
          {
              /* This is the last descriptor for the frame so fix up the
               * length.  It is safe for us to modify the internal fields
               * directly here (rather than calling pbuf_realloc) since we
               * know each of these pbufs is never chained.
               */
              pDescList->pDescriptors[pDescList->ui32Read].pBuf->len =
                       (pDescList->pDescriptors[pDescList->ui32Read].Desc.ui32CtrlStatus &
                        DES0_RX_STAT_FRAME_LENGTH_M) >>
                        DES0_RX_STAT_FRAME_LENGTH_S;
              pDescList->pDescriptors[pDescList->ui32Read].pBuf->tot_len =
                        pDescList->pDescriptors[pDescList->ui32Read].pBuf->len;
          }

          if(pBuf)
          {
              /* Link this pbuf to the last one we looked at since this buffer
               * is a continuation of an existing frame (split across multiple
               * pbufs).  Note that we use pbuf_cat() here rather than
               * pbuf_chain() since we don't want to increase the reference
               * count of either pbuf - we only want to link them together.
               */
              pbuf_cat(pBuf, pDescList->pDescriptors[pDescList->ui32Read].pBuf);
              pDescList->pDescriptors[pDescList->ui32Read].pBuf = pBuf;
          }

          /* Remember the buffer associated with this descriptor. */
          pBuf = pDescList->pDescriptors[pDescList->ui32Read].pBuf;

          /* Is this the last descriptor for the current frame? */
          if(pDescList->pDescriptors[pDescList->ui32Read].Desc.ui32CtrlStatus &
             DES0_RX_STAT_LAST_DESC)
          {
              /* Yes - does the frame contain errors? */
              if(pDescList->pDescriptors[pDescList->ui32Read].Desc.ui32CtrlStatus &
                 DES0_RX_STAT_ERR)
              {
                  /* This is a bad frame so discard it and update the relevant
                   * statistics.
                   */
                  LWIP_DEBUGF(NETIF_DEBUG, ("tivaif_receive: packet error\n"));
                  pbuf_free(pBuf);
                  LINK_STATS_INC(link.drop);
                  pBuf = NULL;
              }
              else
              {
                  /* This is a good frame so pass it up the stack. */
                  LINK_STATS_INC(link.recv);

#if LWIP_PTPD
                  /* Place the timestamp in the PBUF if PTPD is enabled */
                  pBuf->time_s =
                       pDescList->pDescriptors[pDescList->ui32Read].Desc.ui32IEEE1588TimeHi;
                  pBuf->time_ns =
                       pDescList->pDescriptors[pDescList->ui32Read].Desc.ui32IEEE1588TimeLo;
#endif

                    err_t err;
                    err = psNetif->input(pBuf, psNetif);

#ifdef PKT_RX_DUMP
                    if(pBuf)
                        packet_dump(__FUNCTION__, pBuf);
#endif /* PKT_RX_DUMP */

                    if(err != ERR_OK)
                    {
                        /* drop the packet */
                        LWIP_DEBUGF(NETIF_DEBUG, ("tivaif_input: input error\n"));
                        pbuf_free(pBuf);

                        /* Adjust the link statistics */
                        LINK_STATS_INC(link.memerr);
                        LINK_STATS_INC(link.drop);
                    }

                  /* We're finished with this packet so make sure we don't try
                   * to link the next buffer to it.
                   */
                  pBuf = NULL;
              }
          }
      }

      /* Allocate a new buffer for this descriptor */
      pDescList->pDescriptors[pDescList->ui32Read].pBuf = pbuf_alloc(PBUF_RAW,
                                                        PBUF_POOL_BUFSIZE,
                                                        PBUF_POOL);
      pDescList->pDescriptors[pDescList->ui32Read].Desc.ui32Count =
                                                        DES1_RX_CTRL_CHAINED;
      if(pDescList->pDescriptors[pDescList->ui32Read].pBuf)
      {
          /* We got a buffer so fill in the payload pointer and size. */
          pDescList->pDescriptors[pDescList->ui32Read].Desc.pvBuffer1 =
                              pDescList->pDescriptors[pDescList->ui32Read].pBuf->payload;
          pDescList->pDescriptors[pDescList->ui32Read].Desc.ui32Count |=
                              (pDescList->pDescriptors[pDescList->ui32Read].pBuf->len <<
                               DES1_RX_CTRL_BUFF1_SIZE_S);

          /* Give this descriptor back to the hardware */
          pDescList->pDescriptors[pDescList->ui32Read].Desc.ui32CtrlStatus =
                              DES0_RX_CTRL_OWN;
      }
      else
      {
          LWIP_DEBUGF(NETIF_DEBUG, ("tivaif_receive: pbuf_alloc error\n"));

          pDescList->pDescriptors[pDescList->ui32Read].Desc.pvBuffer1 = 0;

          /* Update the stats to show we coui32dn't allocate a pbuf. */
          LINK_STATS_INC(link.memerr);

          /* Stop parsing here since we can't leave a broken descriptor in
           * the chain.
           */
          break;
      }

      /* Move on to the next descriptor in the chain, taking care to wrap. */
      pDescList->ui32Read++;
      if(pDescList->ui32Read == pDescList->ui32NumDescs)
      {
          pDescList->ui32Read = 0;
      }
  }
}


/**
 * Process interrupts from the PHY.
 *
 * should be called from the Stellaris Ethernet Interrupt Handler.  This
 * function will read packets from the Stellaris Ethernet fifo and place them
 * into a pbuf queue.  If the transmitter is idle and there is at least one packet
 * on the transmit queue, it will place it in the transmit fifo and start the
 * transmitter.
 *
 */
void
tivaif_process_phy_interrupt(struct netif *psNetif)
{
    uint16_t ui16Val, ui16Status;
#if EEE_SUPPORT
    uint16_t ui16EEEStatus;
#endif
    uint32_t ui32Config, ui32Mode, ui32RxMaxFrameSize;

    /* Read the PHY interrupt status.  This clears all interrupt sources.
     * Note that we are only enabling sources in EPHY_MISR1 so we don't
     * read EPHY_MISR2.
     */
    ui16Val = MAP_EMACPHYRead(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_MISR1);

    /* Read the current PHY status. */
    ui16Status = MAP_EMACPHYRead(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_STS);

    /* If EEE mode support is requested then read the value of the Link
     * partners status
     */
#if EEE_SUPPORT
    ui16EEEStatus = EMACPHYMMDRead(EMAC0_BASE, PHY_PHYS_ADDR, 0x703D);
#endif

    /* Has the link status changed? */
    if(ui16Val & EPHY_MISR1_LINKSTAT)
    {
        /* Is link up or down now? */
        if(ui16Status & EPHY_STS_LINK)
        {
            /* Tell lwIP the link is up. */
            netif_set_link_up(psNetif);

            /* if the link has been advertised as EEE capable then configure
             * the MAC register for LPI timers and manually set the PHY link
             * status bit
             */
#if EEE_SUPPORT
            if(ui16EEEStatus & 0x2)
            {
                MAP_EMACLPIConfig(EMAC0_BASE, true, 1000, 36);
                MAP_EMACLPILinkSet(EMAC0_BASE);
                g_bEEELinkActive = true;
            }
#endif

            /* In this case we drop through since we may need to reconfigure
             * the MAC depending upon the speed and half/fui32l-duplex settings.
             */
        }
        else
        {
            /* Tell lwIP the link is down */
            netif_set_link_down(psNetif);

            /* if the link has been advertised as EEE capable then clear the
             * MAC register LPI timers and manually clear the PHY link status
             * bit
             */
#if EEE_SUPPORT
           	g_bEEELinkActive = false;
           	MAP_EMACLPILinkClear(EMAC0_BASE);
           	MAP_EMACLPIConfig(EMAC0_BASE, false, 1000, 0);
#endif
        }
    }

    /* Has the speed or duplex status changed? */
    if(ui16Val & (EPHY_MISR1_SPEED | EPHY_MISR1_SPEED | EPHY_MISR1_ANC))
    {
        /* Get the current MAC configuration. */
        MAP_EMACConfigGet(EMAC0_BASE, &ui32Config, &ui32Mode,
                        &ui32RxMaxFrameSize);

        /* What speed is the interface running at now?
         */
        if(ui16Status & EPHY_STS_SPEED)
        {
            /* 10Mbps is selected */
            ui32Config &= ~EMAC_CONFIG_100MBPS;
        }
        else
        {
            /* 100Mbps is selected */
            ui32Config |= EMAC_CONFIG_100MBPS;
        }

        /* Are we in fui32l- or half-duplex mode? */
        if(ui16Status & EPHY_STS_DUPLEX)
        {
            /* Fui32l duplex. */
            ui32Config |= EMAC_CONFIG_FULL_DUPLEX;
        }
        else
        {
            /* Half duplex. */
            ui32Config &= ~EMAC_CONFIG_FULL_DUPLEX;
        }

        /* Reconfigure the MAC */
        MAP_EMACConfigSet(EMAC0_BASE, ui32Config, ui32Mode, ui32RxMaxFrameSize);
    }
}

/**
 * Process tx and rx packets at the low-level interrupt.
 *
 * should be called from the Stellaris Ethernet Interrupt Handler.  This
 * function will read packets from the Stellaris Ethernet fifo and place them
 * into a pbuf queue.  If the transmitter is idle and there is at least one packet
 * on the transmit queue, it will place it in the transmit fifo and start the
 * transmitter.
 *
 */
void ethernetif_input(struct netif *psNetif)
{
    // Read and Clear the interrupt.
    //
    uint32_t ui32Status = MAP_EMACIntStatus(EMAC0_BASE, true);

#if EEE_SUPPORT
    if(ui32Status & EMAC_INT_LPI)
    {
        EMACLPIStatus(EMAC0_BASE);
    }
#endif

    //
    // If the PMT mode exit status bit is set then enable the MAC transmit
    // and receive paths, read the PMT status to clear the interrupt and
    // clear the interrupt flag.
    //
    if(ui32Status & EMAC_INT_POWER_MGMNT)
    {
        MAP_EMACTxEnable(EMAC0_BASE);
        MAP_EMACRxEnable(EMAC0_BASE);

        MAP_EMACPowerManagementStatusGet(EMAC0_BASE);

        ui32Status &= ~(EMAC_INT_POWER_MGMNT);
    }

    //
    // If the interrupt really came from the Ethernet and not our
    // timer, clear it.
    //
    if(ui32Status)
    {
        MAP_EMACIntClear(EMAC0_BASE, ui32Status);
    }

    MAP_EMACIntDisable(EMAC0_BASE, (EMAC_INT_RECEIVE | EMAC_INT_TRANSMIT |
                                    EMAC_INT_TX_STOPPED |
                                    EMAC_INT_RX_NO_BUFFER |
                                    EMAC_INT_RX_STOPPED | EMAC_INT_PHY));
    tStellarisIF *tivaif;

    /* setup pointer to the if state data */
    tivaif = psNetif->state;

    /* Update our debug interrupt counters. */
    if(ui32Status & EMAC_INT_NORMAL_INT)
    {
        g_ui32NormalInts++;
    }

    if(ui32Status & EMAC_INT_ABNORMAL_INT)
    {
        g_ui32AbnormalInts++;
    }

    /* Is this an interrupt from the PHY? */
    if(ui32Status & EMAC_INT_PHY)
    {
        tivaif_process_phy_interrupt(psNetif);
    }

    /* Process the transmit DMA list, freeing any buffers that have been
    * transmitted since our last interrupt.
    */
    if(ui32Status & EMAC_INT_TRANSMIT)
    {
#if EEE_SUPPORT
        if(g_bEEELinkActive)
        {
            EMACLPIEnter(EMAC0_BASE);
        }
#endif

        tivaif_process_transmit(tivaif);
    }

    /**
     * Process the receive DMA list and pass all successfully received packets
     * up the stack.  We also call this function in cases where the receiver has
     * stalled due to missing buffers since the receive function will attempt to
     * allocate new pbufs for descriptor entries which have none.
     */
    if(ui32Status & (EMAC_INT_RECEIVE | EMAC_INT_RX_NO_BUFFER |
        EMAC_INT_RX_STOPPED))
    {
        tivaif_receive(psNetif);
    }

    //
    // Re-enable the Ethernet interrupts.
    //
    MAP_EMACIntEnable(EMAC0_BASE, (EMAC_INT_RECEIVE | EMAC_INT_TRANSMIT |
                                    EMAC_INT_TX_STOPPED |
                                    EMAC_INT_RX_NO_BUFFER |
                                    EMAC_INT_RX_STOPPED | EMAC_INT_PHY));
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf might be
 * chained.
 *
 * @param psNetif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet coui32d be sent
 *         an err_t value if the packet coui32dn't be sent
 */
static err_t low_level_output(struct netif *psNetif, struct pbuf *p)
{
  tStellarisIF *pIF;
  tDescriptor *pDesc;
  struct pbuf *pBuf;
  uint32_t ui32NumChained, ui32NumDescs;
  bool bFirst;
  SYS_ARCH_DECL_PROTECT(lev);

  LWIP_DEBUGF(NETIF_DEBUG, ("tivaif_transmit 0x%08x, len %d\n", p,
              p->tot_len));

  /**
   * This entire function must run within a "critical section" to preserve
   * the integrity of the transmit pbuf queue.
   */
  SYS_ARCH_PROTECT(lev);

#ifdef PKT_TX_DUMP
    packet_dump(__FUNCTION__, p);
#endif /* PKT_TX_DUMP */

  /**
   * Increase the reference count on the packet provided so that we can
   * hold on to it until we are finished transmitting its content.
   */
  pbuf_ref(p);

  /**
   * Determine whether all buffers passed are within SRAM and, if not, copy
   * the pbuf into SRAM-resident buffers so that the Ethernet DMA can access
   * the data.
   */
  p = tivaif_check_pbuf(p);

  /* Make sure we still have a valid buffer (it may have been copied) */
  if(!p)
  {
      LINK_STATS_INC(link.memerr);
      SYS_ARCH_UNPROTECT(lev);
      return(ERR_MEM);
  }

  /* Get our state data from the netif structure we were passed. */
  pIF = (tStellarisIF *)psNetif->state;

  /* Make sure that the transmit descriptors are not all in use */
  pDesc = &(pIF->pTxDescList->pDescriptors[pIF->pTxDescList->ui32Write]);
  if(pDesc->pBuf)
  {
      /**
       * The current write descriptor has a pbuf attached to it so this
       * implies that the ring is full. Reject this transmit request with a
       * memory error since we can't satisfy it just now.
       */
      pbuf_free(p);
      LINK_STATS_INC(link.memerr);
      SYS_ARCH_UNPROTECT(lev);
      return (ERR_MEM);
  }

  /* How many pbufs are in the chain passed? */
  ui32NumChained = (uint32_t)pbuf_clen(p);

  /* How many free transmit descriptors do we have? */
  ui32NumDescs = (pIF->pTxDescList->ui32Read > pIF->pTxDescList->ui32Write) ?
          (pIF->pTxDescList->ui32Read - pIF->pTxDescList->ui32Write) :
          ((NUM_TX_DESCRIPTORS - pIF->pTxDescList->ui32Write) +
           pIF->pTxDescList->ui32Read);

  /* Do we have enough free descriptors to send the whole packet? */
  if(ui32NumDescs < ui32NumChained)
  {
      /* No - we can't transmit this whole packet so return an error. */
      pbuf_free(p);
      LINK_STATS_INC(link.memerr);
      SYS_ARCH_UNPROTECT(lev);
      return (ERR_MEM);
  }

  /* Tag the first descriptor as the start of the packet. */
  bFirst = true;
  pDesc->Desc.ui32CtrlStatus = DES0_TX_CTRL_FIRST_SEG;

  /* Here, we know we can send the packet so write it to the descriptors */
  pBuf = p;

  while(ui32NumChained)
  {
      /* Get a pointer to the descriptor we will write next. */
      pDesc = &(pIF->pTxDescList->pDescriptors[pIF->pTxDescList->ui32Write]);

      /* Fill in the buffer pointer and length */
      pDesc->Desc.ui32Count = (uint32_t)pBuf->len;
      pDesc->Desc.pvBuffer1 = pBuf->payload;

      /* Tag the first descriptor as the start of the packet. */
      if(bFirst)
      {
          bFirst = false;
          pDesc->Desc.ui32CtrlStatus = DES0_TX_CTRL_FIRST_SEG;
      }
      else
      {
          pDesc->Desc.ui32CtrlStatus = 0;
      }

      pDesc->Desc.ui32CtrlStatus |= (DES0_TX_CTRL_IP_ALL_CKHSUMS |
                                     DES0_TX_CTRL_CHAINED);

      /* Decrement our descriptor counter, move on to the next buffer in the
       * pbuf chain. */
      ui32NumChained--;
      pBuf = pBuf->next;

      /* Update the descriptor list write index. */
      pIF->pTxDescList->ui32Write++;
      if(pIF->pTxDescList->ui32Write == NUM_TX_DESCRIPTORS)
      {
          pIF->pTxDescList->ui32Write = 0;
      }

      /* If this is the last descriptor, mark it as the end of the packet. */
      if(!ui32NumChained)
      {
          pDesc->Desc.ui32CtrlStatus |= (DES0_TX_CTRL_LAST_SEG |
                                         DES0_TX_CTRL_INTERRUPT);

          /* Tag the descriptor with the original pbuf pointer. */
          pDesc->pBuf = p;
      }
      else
      {
          /* Set the lsb of the pbuf pointer.  We use this as a signal that
           * we should not free the pbuf when we are walking the descriptor
           * list while processing the transmit interrupt.  We only free the
           * pbuf when processing the last descriptor used to transmit its
           * chain.
           */
          pDesc->pBuf = (struct pbuf *)((uint32_t)p + 1);
      }

      /* Hand the descriptor over to the hardware. */
      pDesc->Desc.ui32CtrlStatus |= DES0_TX_CTRL_OWN;
  }

  /* Tell the transmitter to start (in case it had stopped). */
  MAP_EMACTxDMAPollDemand(EMAC0_BASE);

  /* Update lwIP statistics */
  LINK_STATS_INC(link.xmit);

  SYS_ARCH_UNPROTECT(lev);

  return(ERR_OK);
}

#ifdef ETH_INPUT_USE_IT
void EMAC0_IRQHandler(void)
{
    /* 若需要喚醒任務： */
    NeonRTOS_SyncObjSignalFromISR(&s_ethSync);
}
#endif

/*---------------------------------------------------------------------------*/
/* netif 初始化                                                              */
/*---------------------------------------------------------------------------*/
err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif!=NULL", netif != NULL);

#if LWIP_NETIF_HOSTNAME
    netif->hostname = "NeonRT";
#endif
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

    netif->state = &g_StellarisIFData;

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    netif->output   = etharp_output;
    netif->linkoutput = low_level_output;

    pNetif = netif;

#if LWIP_IGMP
    ETH_HashTableHigh = 0;
    ETH_HashTableLow  = 0;
#endif

#ifdef ETH_INPUT_USE_IT
    if(NeonRTOS_SyncObjCreate(&s_ethSync) != NeonRTOS_OK) {
        return ERR_MEM;
    }
    NeonRTOS_SyncObjClear(&s_ethSync);
    /* 你可以建立一個處理 task */
    NeonRTOS_TaskCreate(
        /* task fn */ [](void *arg){
            (void)arg;
            for(;;){
                if(NeonRTOS_SyncObjWait(&s_ethSync, NEONRT_WAIT_FOREVER)==NeonRTOS_OK){
                    ethernetif_input(s_ethIF.netif);
                }
            }
        },
        "eth_task",
        ETH_TASK_STACK_SIZE,
        NULL,
        ETH_TASK_PRIORITY,
        NULL
    );
#endif

    /* Initialize the hardware */
    tivaif_hwinit(netif);

    netif_set_up(netif);

    return ERR_OK;
}

/*---------------------------------------------------------------------------*/
/* 其他 API (可視需求保留/簡化)                                              */
/*---------------------------------------------------------------------------*/
/**
 * @brief  讀取 PHY 並更新 netif 的 link (up/down) 狀態。
 *         相當於 STM32 的 ethernetif_set_link()。
 */
void ethernetif_set_link(struct netif *netif)
{
    /* 讀取並清除 PHY 中斷（如果需要），這裡可用 MISR 但不是一定要在這裡清 */
    (void)MAP_EMACPHYRead(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_MISR1); /* 讀即清，不使用回傳 */

    /* ❶ 第一次讀 – 清掉 latched 0 */
    MAP_EMACPHYRead(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_BMSR);

    /* ❷ 第二次讀 – 取得即時狀態                          */
    uint16_t bmsr = MAP_EMACPHYRead(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_BMSR);
    bool linkNow = (bmsr & EPHY_BMSR_LINKSTAT) != 0;
    
    if(linkNow) {
        /* Link 由 down -> up */
#if LWIP_IGMP
        if(!(netif->flags & NETIF_FLAG_IGMP)) {
            netif->flags |= NETIF_FLAG_IGMP;
            /* 通常 igmp_start 只需在加入組播時；這裡視需要可不主動呼叫 */
        }
#endif
        netif_set_link_up(netif);
    }
    else {
        /* Link 由 up -> down */
        netif_set_link_down(netif);
    }
    /* 若狀態未變化，不做事 */
}

/**
 * @brief  依目前 PHY 協商結果 (速度/雙工) 重新配置 MAC。
 *         相當於 STM32 的 ethernetif_update_config()。
 */
void ethernetif_update_config(struct netif *netif)
{
    LWIP_UNUSED_ARG(netif);

    /* ❶ 第一次讀 – 清掉 latched 0 */
    MAP_EMACPHYRead(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_BMSR);

    /* ❷ 第二次讀 – 取得即時狀態                          */
    uint16_t bmsr = MAP_EMACPHYRead(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_BMSR);
    bool linkNow = (bmsr & EPHY_BMSR_LINKSTAT) != 0;
    
    uint16_t sts = MAP_EMACPHYRead(EMAC0_BASE, PHY_PHYS_ADDR, EPHY_STS);

    if(!linkNow) {
        /* 選擇性：可停用 Tx/Rx 或僅記錄狀態 */
        MAP_EMACTxDisable(EMAC0_BASE);
        MAP_EMACRxDisable(EMAC0_BASE);
        return;
    }

    /* 取得目前 MAC 設定 */
    uint32_t cfg, mode, rxsz;
    MAP_EMACConfigGet(EMAC0_BASE, &cfg, &mode, &rxsz);

    /* 調整速率：EPHY_STS_SPEED =1 => 10Mbps, =0 => 100Mbps */
    if(sts & EPHY_STS_SPEED) {
        cfg &= ~EMAC_CONFIG_100MBPS;  /* 10M */
    } else {
        cfg |= EMAC_CONFIG_100MBPS;   /* 100M */
    }

    /* 調整雙工：EPHY_STS_DUPLEX =1 => Full */
    if(sts & EPHY_STS_DUPLEX) {
        cfg |= EMAC_CONFIG_FULL_DUPLEX;
    } else {
        cfg &= ~EMAC_CONFIG_FULL_DUPLEX;
    }

    /* 重新寫回設定 */
    MAP_EMACConfigSet(EMAC0_BASE, cfg, mode, rxsz);

    /* 不需特地停/啟；EMACConfigSet 會套用新設定。若想保守，可：*/
    MAP_EMACTxEnable(EMAC0_BASE);
    MAP_EMACRxEnable(EMAC0_BASE);
    
    /* 如果你想在這裡提供外部通知 (等同 STM32 的 ethernetif_notify_conn_changed) */
    ethernetif_notify_conn_changed(netif);
}

/* 選用：若要與 lwIP 的 netif link callback 整合，可提供 stub */
__attribute__((weak)) void ethernetif_notify_conn_changed(struct netif *netif)
{
    LWIP_UNUSED_ARG(netif);
    /* 使用者可在另一個檔案重載此函式，打印 log 或觸發事件 */
}

uint8_t ethernetif_is_init(void)
{
    return (pNetif != NULL);
}

void ethernetif_set_mac_addr(const uint8_t *mac)
{
    if(mac){
        memcpy(macaddress, mac, 6);
        if(pNetif){
            memcpy(pNetif->hwaddr, mac, 6);
            MAP_EMACAddrSet(EMAC0_BASE, 0, (uint8_t*)mac);
        }
    }
}

u32_t sys_now(void)
{
    /* 替換成你的系統 tick 函數 */
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

uint32_t ethernetif_get_tick(void)
{
    return sys_now();
}

void get_hardware_mac(uint8_t mac[6])
{
    uint32_t user0, user1;

    MAP_FlashUserGet(&user0, &user1);        /* 取出兩個 32‑bit */

    if (user0 == 0xFFFFFFFF || user1 == 0xFFFFFFFF) {
        /* 這顆晶片還沒燒 MAC，自己決定要用預設值或停機 */
        return;
    }

    mac[0] =  user0        & 0xFF;
    mac[1] = (user0 >>  8) & 0xFF;
    mac[2] = (user0 >> 16) & 0xFF;
    mac[3] =  user1        & 0xFF;
    mac[4] = (user1 >>  8) & 0xFF;
    mac[5] = (user1 >> 16) & 0xFF;
}

#endif //DEVICE_TM4C1294

#ifdef __cplusplus
}
#endif