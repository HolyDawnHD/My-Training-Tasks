#include "sys.h"

#ifndef __DMA_H
#define __DMA_H

/*
void dma_Init(void);
void DMA1_Stream4_IRQHandler(void);
void SendData(void);
*/

void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,uint16_t ndtr);

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,uint32_t chx,uint32_t par,uint32_t mar,uint16_t ndtr);

#endif


