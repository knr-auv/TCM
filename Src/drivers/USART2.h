#ifndef USART2_H
#define USART2_H
#include <stdbool.h>
#include <stdint.h>
void USART2_Transmit_DMA(uint8_t* tx_buffer, uint16_t len);
void USART2_Receive_DMA(uint8_t *rx_buffer, uint16_t buffer_size);  //once enabled it will serve forever untill stopreceiving will be called
void USART2_StopReceiving(void);
void USART2_NewDataFlagReset();
bool USART2_NewDataFlag(void);
bool USART2_Check_Tx_end(void);                 //its important to check it before new operation
uint16_t USART2_GetReceivedBytes(void);         //returns number of bytes that sits in rx buffer
uint16_t USART2_GetSkippedFrames(void);         //return number of unhandled frames. Frames is considered handled if USART2_GetReceivedBytes is called
void USART2_RC_Complete_Callback(void);

#endif