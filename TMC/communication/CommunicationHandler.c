#include <stdbool.h>
#include <string.h>
#include "Config/config.h"
#include "time/time.h"
#include "drivers/USART.h"
#include "protocol.h"
#include "IO/Sticks.h"
#include "operation/ControlLoop.h"
#include "scheduler/scheduler.h"
#include "tx_handler.h"


static void _rx_complete_cb();
static void HandleNewMsg();

static USART_t *port;
static COM_PROTO_msg_t msg_tx, msg_rx;

static uint8_t rx_buffer[CONFIG_COMM_HANDLER_BUFFER_LEN];


static bool receiving;
static uint16_t received_bytes;
static timeUs_t last_update;


static void SendPacket(uint8_t *buffer, uint16_t len)
{
    TX_SendData(buffer, len);
}

timeUs_t COMHANDLER_TimeSinceLastUpdate()
{
    return micros() - last_update;
}
void COMHANDLER_Init()
{
    port = USART_GetUSART(CONFIG_COMM_HANDLER_USART);
    port->RXCompleteCallback = _rx_complete_cb;
    TX_Init();
}

bool COMHANDLER_CheckFun(timeUs_t currentTime, timeUs_t deltaTime)
{
    if(!TX_CheckEmpty())
        return true;
    return (!receiving);
}

void COMHANDLER_Task(timeUs_t t)
{
    if (!TX_CheckEmpty())
    {
        TX_Task();
        return;
    }

    if (!receiving)
    {
        received_bytes = port->GetReceivedBytes();
        if (received_bytes > 0)
        {
            for (uint16_t i = 0; i < received_bytes; i++)
                if (COM_PROTO_ParseByte(rx_buffer[i], &msg_rx))
                {
                    last_update = micros();
                    HandleNewMsg();
                }
        }

        receiving = true;
        port->ReceiveDMA(rx_buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
    }
}

void COMHANDLER_SendTelemetry(const uint8_t *data, const uint8_t len)
{
    msg_tx.type = COM_PROTO_MSG_TYPE_TELEMETRY;
    msg_tx.payload_len = len;
    memcpy(msg_tx.p_payload, data, len);
    uint8_t* buffer = TX_GetFreeBuffer();
    if(buffer==NULL)
        return;
    const uint16_t _len = COM_PROTO_CreateMsg(&msg_tx, buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
    SendPacket(buffer, _len);
}

static void _rx_complete_cb()
{
    receiving = false;
}

static void HandleNewSticks(float *buffer, uint8_t len)
{
    STICK_HandleNewInput(buffer, len);
}

static void HandleNewSettings(uint8_t *buffer, uint8_t len)
{
    COM_PROTO_SERVICE_TYPE_e type = buffer[0];
    switch (type)
    {
    case COM_PROTO_SERVICE_TYPE_CONTROL_MATRIX:
    {
        CL_SetActuatorMatrix((float *)(buffer + 1), (len - 1) >> 2);
        break;
    }
    case COM_PROTO_SERVICE_TYPE_LIMITS:
    {
        if (len != sizeof(LIMITS_t) + 1)
            return;
        LIMITS_t lim;
        memcpy(&lim, buffer + 1, sizeof(LIMITS_t));
        CL_SetLimits(lim);
        break;
    }
    case COM_PROTO_SERVICE_TYPE_PID:
    {
        CL_SetPID((float *)(buffer + 1), (len - 1) >> 2);
        break;
    }
    case COM_PROTO_SERVICE_TYPE_TASK_FREQUENCY:
    {

        break;
    }
    default:
        break;
    }
}
static void HandleNewRequest(uint8_t *buffer, uint8_t len)
{
    COM_PROTO_REQUEST_TYPE_e type = buffer[0];
    switch (type)
    {
    case COM_PROTO_REQUEST_TYPE_LIMITS:
    {
        LIMITS_t *ptr = CL_GetLimits();
        msg_tx.type = COM_PROTO_MSG_TYPE_REQUEST;
        memcpy(msg_tx.p_payload + 1, ptr, sizeof(LIMITS_t));
        msg_tx.p_payload[0] = COM_PROTO_REQUEST_TYPE_LIMITS;
        msg_tx.payload_len = sizeof(LIMITS_t) + 1;
        uint8_t* _buffer = TX_GetFreeBuffer();
        if(_buffer==NULL)
            return;
        const uint16_t _len = COM_PROTO_CreateMsg(&msg_tx, _buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
        SendPacket(_buffer, _len);
    }
    break;

    case COM_PROTO_REQUEST_TYPE_CONTROL_MATRIX:
    {
        float *ptr = CL_GetActuatorsMatrix();
        msg_tx.type = COM_PROTO_MSG_TYPE_REQUEST;
        memcpy(msg_tx.p_payload + 1, ptr, sizeof(float) * 48);
        msg_tx.p_payload[0] = COM_PROTO_REQUEST_TYPE_CONTROL_MATRIX;
        msg_tx.payload_len = sizeof(float) * 48 + 1;
        uint8_t* _buffer = TX_GetFreeBuffer();
        if(_buffer==NULL)
            return;
        const uint16_t _len = COM_PROTO_CreateMsg(&msg_tx, _buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
        SendPacket(_buffer, _len);
    }
    break;
    case COM_PROTO_REQUEST_TYPE_PID:
    {

        CONFIG_PID_Container_t pid = CL_GetPID();
        msg_tx.type = COM_PROTO_MSG_TYPE_REQUEST;

        memcpy(msg_tx.p_payload + 1, &pid, sizeof(CONFIG_PID_Container_t));
        msg_tx.p_payload[0] = COM_PROTO_REQUEST_TYPE_PID;
        msg_tx.payload_len = sizeof(CONFIG_PID_Container_t) + 1;
        uint8_t* _buffer = TX_GetFreeBuffer();
        if (_buffer == NULL)
            return;
        const uint16_t _len = COM_PROTO_CreateMsg(&msg_tx, _buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
        SendPacket(_buffer, _len);
    }
    break;
    case COM_PROTO_REQUEST_TYPE_TASK_FREQUENCY:
    {
        uint16_t *ptr = (uint16_t *)(msg_tx.p_payload + 1);
        for (uint8_t i = 0; i < TASK_COUNT; i++)
        {
            uint16_t freq = 1E6 / tasks[i].desiredPeriod;
            *ptr++ = freq;
        }
        msg_tx.type = COM_PROTO_MSG_TYPE_REQUEST;
        msg_tx.p_payload[0] = COM_PROTO_REQUEST_TYPE_TASK_FREQUENCY;
        msg_tx.payload_len = TASK_COUNT * sizeof(uint16_t) + 1;
        uint8_t* _buffer = TX_GetFreeBuffer();
        if (buffer == NULL)
            return;
        const uint16_t _len = COM_PROTO_CreateMsg(&msg_tx, _buffer, CONFIG_COMM_HANDLER_BUFFER_LEN);
        SendPacket(_buffer, _len);
    }
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
        HandleNewSticks((float *)msg_rx.p_payload, msg_rx.payload_len / sizeof(float));
        break;

    case COM_PROTO_MSG_TYPE_SERVICE:
        HandleNewSettings((uint8_t *)msg_rx.p_payload, msg_rx.payload_len);
        break;

    case COM_PROTO_MSG_TYPE_REQUEST:
        HandleNewRequest((uint8_t *)msg_rx.p_payload, msg_rx.payload_len);
        break;
    default:
        break;
    }
}
