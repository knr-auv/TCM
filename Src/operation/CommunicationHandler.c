#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "time/time.h"
#include "helpers/CommunicationProtocol/communication_protocol.h"

#include "DirectThrustersCtrl.h"
#include "operation/ControlLoop.h"
#include "IO/Sticks.h"
#include "drivers/USART.h"
#include "ServiceMode.h"


static uint8_t rx_buffer[CONFIG_COMM_HANDLER_BUFFER_LEN];
static uint8_t tx_buffer[CONFIG_COMM_HANDLER_BUFFER_LEN];

timeUs_t last_time = 0;
COMPROTO_msg_info_t msg;
COMPROTO_msg_t msg_from_okon;

static USART_t uart;

enum
{
    NEW_MSG,
    CHUNK
}msg_status;
static bool new_msg = false;
static uint16_t received_bytes = 0;
uint16_t msg_len =0;

void HandleRequestMsg(COMPROTO_msg_info_t *msg);
void HandleSticksMsg(COMPROTO_msg_info_t *msg);
void HandeModeMsg(COMPROTO_msg_info_t *msg);
void HandleCLStatusMsg(COMPROTO_msg_info_t *msg);
void uart_rxEnd_irq();

timeUs_t COMHANDLER_TimeSinceLastUpdate()
{
    return micros()-last_time;
}

void rx_ir()
{
     uart.ReceiveDMA(rx_buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
}
void COMHANDLER_Init()
{
    USART_t* temp = USART_GetUSART(CONFIG_COMM_HANDLER_USART);
    temp->RXCompleteCallback = uart_rxEnd_irq;
    uart = *temp;
    uart.ReceiveDMA(rx_buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
    msg_from_okon.tx_buffer = tx_buffer;
}

bool COMHANDLER_CheckFun(timeUs_t currentTime, timeUs_t deltaTime)
{
    last_time = currentTime;

    return new_msg;
}
void uart_rxEnd_irq()
{
    if(msg_status == NEW_MSG)
    {
        int len = COMPROTO_ParseHeader(rx_buffer);
        if((len==-1)||(len>CONFIG_COMM_HANDLER_BUFFER_LEN))
            {
                uart.ReceiveDMA(rx_buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
                return;
            }
        msg_len = 7+len;
        if((msg_len) == uart.GetReceivedBytes())
        {
            received_bytes = msg_len;
            new_msg = true;
        }
        else
        {
            msg_status = CHUNK;
            uint16_t count = uart.GetReceivedBytes();
            received_bytes = count;
            uart.ReceiveDMA(rx_buffer+count, msg_len-count);
        }
    }
    else if(msg_status == CHUNK)
    {
        received_bytes+= uart.GetReceivedBytes();
        if((received_bytes)<msg_len)
        {
           uart.ReceiveDMA(rx_buffer+received_bytes, msg_len-received_bytes);
        }
        else
        {
         msg_status = NEW_MSG;
         new_msg = true;   
        }
    }
}
void COMHANDLER_Task(timeUs_t t)
{
    new_msg = false;
   
   COMPROTO_ParseMsg(rx_buffer, received_bytes, &msg);
   if(!msg.valid)
   {
       uart.ReceiveDMA(rx_buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
    return;
   }

   switch (msg.msg_type)
   {
   case MSG_TYPE_REQUEST:
       HandleRequestMsg(&msg);
       break;
    case MSG_TYPE_SERVICE:
        SERVICE_HandleMsg(&msg);
       break;
    case MSG_TYPE_STICKS:
       HandleSticksMsg(&msg);
       break;
   
   default:
       break;
   }
   uart.ReceiveDMA(rx_buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
}


void COMHANDLER_SendResponse(uint8_t* data, uint8_t len)
{
    uart.TransmitDMA(data, len);
}
void COMHANDLER_SendConfirmation(uint8_t data)
{
    msg_from_okon.user_data_len = 1;
    msg_from_okon.type = MSG_TYPE_FROM_OKON_SERVICE_CONFIRM;
    uint8_t buff[1];
    msg_from_okon.user_data = buff;
    msg_from_okon.user_data[0]= data;
    COMPROTO_CreateMsg(&msg_from_okon); 
    COMHANDLER_SendResponse(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
}

void HandleRequestMsg(COMPROTO_msg_info_t *msg)
{
    if(CL_GetStatus()==CL_STATUS_ARMED)
    {
        if(msg->data[0]==MSG_OKON_REQUEST_PID)
        {
            msg_from_okon.user_data =CL_SerializePIDs(&msg_from_okon.user_data_len);
            msg_from_okon.type = MSG_TYPE_FROM_OKON_PID;
            COMPROTO_CreateMsg(&msg_from_okon);
            free(msg_from_okon.user_data); 

            uart.TransmitDMA(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
        }
    }
    else
    {
        if(msg->data[0]==MSG_OKON_REQUEST_PID)
        {
            msg_from_okon.user_data = CL_SerializePIDs( &msg_from_okon.user_data_len);
            msg_from_okon.type = MSG_TYPE_FROM_OKON_PID;
            COMPROTO_CreateMsg(&msg_from_okon);
            free(msg_from_okon.user_data); 
            
            uart.TransmitDMA(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
        }
        else if (msg->data[0]==MSG_OKON_REQUEST_CL_MATRIX)
        {
            msg_from_okon.user_data = CL_SerializeControlThrustersMatrix(&msg_from_okon.user_data_len);
            msg_from_okon.type = MSG_TYPE_FROM_OKON_CL_MATRIX;
            COMPROTO_CreateMsg(&msg_from_okon);
           // free(msg_from_okon.user_data); 

            uart.TransmitDMA(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
        }
    }
    
}
void HandleSticksMsg(COMPROTO_msg_info_t *msg)
{
    STICK_HandleNewInput((float*)(msg->data), msg->len/(sizeof(float)));
}



void COMHANDLER_SendHeartBeat()
{
            msg_from_okon.user_data =0;
            msg_from_okon.user_data_len =0;
            msg_from_okon.type = MSG_TYPE_FROM_OKON_HEART_BEAT;
            COMPROTO_CreateMsg(&msg_from_okon);
            uart.TransmitDMA(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
           
}