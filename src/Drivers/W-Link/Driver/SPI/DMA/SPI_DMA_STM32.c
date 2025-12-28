#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "soc.h"

#include "NeonRTOS.h"

#include "SPI/SPI_Master.h"

#include "DMA/DMA.h"

#ifdef STM32

#include "DMA/DMA_STM32.h"

extern SPI_HandleTypeDef g_spi[hwSPI_Index_MAX];

#define STM32_DMA_LEGACY_ARCH

#ifdef STM32_DMA_NEW_ARCH
hwDMA_OpResult SPI_Config_DMA(hwSPI_Index index, hwDMA_Stream_Index channel, hwDMA_Peripheral_Direction dir)
{
        if(dir>=hwDMA_Peripheral_Direction_MAX || index>=hwSPI_Index_MAX)
        {
                return hwDMA_InvalidParameter;
        }

        if(channel>=hwDMA_Stream_Index_MAX)
        {
                return hwDMA_InvalidParameter;
        }

        if(index>=hwSPI_Index_MAX)
        {
                return hwDMA_InvalidParameter;
        }

        DMA_Stream_TypeDef* dma_soc_stream_base = NULL;

#if defined (BDMA_BASE)
        DMA_Stream_TypeDef* bdma_soc_channel_base = NULL;
        if(channel<hwDMA_Stream_Index_16)
        {
                dma_soc_stream_base = DMA_Map_Soc_Stream_Base(channel);
                if(dma_soc_stream_base==NULL)
                {
                        return hwDMA_InvalidParameter;
                }
                g_dma[channel].Instance = dma_soc_stream_base;
        }
        else if(channel>=hwDMA_Stream_Index_16 && channel<=hwDMA_Stream_Index_23)
        {
                bdma_soc_channel_base = BDMA_Map_Soc_Channel_Base(channel);
                if(bdma_soc_channel_base==NULL)
                {
                        return hwDMA_InvalidParameter;
                }
                g_dma[channel].Instance = bdma_soc_channel_base;
        }

        if(dma_soc_stream_base==NULL && bdma_soc_channel_base==NULL)
        {
                return hwDMA_InvalidParameter;
        }
#else //BDMA_BASE
        dma_soc_stream_base = DMA_Map_Soc_Stream_Base(channel);
        if(dma_soc_stream_base==NULL)
        {
                return hwDMA_InvalidParameter;
        }
        g_dma[channel].Instance = dma_soc_stream_base;

        if(dma_soc_stream_base==NULL)
        {
                return hwDMA_InvalidParameter;
        }
#endif //BDMA_BASE

        g_dma[channel].Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
        g_dma[channel].Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
        g_dma[channel].Init.MemBurst            = DMA_MBURST_SINGLE;
        g_dma[channel].Init.PeriphBurst         = DMA_PBURST_SINGLE;
        
        switch(dir)
        {
                case hwDMA_Peripheral_Direction_TX:
                        g_dma[channel].Init.Direction = DMA_MEMORY_TO_PERIPH;
                        switch(index)
                        {
                                case hwSPI_Index_0:
                                        g_dma[channel].Init.Request   = DMA_REQUEST_SPI1_TX;
                                        break;
                                case hwSPI_Index_1:
                                        g_dma[channel].Init.Request   = DMA_REQUEST_SPI2_TX;
                                        break;
                                case hwSPI_Index_2:
                                        g_dma[channel].Init.Request   = DMA_REQUEST_SPI3_TX;
                                        break;
#if defined (SPI4_BASE)
                                case hwSPI_Index_3:
                                        g_dma[channel].Init.Request   = DMA_REQUEST_SPI4_TX;
                                        break;
#endif
#if defined (SPI5_BASE)
                                case hwSPI_Index_4:
                                        g_dma[channel].Init.Request   = DMA_REQUEST_SPI5_TX;
                                        break;
#endif
#if defined (SPI6_BASE)
                                case hwSPI_Index_5:
                                        g_dma[channel].Init.Request   = BDMA_REQUEST_SPI6_TX;
                                        break;
#endif
                        }
                        break;
                case hwDMA_Peripheral_Direction_RX:
                        g_dma[channel].Init.Direction = DMA_PERIPH_TO_MEMORY;
                        switch(index)
                        {
                                case hwSPI_Index_0:
                                        g_dma[channel].Init.Request   = DMA_REQUEST_SPI1_RX;
                                        break;
                                case hwSPI_Index_1:
                                        g_dma[channel].Init.Request   = DMA_REQUEST_SPI2_RX;
                                        break;
                                case hwSPI_Index_2:
                                        g_dma[channel].Init.Request   = DMA_REQUEST_SPI3_RX;
                                        break;
#if defined (SPI4_BASE)
                                case hwSPI_Index_3:
                                        g_dma[channel].Init.Request   = DMA_REQUEST_SPI4_RX;
                                        break;
#endif
#if defined (SPI5_BASE)
                                case hwSPI_Index_4:
                                        g_dma[channel].Init.Request   = DMA_REQUEST_SPI5_RX;
                                        break;
#endif
#if defined (SPI6_BASE)
                                case hwSPI_Index_5:
                                        g_dma[channel].Init.Request   = BDMA_REQUEST_SPI6_RX;
                                        break;
#endif
                        }
                        break;
        }
        g_dma[channel].Init.PeriphInc           = DMA_PINC_DISABLE;
        g_dma[channel].Init.MemInc              = DMA_MINC_ENABLE;
        g_dma[channel].Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        g_dma[channel].Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        g_dma[channel].Init.Mode                = DMA_NORMAL;
        g_dma[channel].Init.Priority            = DMA_PRIORITY_HIGH;
        HAL_DMA_Init(&g_dma[channel]);

        switch(dir)
        {
                case hwDMA_Peripheral_Direction_TX:
                        __HAL_LINKDMA(&g_spi[index], hdmatx, g_dma[channel]);
                        break;
                case hwDMA_Peripheral_Direction_RX:
                        __HAL_LINKDMA(&g_spi[index], hdmarx, g_dma[channel]);
                        break;
        }

        return hwDMA_OK;
}
#endif //STM32_DMA_NEW_ARCH

