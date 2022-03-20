#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "time/time.h"
#include "helpers/CommunicationProtocol/communication_protocol.h"

#include "DirectThrustersCtrl.h"
#include "operation/ControlLoop.h"
#include "IO/Sticks.h"
#include "drivers/USART3.h"
#include "ServiceMode.h"
#define RX_BUFFER_SIZE 252
#define TX_BUFFER_SIZE 252

static uint8_t rx_buffer[RX_BUFFER_SIZE];

timeUs_t last_time = 0;
COMPROTO_msg_info_t msg;
COMPROTO_msg_from_okon_t msg_from_okon;

bool pending_response = false;


void HandleRequestMsg(COMPROTO_msg_info_t *msg);
void HandleSticksMsg(COMPROTO_msg_info_t *msg);
void HandeModeMsg(COMPROTO_msg_info_t *msg);
void HandleCLStatusMsg(COMPROTO_msg_info_t *msg);

timeUs_t COMHANDLER_TimeSinceLastUpdate()
{
    return micros()-last_time;
}

void COMHANDLER_Init()
{
    USART3_Receive_DMA(rx_buffer);
}

bool COMHANDLER_CheckFun(timeUs_t currentTime, timeUs_t deltaTime)
{
    if(pending_response)
    {
        if(USART3_Check_Tx_end())
        {
            pending_response = false;
            free(msg_from_okon.tx_buffer);    
        }
            
            
    }
    if(USART3_NewData())
    {
        last_time = currentTime;
        return true;
    }
    return false;
}

void COMHANDLER_SendResponse(uint8_t* data, uint8_t len)
{
    if(!pending_response){
    USART3_Transmit_DMA(data, len);
    pending_response = true;

    }
}
void COMHANDLER_SendConfirmation(uint8_t data)
{
    if(pending_response)
        return;

    msg_from_okon.user_data_len = 1;
    msg_from_okon.type = MSG_FROM_OKON_SERVICE_CONFIRM;
    msg_from_okon.user_data =malloc(1);
    msg_from_okon.user_data[0]= data;
    COMPROTO_CreateMsg(&msg_from_okon); 
    pending_response =true;
    COMHANDLER_SendResponse(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
}
void COMHANDLER_Task(timeUs_t t)
{
   uint16_t len = USART3_GetReceivedBytes();
   COMPROTO_ParseMsg(rx_buffer, len, &msg);
   if(!msg.valid)
    return;

   switch (msg.msg_type)
   {
   case MSG_OKON_REQUEST:
       HandleRequestMsg(&msg);
       break;
    case MSG_OKON_SERVICE:
        SERVICE_HandleMsg(&msg);
       break;
    case MSG_OKON_STICKS:
       HandleSticksMsg(&msg);
       break;
    case MSG_OKON_MODE:
       HandeModeMsg(&msg);
       break;
    case MSG_OKON_CL_STATUS:
       HandleCLStatusMsg(&msg);
       break;
   
   default:
       break;
   }
}

void HandleRequestMsg(COMPROTO_msg_info_t *msg)
{
    if(!pending_response){
    if(CL_GetStatus()==CL_STATUS_ARMED)
    {
        if(msg->specific==MSG_OKON_REQUEST_PID)
        {
            msg_from_okon.user_data =CL_SerializePIDs(&msg_from_okon.user_data_len);
            msg_from_okon.type = MSG_FROM_OKON_PID;
            COMPROTO_CreateMsg(&msg_from_okon);
            free(msg_from_okon.user_data); 
            pending_response =true;
            COMHANDLER_SendResponse(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
        }
    }
    else
    {
        if(msg->specific==MSG_OKON_REQUEST_PID)
        {
            msg_from_okon.user_data = CL_SerializePIDs( &msg_from_okon.user_data_len);
            msg_from_okon.type = MSG_FROM_OKON_PID;
            COMPROTO_CreateMsg(&msg_from_okon);
            free(msg_from_okon.user_data); 
            pending_response =true;
            COMHANDLER_SendResponse(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
        }
        else if (msg->specific==MSG_OKON_REQUEST_CL_MATRIX)
        {
            msg_from_okon.user_data = CL_SerializeControlThrustersMatrix(&msg_from_okon.user_data_len);
            msg_from_okon.type = MSG_FROM_OKON_CL_MATRIX;
            COMPROTO_CreateMsg(&msg_from_okon);
           // free(msg_from_okon.user_data); 
            pending_response =true;
            COMHANDLER_SendResponse(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
        }
    }
    }
}
void HandleSticksMsg(COMPROTO_msg_info_t *msg)
{
    STICK_HandleNewInput((float*)(msg->data), msg->len/(sizeof(float)));
}

void HandeModeMsg(COMPROTO_msg_info_t *msg)
{
    uint8_t mode = msg->data[0];
    if(mode == CL_MODE_STABLE)
        CL_SetMode(mode);
    else if(mode == CL_MODE_ACRO)
        CL_SetMode(mode);

}
void HandleCLStatusMsg(COMPROTO_msg_info_t *msg)
{
    uint8_t status = msg->data[0];
    if(status == CL_STATUS_ARMED)
        CL_SetMode(status);
    else if(status == CL_STATUS_DISARMED)
        CL_SetMode(status);
}