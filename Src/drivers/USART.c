#include "USART1.h"
#include "USART2.h"
#include "USART3.h"
#include "USART.h"

USART_t USARTs[3] =
{
    [UART1] =
    {
        .CheckTxEnd = USART1_Check_Tx_end,
        .GetReceivedBytes = USART1_GetReceivedBytes,
        .NewDataFlag = USART1_NewDataFlag,
        .NewDataFlagReset = USART1_NewDataFlagReset,
        .ReceiveDMA = USART1_Receive_DMA,
        .StopReceiving = USART1_StopReceiving,
        .TransmitDMA = USART1_Transmit_DMA
    },
    [UART2] = {
        .CheckTxEnd = USART2_Check_Tx_end,
        .GetReceivedBytes = USART2_GetReceivedBytes,
        .NewDataFlag = USART2_NewDataFlag,
        .NewDataFlagReset = USART2_NewDataFlagReset,
        .ReceiveDMA = USART2_Receive_DMA,
        .StopReceiving = USART2_StopReceiving,
        .TransmitDMA = USART2_Transmit_DMA
    },
    [UART3] = {
        .CheckTxEnd = USART3_Check_Tx_end,
        .GetReceivedBytes = USART3_GetReceivedBytes,
        .NewDataFlag = USART3_NewDataFlag,
        .NewDataFlagReset = USART3_NewDataFlagReset,
        .ReceiveDMA = USART3_Receive_DMA,
        .StopReceiving = USART3_StopReceiving,
        .TransmitDMA = USART3_Transmit_DMA,
    }
};

void USART1_RX_Complete_Callback()
{
    if(USARTs[UART1].RXCompleteCallback)
    {
       USARTs[UART1].RXCompleteCallback(); 
    }
}
void USART1_TX_Complete_Callback()
{
    if(USARTs[UART1].TXCompleteCallback)
    {
       USARTs[UART1].TXCompleteCallback(); 
    }
}
void USART2_RX_Complete_Callback()
{
    if(USARTs[UART2].RXCompleteCallback)
    {
       USARTs[UART2].RXCompleteCallback(); 
    }
}

void USART3_RX_Complete_Callback()
{
    if(USARTs[UART3].RXCompleteCallback)
    {
       USARTs[UART3].RXCompleteCallback(); 
    }
}

void USART3_TX_Complete_Callback()
{
    if(USARTs[UART3].TXCompleteCallback)
    {
       USARTs[UART3].TXCompleteCallback(); 
    }
}


USART_t* USART_GetUSART(UART_e usart)
{
    if((usart>=0)&&(usart<3))
        return &USARTs[usart];
    else 
    return 0;
}