#ifdef STM32_DMA_LEGACY_ARCH
static uint32_t SPI_DMA_Legacy_Channel_Map(hwSPI_Index index, hwDMA_Peripheral_Direction dir)
{
    /* F2 / F4 / F7 通用（實務上最安全） */
    switch (index)
    {
        case hwSPI_Index_0: /* SPI1 */
            return DMA_CHANNEL_3;

        case hwSPI_Index_1: /* SPI2 */
            return DMA_CHANNEL_0;

        case hwSPI_Index_2: /* SPI3 */
            return DMA_CHANNEL_0;

#if defined(SPI4_BASE)
        case hwSPI_Index_3: /* SPI4 */
            return DMA_CHANNEL_4;
#endif

#if defined(SPI5_BASE)
        case hwSPI_Index_4: /* SPI5 */
            return DMA_CHANNEL_2;
#endif

        default:
            return DMA_CHANNEL_0;
    }
}
hwDMA_OpResult SPI_Config_DMA(hwSPI_Index index, hwDMA_Stream_Index channel, hwDMA_Peripheral_Direction dir)
{
    if (index >= hwSPI_Index_MAX ||
        channel >= hwDMA_Stream_Index_MAX ||
        dir >= hwDMA_Peripheral_Direction_MAX)
    {
        return hwDMA_InvalidParameter;
    }

    DMA_Stream_TypeDef *stream = DMA_Map_Soc_Stream_Base(channel);
    if (stream == NULL)
    {
        return hwDMA_InvalidParameter;
    }

    g_dma[channel].Instance = stream;

    /* ---------- 基本 DMA init ---------- */
    g_dma[channel].Init.FIFOMode      = DMA_FIFOMODE_ENABLE;
    g_dma[channel].Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    g_dma[channel].Init.MemBurst      = DMA_MBURST_SINGLE;
    g_dma[channel].Init.PeriphBurst   = DMA_PBURST_SINGLE;

    g_dma[channel].Init.Direction =
        (dir == hwDMA_Peripheral_Direction_TX) ?
        DMA_MEMORY_TO_PERIPH :
        DMA_PERIPH_TO_MEMORY;

    /* ---------- legacy 關鍵：Channel ---------- */
    g_dma[channel].Init.Channel = SPI_DMA_Legacy_Channel_Map(index, dir);

    g_dma[channel].Init.PeriphInc           = DMA_PINC_DISABLE;
    g_dma[channel].Init.MemInc              = DMA_MINC_ENABLE;
    g_dma[channel].Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    g_dma[channel].Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    g_dma[channel].Init.Mode                = DMA_NORMAL;
    g_dma[channel].Init.Priority            = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&g_dma[channel]);

    /* ---------- Link DMA ---------- */
    if (dir == hwDMA_Peripheral_Direction_TX)
    {
        __HAL_LINKDMA(&g_spi[index], hdmatx, g_dma[channel]);
    }
    else
    {
        __HAL_LINKDMA(&g_spi[index], hdmarx, g_dma[channel]);
    }

    return hwDMA_OK;
}
#endif //STM32_DMA_LEGACY_ARCH

hwSPI_OpResult SPI_Transmit_DMA(hwSPI_Index index, uint8_t* buf, size_t len)
{
        if(index>=hwSPI_Index_MAX || buf==NULL)
        {
                return hwDMA_InvalidParameter;
        }

        if(index>=hwSPI_Index_MAX)
        {
                return hwDMA_InvalidParameter;
        }

        SCB_CleanDCache_by_Addr((uint32_t *)buf, len);
        HAL_SPI_Transmit_DMA(&g_spi[index], buf, len);

        while (HAL_SPI_GetState(&g_spi[index]) != HAL_SPI_STATE_READY) { }
        
        HAL_SPI_DMAStop(&g_spi[index]);

        return hwDMA_OK;
}

hwSPI_OpResult SPI_Receive_DMA(hwSPI_Index index, uint8_t* buf, size_t len)
{
        if(index>=hwSPI_Index_MAX || buf==NULL)
        {
                return hwDMA_InvalidParameter;
        }

        if(index>=hwSPI_Index_MAX)
        {
                return hwDMA_InvalidParameter;
        }

        SCB_InvalidateDCache_by_Addr((uint32_t *)buf, len);
        HAL_SPI_Receive_DMA(&g_spi[index], buf, len);

        while (HAL_SPI_GetState(&g_spi[index]) != HAL_SPI_STATE_READY) { }
        
        HAL_SPI_DMAStop(&g_spi[index]);

        return hwDMA_OK;
}

#endif //STM32
