#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"
#include "stm32f4xx.h"



static bool txCompleted = true;

static uint8_t DMA_rx_buffer[USART1_RX_BUFFER_SIZE];
static uint8_t *_rx_buffer;
static uint16_t receivedBytes = 0;
static bool shouldRxStop = false;
static bool new_data = false;
static uint16_t skippedFrames = 0;
bool USART1_NewData(void){
    bool ret = new_data;
    new_data=false;
    return ret;
}
uint16_t USART1_GetReceivedBytes(void){
    uint16_t ret = receivedBytes;
    receivedBytes = 0;
    return ret;
}

uint16_t USART1_GetSkippedFrames(void){
    return skippedFrames;
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

void USART1_Receive_DMA(uint8_t* rx_buffer){
    shouldRxStop = false;
    DMA2_Stream5->CR&= ~(DMA_SxCR_EN);      //disable dma rx
    while(DMA2_Stream5->CR&DMA_SxCR_EN);    //wait for it
    _rx_buffer = rx_buffer;
    DMA2_Stream5->M0AR = (uint32_t)DMA_rx_buffer;
    DMA2_Stream5->NDTR = USART1_RX_BUFFER_SIZE; 
    DMA2_Stream5->CR |= DMA_SxCR_EN;   
}
void USART1_StopReceiving(void){
    DMA2_Stream5->CR&= ~(DMA_SxCR_EN); 
    shouldRxStop = true;
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
        if(receivedBytes!=0)                    //check if bytes were readed
            skippedFrames++;
        receivedBytes = USART1_RX_BUFFER_SIZE - DMA2_Stream5->NDTR;    //we expected USART2_BUFFER_SIZE NDTR keeps how many bytes left to transfe
        memcpy(_rx_buffer,DMA_rx_buffer, receivedBytes);
        memset(DMA_rx_buffer, 0, sizeof(DMA_rx_buffer));
        DMA2_Stream5->M0AR = (uint32_t)DMA_rx_buffer;                   //start new transfer
        DMA2_Stream5->NDTR = USART1_RX_BUFFER_SIZE;
        if(!shouldRxStop)
            DMA2_Stream5->CR |= DMA_SxCR_EN;                              
    }
}
void DMA2_Stream7_IRQHandler(void)
{
    if(DMA2->HISR & DMA_HISR_TCIF7){ //if interupt is TC
        txCompleted = true;
        DMA2->HIFCR = DMA_HIFCR_CTCIF7;     //clear tc flag
    }
}
