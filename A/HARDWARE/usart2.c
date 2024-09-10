#include "usart2.h"
#include "string.h"
#include "stdio.h"
#include "math.h"

u8 usart2_rx_buf[20];
rx_data_t rx_data;
s32 ecg_buf[128] = {0};
static u8 i = 0;

void usart2_init(void)
{
    GPIO_InitTypeDef     GPIO_InitStruct;
    USART_InitTypeDef    USART_InitStruct;
    NVIC_InitTypeDef     NVIC_InitStruct;
    DMA_InitTypeDef      DMA_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);      // 使能DMA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   // 使能GPIOA
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);   // 使能时钟 复用USART

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = 256000;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2,&USART_InitStruct);   //初始化 USART

    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  //开启 USART2 总线空闲中断

    USART_Cmd(USART2, ENABLE);//使能USART中断

    DMA_DeInit(DMA1_Channel6);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);    //外设--->内存
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)usart2_rx_buf;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_BufferSize = 20;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_Init(DMA1_Channel6, &DMA_InitStruct);

    DMA_Cmd(DMA1_Channel6, ENABLE);

    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x03;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Init(&NVIC_InitStruct);
}

void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        DMA_Cmd(DMA1_Channel6,DISABLE);

        USART2->SR;
        USART2->DR;

        if(usart2_rx_buf[0]==0xFF)
        {
			memcpy(&rx_data,usart2_rx_buf + 1,5);
			for(i=0;i<127;i++)
			{
				ecg_buf[i]=ecg_buf[i+1];
			}
			ecg_buf[127]=rx_data.ecg;
        }

        DMA_SetCurrDataCounter(DMA1_Channel6,20);

        DMA_Cmd(DMA1_Channel6,ENABLE);

        USART_ClearITPendingBit(USART2, USART_IT_IDLE); // 清除空闲中断
    }
}

