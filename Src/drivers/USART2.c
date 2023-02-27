#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "time/time.h"
#include "Config/config.h"
#include "stm32f4xx.h"



static bool txCompleted = true;
static bool new_data = false;

static uint16_t receivedBytes = 0;

static uint16_t rx_buffer_size = 0;

void USART2_RX_Complete_Callback();

void USART2_NewDataFlagReset()
{
    new_data=false;
}

bool USART2_NewDataFlag(void){
    return new_data;
}
uint16_t USART2_GetReceivedBytes(void){
    uint16_t ret = receivedBytes;
    return ret;
}



bool USART2_Check_Tx_end(void){
    return txCompleted;
}

void USART2_Transmit_DMA(uint8_t* tx_buffer, uint16_t len){
    DMA1_Stream6->CR&=~(DMA_SxCR_EN);
    txCompleted = false;

    DMA1_Stream6->M0AR = (uint32_t)tx_buffer;
    DMA1_Stream6->NDTR = len;
    DMA1_Stream6->CR |= DMA_SxCR_EN;
}
static timeUs_t time = 0;
void USART2_Receive_DMA(uint8_t* rx_buffer, uint16_t buffer_size){
    rx_buffer_size = buffer_size;

    DMA1_Stream5->CR&= ~(DMA_SxCR_EN);      //disable dma rx
    time = micros();
    while(DMA1_Stream5->CR&DMA_SxCR_EN)    //wait for it
    {
        if(micros()-time>1000)
        {
            DMA2_Stream5->CR&= ~(DMA_SxCR_EN);
            return;
        }
    }
    DMA1_Stream5->M0AR = (uint32_t)rx_buffer;
    DMA1_Stream5->NDTR = buffer_size; 
    DMA1_Stream5->CR |= DMA_SxCR_EN;   
}
void USART2_StopReceiving(void){
    DMA1_Stream5->CR&= ~(DMA_SxCR_EN); 

}

void USART2_IRQHandler(void)
{
  if (USART2->SR & USART_SR_IDLE)
    {
        USART2->DR;                             //If not read usart willl crush                  
        DMA1_Stream5->CR &= ~DMA_SxCR_EN;       /* Disable DMA on stream 5 - trigers dma TC */
    }  

}
void DMA1_Stream5_IRQHandler(void)
{
    if(DMA1->HISR & DMA_HISR_TCIF5){            //if interupt is TC
        DMA1->HIFCR = DMA_HIFCR_CTCIF5;         //clear tc flag
        new_data = true;
        receivedBytes = rx_buffer_size - DMA1_Stream5->NDTR;    //we expected USART2_RX_BUFFER_SIZE NDTR keeps how many bytes left to transfe
        USART2_RX_Complete_Callback();
    }    
}
void DMA1_Stream6_IRQHandler(void)
{
    if(DMA1->HISR & DMA_HISR_TCIF6){ //if interupt is TC
        txCompleted = true;
        DMA1->HIFCR = DMA_HIFCR_CTCIF6;     //clear tc flag
    }
}
