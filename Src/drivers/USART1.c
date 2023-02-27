#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "time/time.h"
#include "Config/config.h"
#include "stm32f4xx.h"



static bool txCompleted = true;

static uint16_t receivedBytes = 0;

static bool new_data = false;
static uint16_t rx_buffer_size = 0;

void USART1_RX_Complete_Callback();
void USART1_TX_Complete_Callback();
void USART1_NewDataFlagReset()
{
    new_data=false;
}

bool USART1_NewDataFlag(void){
    return new_data;
}
uint16_t USART1_GetReceivedBytes(void){
    uint16_t ret = receivedBytes;
    receivedBytes = 0;
    return ret;
}

bool USART1_Check_Tx_end(void){
    return txCompleted;
}

void USART1_Transmit_DMA(uint8_t* tx_buffer, uint16_t len){
    DMA2_Stream7->CR&=~(DMA_SxCR_EN);
    txCompleted = false;

    DMA2_Stream7->M0AR = (uint32_t)tx_buffer;
    DMA2_Stream7->NDTR = len;
    DMA2_Stream7->CR |= DMA_SxCR_EN;
}

static timeUs_t time = 0;
void USART1_Receive_DMA(uint8_t* rx_buffer, uint16_t buffer_size){
    rx_buffer_size = buffer_size;
    DMA2_Stream5->CR&= ~(DMA_SxCR_EN);      //disable dma rx
    time = micros();
    while(DMA2_Stream5->CR&DMA_SxCR_EN)
    {
        if(micros()-time>1000)
        {
            DMA2_Stream5->CR&= ~(DMA_SxCR_EN);
            return;
        }
    }
    DMA2_Stream5->M0AR = (uint32_t)rx_buffer;
    DMA2_Stream5->NDTR = buffer_size; 
    DMA2_Stream5->CR |= DMA_SxCR_EN;   
}
void USART1_StopReceiving(void){
    DMA2_Stream5->CR&= ~(DMA_SxCR_EN); 
}

void USART1_IRQHandler(void)
{
  if (USART1->SR & USART_SR_IDLE)
    {
        USART1->DR;                             //If not read usart willl crush                  
        DMA2_Stream5->CR &= ~DMA_SxCR_EN;       /* Disable DMA on stream 5 - trigers dma TC */
    }  

}
void DMA2_Stream5_IRQHandler(void)
{
    if(DMA2->HISR & DMA_HISR_TCIF5){            //if interupt is TC
        DMA2->HIFCR = DMA_HIFCR_CTCIF5;         //clear tc flag
        new_data = true;
        receivedBytes = rx_buffer_size - DMA2_Stream5->NDTR;    //we expected USART2_BUFFER_SIZE NDTR keeps how many bytes left to transfe      
        USART1_RX_Complete_Callback();                            
    }
}
void DMA2_Stream7_IRQHandler(void)
{
    if(DMA2->HISR & DMA_HISR_TCIF7){ //if interupt is TC
        txCompleted = true;
        DMA2->HIFCR = DMA_HIFCR_CTCIF7;     //clear tc flag
        USART1_TX_Complete_Callback();
    }
}
