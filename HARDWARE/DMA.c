#include "DMA.h"
#include "usart.h"
#include "sys.h"

/*
char SendToPC_Buff[6];

void dma_Init(void)
{
	USART_DMACmd(UART5,USART_DMAReq_Rx,ENABLE);       
    USART_DMACmd(UART5,USART_DMAReq_Tx,ENABLE);
	
	DMA_InitTypeDef dma;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
    DMA_DeInit(DMA1_Stream7);
    dma.DMA_Channel= DMA_Channel_4;
    dma.DMA_PeripheralBaseAddr = (uint32_t)&(UART5->DR);
    dma.DMA_Memory0BaseAddr = (uint32_t)SendToPC_Buff;
    dma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    dma.DMA_BufferSize = 6;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_Priority = DMA_Priority_VeryHigh;
    dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
    dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    dma.DMA_MemoryBurst = DMA_Mode_Normal;
    dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream7,&dma);
    DMA_ITConfig(DMA1_Stream7,DMA_IT_TC,ENABLE);               
    DMA_Cmd(DMA1_Stream7,DISABLE);
	
}

void DMA1_Stream4_IRQHandler(void)
{
        if(DMA_GetFlagStatus(DMA1_Stream7, DMA_IT_TCIF7) == SET)
        {
                DMA_Cmd(DMA1_Stream7, DISABLE);
                DMA_SetCurrDataCounter(DMA1_Stream7, 6);
                DMA_ClearFlag(DMA1_Stream7, DMA_FLAG_TCIF7);
                DMA_ClearITPendingBit(DMA1_Stream7, DMA_IT_TCIF7);
        }
		
}

void SendData(void)
{
        SendToPC_Buff[0]=0x01;
		SendToPC_Buff[1]=0x02;
		SendToPC_Buff[2]=0x03;
        SendToPC_Buff[3]=0x04;
        SendToPC_Buff[4]=0x05;
        SendToPC_Buff[5]=0x06;
        DMA_Cmd(DMA1_Stream7, ENABLE);
}

*/



void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr)
{ 
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	DMA_DeInit(DMA_Streamx);
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}
	
	 DMA_InitStructure.DMA_Channel = chx; 
	 DMA_InitStructure.DMA_PeripheralBaseAddr = par;
	 DMA_InitStructure.DMA_Memory0BaseAddr = mar;
	 DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	 DMA_InitStructure.DMA_BufferSize = ndtr;
	 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

	 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

	 DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	 DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	 DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; 
	 DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	 DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;

	 DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	 DMA_Init(DMA_Streamx, &DMA_InitStructure);
} 

void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	DMA_Cmd(DMA_Streamx, DISABLE); 
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){} 
	DMA_SetCurrDataCounter(DMA_Streamx,ndtr); 
	DMA_Cmd(DMA_Streamx, ENABLE); 
}


