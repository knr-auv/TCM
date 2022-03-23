#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    void (*TransmitDMA)(uint8_t* tx_Buffer, uint16_t len);
    void (*ReceiveDMA)(uint8_t* rx_buffer, uint16_t buffer_len);
    void (*StopReceiving) ();
    bool (*NewDataFlag)();
    void (*NewDataFlagReset)();
    bool (*CheckTxEnd)();
    uint16_t (*GetReceivedBytes)();
    uint16_t (*GetSkippedFrames)();
    void (*RXCompleteCallback)();
}USART_t;

typedef enum
{
    UART1,
    UART2,
    UART3
} UART_e;

USART_t* USART_GetUSART(UART_e usart);