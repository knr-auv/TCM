#ifndef USART1_H
#define USART1_H
#include <stdbool.h>
void USART1_Transmit_DMA(uint8_t* tx_buffer, uint16_t len);

void USART1_Receive_DMA(uint8_t *rx_buffer);  //once enabled it will serve forever untill stopreceiving will be called

void USART1_StopReceiving(void);
bool USART1_NewData(void);

bool USART1_Check_Tx_end(void);                 //its important to check it before new operation

uint16_t USART1_GetReceivedBytes(void);         //returns number of bytes that sits in rx buffer

uint16_t USART1_GetSkippedFrames(void);         //return number of unhandled frames. Frames is considered handled if USART2_GetReceivedBytes is called
#endif