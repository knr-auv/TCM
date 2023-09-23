#include <stdbool.h>
#include <string.h>
#include "Config/config.h"
#include "time/time.h"
#include "drivers/USART.h"
#include "protocol.h"
#include "IO/Sticks.h"
#include "operation/ControlLoop.h"
static void _tx_complete_cb();
static void _rx_complete_cb();
static void HandleNewMsg();

static USART_t *port;
static COM_PROTO_msg_t msg_tx, msg_rx;

static uint8_t rx_buffer[CONFIG_COMM_HANDLER_BUFFER_LEN];
static uint8_t tx_buffer[CONFIG_COMM_HANDLER_BUFFER_LEN];


static bool tx_complete, receiving;
static uint16_t received_bytes;
static timeUs_t last_update;
// void HandleRequestMsg(COMPROTO_msg_info_t *msg);
// void HandleSticksMsg(COMPROTO_msg_info_t *msg);
// void HandeModeMsg(COMPROTO_msg_info_t *msg);
// void HandleCLStatusMsg(COMPROTO_msg_info_t *msg);
static uint32_t failed_packets = 0;
static void SendPacket(uint8_t* buffer, uint16_t len)
{
     if(tx_complete==false)
    {
        failed_packets++;
        return;
    }
    tx_complete = false;
    port->TransmitDMA(tx_buffer,len);
    
}
timeUs_t COMHANDLER_TimeSinceLastUpdate()
{
    return micros()-last_update;
}
void COMHANDLER_Init()
{
    port = USART_GetUSART(CONFIG_COMM_HANDLER_USART);
    port->RXCompleteCallback = _rx_complete_cb;
    port->TXCompleteCallback = _tx_complete_cb;
    tx_complete =true;
}


bool COMHANDLER_CheckFun(timeUs_t currentTime, timeUs_t deltaTime)
{
    if(!receiving)
        return true;
    return false;
}

void COMHANDLER_Task(timeUs_t t)
{
    received_bytes = port->GetReceivedBytes();
    if(received_bytes>0)
    {
        for(uint16_t i =0; i<received_bytes;i++)
            if(COM_PROTO_ParseByte(rx_buffer[i],&msg_rx))
            {
                last_update = micros();
                HandleNewMsg();
            }
    }
    if(!receiving)
    {
        port->ReceiveDMA(rx_buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
    }
}


void COMHANDLER_SendTelemetry(const uint8_t *data, const uint8_t len)
{
    msg_tx.type = COM_PROTO_MSG_TYPE_TELEMETRY;
    msg_tx.payload_len = len;
    memcpy(msg_tx.p_payload, data, len);
    const uint16_t _len = COM_PROTO_CreateMsg(&msg_tx, tx_buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
    SendPacket(tx_buffer, _len);
}

static void _tx_complete_cb()
{
    tx_complete = true;
}
static void _rx_complete_cb()
{
    
}

static void HandleNewSticks(float* buffer, uint8_t len)
{
 STICK_HandleNewInput(buffer, len);
}

static void HandleNewSettings(uint8_t* buffer, uint8_t len)
{

}
static void HandleNewRequest(uint8_t *buffer, uint8_t len)
{
 COM_PROTO_REQUEST_TYPE_e type = buffer[0];
 switch (type)
 {
 case COM_PROTO_REQUEST_TYPE_LIMITS:
        break;

 case COM_PROTO_REQUEST_TYPE_CONTROL_MATRIX:
    {
        float* ptr = CL_GetActuatorsMatrix();
        msg_tx.type = COM_PROTO_MSG_TYPE_REQUEST;
        memcpy(msg_tx.p_payload + 1, ptr, sizeof(float)*48);
        msg_tx.p_payload[0] = COM_PROTO_REQUEST_TYPE_CONTROL_MATRIX;
        const uint16_t _len = COM_PROTO_CreateMsg(&msg_tx, tx_buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
        SendPacket(tx_buffer, _len);
    }
        break;
 case COM_PROTO_REQUEST_TYPE_PID:
        break;
 case COM_PROTO_REQUEST_TYPE_TASK_FREQUENCY:
        break;

 default:
        break;
 }
}

static void HandleNewMsg()
{
switch (msg_rx.type)
{
case COM_PROTO_MSG_TYPE_STICKS:
    HandleNewSticks((float*)msg_rx.p_payload, msg_rx.payload_len/sizeof(float));
    break;

case COM_PROTO_MSG_TYPE_SERVICE:
    HandleNewSettings((uint8_t*)msg_rx.p_payload, msg_rx.payload_len*sizeof(float));
    break;

case COM_PROTO_MSG_TYPE_REQUEST:
    HandleNewRequest((uint8_t*)msg_rx.p_payload, msg_rx.payload_len*sizeof(float));
    break;
default:
    break;
}
}
