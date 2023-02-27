#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "Config/config.h"
#include "time/time.h"
#include "helpers/CommunicationProtocol/communication_protocol.h"

#include "operation/DirectThrustersCtrl.h"
#include "operation/ControlLoop.h"
#include "IO/Sticks.h"
#include "operation/ServiceMode.h"

#include "rx_handler.h"
#include "tx_handler.h"

static uint8_t rx_buffer[CONFIG_COMM_HANDLER_BUFFER_LEN];
static uint8_t tx_buffer[CONFIG_COMM_HANDLER_BUFFER_LEN];

timeUs_t last_time = 0;
COMPROTO_msg_info_t msg;
COMPROTO_msg_t msg_from_okon;

static bool new_msg = false;

void HandleRequestMsg(COMPROTO_msg_info_t *msg);
void HandleSticksMsg(COMPROTO_msg_info_t *msg);
void HandeModeMsg(COMPROTO_msg_info_t *msg);
void HandleCLStatusMsg(COMPROTO_msg_info_t *msg);

timeUs_t COMHANDLER_TimeSinceLastUpdate()
{
    return micros() - last_time;
}

void COMHANDLER_Init()
{
    RX_Handler_init();
    TX_Handler_init();
    RX_Handler_receive();

    msg_from_okon.tx_buffer = tx_buffer;
}

bool COMHANDLER_CheckFun(timeUs_t currentTime, timeUs_t deltaTime)
{
    last_time = currentTime;
    return RX_Handler_CheckNewMessage();
}

void COMHANDLER_Task(timeUs_t t)
{
    new_msg = false;
    uint16_t received_bytes = RX_Handler_GetNewMsg(rx_buffer);

    COMPROTO_ParseMsg(rx_buffer, received_bytes, &msg);
    if (!msg.valid)
    {
        RX_Handler_receive();
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
    RX_Handler_receive();
}

void COMHANDLER_SendResponse(uint8_t *data, uint8_t len)
{
    TX_Handler_SendData(data, len);
}
void COMHANDLER_SendConfirmation(uint8_t data)
{
    msg_from_okon.user_data_len = 1;
    msg_from_okon.type = MSG_TYPE_FROM_OKON_SERVICE_CONFIRM;
    uint8_t buff[1];
    msg_from_okon.user_data = buff;
    msg_from_okon.user_data[0] = data;
    COMPROTO_CreateMsg(&msg_from_okon);
    COMHANDLER_SendResponse(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
}

void HandleRequestMsg(COMPROTO_msg_info_t *msg)
{
    if (CL_GetStatus() == CL_STATUS_ARMED)
    {
        if (msg->data[0] == MSG_OKON_REQUEST_PID)
        {
            msg_from_okon.user_data = CL_SerializePIDs(&msg_from_okon.user_data_len);
            msg_from_okon.type = MSG_TYPE_FROM_OKON_PID;
            COMPROTO_CreateMsg(&msg_from_okon);
            free(msg_from_okon.user_data);
            TX_Handler_SendData(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
        }
    }
    else
    {
        if (msg->data[0] == MSG_OKON_REQUEST_PID)
        {
            msg_from_okon.user_data = CL_SerializePIDs(&msg_from_okon.user_data_len);
            msg_from_okon.type = MSG_TYPE_FROM_OKON_PID;
            COMPROTO_CreateMsg(&msg_from_okon);
            free(msg_from_okon.user_data);

            TX_Handler_SendData(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
        }
        else if (msg->data[0] == MSG_OKON_REQUEST_CL_MATRIX)
        {
            msg_from_okon.user_data = CL_SerializeControlActuatorsMatrix(&msg_from_okon.user_data_len);
            msg_from_okon.type = MSG_TYPE_FROM_OKON_CL_MATRIX;
            COMPROTO_CreateMsg(&msg_from_okon);
            // free(msg_from_okon.user_data);

            TX_Handler_SendData(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
        }
    }
}
void HandleSticksMsg(COMPROTO_msg_info_t *msg)
{
    STICK_HandleNewInput((float *)(msg->data), msg->len / (sizeof(float)));
}

void COMHANDLER_SendHeartBeat()
{
    msg_from_okon.user_data = 0;
    msg_from_okon.user_data_len = 0;
    msg_from_okon.type = MSG_TYPE_FROM_OKON_HEART_BEAT;
    COMPROTO_CreateMsg(&msg_from_okon);
    TX_Handler_SendData(msg_from_okon.tx_buffer, msg_from_okon.tx_buffer_len);
}