
#ifndef SPI_DMA_STM32_H
#define SPI_DMA_STM32_H

#include "soc.h"

#include "SPI/SPI_Master.h"

#include "DMA/DMA.h"

#define SPI0_MASTER_TX_DMA_STREAM hwDMA_Stream_Index_0
#define SPI0_MASTER_RX_DMA_STREAM hwDMA_Stream_Index_1
#define SPI1_MASTER_TX_DMA_STREAM hwDMA_Stream_Index_2
#define SPI1_MASTER_RX_DMA_STREAM hwDMA_Stream_Index_3
#define SPI2_MASTER_TX_DMA_STREAM hwDMA_Stream_Index_4
#define SPI2_MASTER_RX_DMA_STREAM hwDMA_Stream_Index_5
#define SPI3_MASTER_TX_DMA_STREAM hwDMA_Stream_Index_6
#define SPI3_MASTER_RX_DMA_STREAM hwDMA_Stream_Index_7
#define SPI4_MASTER_TX_DMA_STREAM hwDMA_Stream_Index_8
#define SPI4_MASTER_RX_DMA_STREAM hwDMA_Stream_Index_9

#if defined (BDMA_BASE)
#define SPI5_MASTER_TX_DMA_STREAM hwDMA_Stream_Index_16
#define SPI5_MASTER_RX_DMA_STREAM hwDMA_Stream_Index_17
#else //BDMA_BASE
#define SPI5_MASTER_TX_DMA_STREAM hwDMA_Stream_Index_10
#define SPI5_MASTER_RX_DMA_STREAM hwDMA_Stream_Index_11
#endif //BDMA_BASE

hwDMA_OpResult SPI_Config_DMA(hwSPI_Index index, hwDMA_Stream_Index channel, hwDMA_Peripheral_Direction dir);
hwSPI_OpResult SPI_Transmit_DMA(hwSPI_Index index, uint8_t* buf, size_t len);
hwSPI_OpResult SPI_Receive_DMA(hwSPI_Index index, uint8_t* buf, size_t len);

#endif //SPI_DMA_STM32_H