#include <string.h>
#include <stdbool.h>
#include "config.h"
#include "drivers/USART.h"
#include "helpers/CommunicationProtocol/communication_protocol.h"
static enum
{
    NEW_MSG,
    CHUNK
}msg_status;
static uint16_t msg_len =0;
static uint16_t received_bytes = 0;
static bool new_msg;

static USART_t *port;
uint8_t _rx_buffer[255];
void _rx_completed_callback();

void RX_Handler_init()
{
    port = USART_GetUSART(CONFIG_COMM_HANDLER_USART);
    port->RXCompleteCallback = _rx_completed_callback;
    msg_status = NEW_MSG;
    received_bytes = 0;
    new_msg = false;
    
}
void RX_Handler_receive()
{
    new_msg =false;
    port->ReceiveDMA(_rx_buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
}
bool RX_Handler_CheckNewMessage()
{
    return new_msg;
}
uint16_t RX_Handler_GetNewMsg(uint8_t *rx_buffer)
{
    if(!new_msg)
        return 0;
    rx_buffer = _rx_buffer;
    return received_bytes;
}

void _rx_completed_callback()
{
    if(msg_status == NEW_MSG)
    {
        int len = COMPROTO_ParseHeader(_rx_buffer);
        if((len==-1)||(len>CONFIG_COMM_HANDLER_BUFFER_LEN))
            {
                port->ReceiveDMA(_rx_buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
                return;
            }
        msg_len = 7+len;
        if((msg_len) == port->GetReceivedBytes())
        {
            received_bytes = msg_len;
            new_msg = true;
        }
        else
        {
            msg_status = CHUNK;
            uint16_t count = port->GetReceivedBytes();
            received_bytes = count;
            port->ReceiveDMA(_rx_buffer+count, msg_len-count);
        }
    }
    else if(msg_status == CHUNK)
    {
        received_bytes+= port->GetReceivedBytes();
        if((received_bytes)<msg_len)
        {
           port->ReceiveDMA(_rx_buffer+received_bytes, msg_len-received_bytes);
        }
        else
        {
         msg_status = NEW_MSG;
         new_msg = true;   
        }
    }
}
