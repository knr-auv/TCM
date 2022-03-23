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
        .GetSkippedFrames = USART1_GetSkippedFrames,
        .NewDataFlag = USART1_NewDataFlag,
        .NewDataFlagReset = USART1_NewDataFlagReset,
        .ReceiveDMA = USART1_Receive_DMA,
        .StopReceiving = USART1_StopReceiving,
        .TransmitDMA = USART1_Transmit_DMA
    },
    [UART2] = {
        .CheckTxEnd = USART2_Check_Tx_end,
        .GetReceivedBytes = USART2_GetReceivedBytes,
        .GetSkippedFrames = USART2_GetSkippedFrames,
        .NewDataFlag = USART2_NewDataFlag,
        .NewDataFlagReset = USART2_NewDataFlagReset,
        .ReceiveDMA = USART2_Receive_DMA,
        .StopReceiving = USART2_StopReceiving,
        .TransmitDMA = USART2_Transmit_DMA
    },
    [UART3] = {
        .CheckTxEnd = USART3_Check_Tx_end,
        .GetReceivedBytes = USART3_GetReceivedBytes,
        .GetSkippedFrames = USART3_GetSkippedFrames,
        .NewDataFlag = USART3_NewDataFlag,
        .NewDataFlagReset = USART3_NewDataFlagReset,
        .ReceiveDMA = USART3_Receive_DMA,
        .StopReceiving = USART3_StopReceiving,
        .TransmitDMA = USART3_Transmit_DMA,

    }
};

void USART1_RC_Complete_Callback()
{
    if(USARTs[UART1].RXCompleteCallback)
    {
       USARTs[UART1].RXCompleteCallback(); 
    }
}
void USART2_RC_Complete_Callback()
{
    if(USARTs[UART2].RXCompleteCallback)
    {
       USARTs[UART2].RXCompleteCallback(); 
    }
}
void USART3_RC_Complete_Callback()
{
if(USARTs[UART3].RXCompleteCallback)
    {
       USARTs[UART3].RXCompleteCallback(); 
    }
}

USART_t* USART_GetUSART(UART_e usart)
{
    if((usart>=0)&&(usart<3))
        return &USARTs[usart];
    else 
    return 0;
}