#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    MSG_SERVICE_ENTER,
    MSG_SERVICE_REBOOT,
    MSG_SERVICE_UPDATE_CL_MATRIX,
    MSG_SERVICE_ENABLE_DIRECT_MOTORS_CTRL,
    MSG_SERVICE_DISABLE_DIRECT_MOTORS_CTRL,
    MSG_SERVICE_DIRECT_THRUSTERS_CTRL,
    MSG_SERVICE_DIRECT_MATRIX_THRUSTERS_CTRL,
    MSG_SERVICE_SAVE_SETTINGS,
    MSG_SERVICE_LOAD_SETTINGS,
    MSG_SERVICE_NEW_PIDS
}COMPROTO_msg_service_spec_e;

typedef enum{
    MSG_OKON_REQUEST_PID,
    MSG_OKON_REQUEST_CL_MATRIX
}COMPROTO_msg_request_spec_e;

typedef enum{
    MSG_TYPE_REQUEST,
    MSG_TYPE_SERVICE,
    MSG_TYPE_STICKS,
    MSG_TYPE_FROM_OKON_PID,
    MSG_TYPE_FROM_OKON_CL_MATRIX,
    MSG_TYPE_FROM_OKON_HEART_BEAT,
    MSG_TYPE_FROM_OKON_SERVICE_CONFIRM,
    MSG_TYPE_FROM_OKON_TELEMETRY,
    MSG_TYPE_COUNT
}COMPROTO_msg_type_e;

typedef struct
{
    COMPROTO_msg_type_e msg_type;
    uint8_t* data;
    uint16_t len;
    bool valid;
}COMPROTO_msg_info_t;

typedef struct
{
    COMPROTO_msg_type_e type;
    uint16_t user_data_len;
    uint8_t* user_data;
    uint8_t* tx_buffer;
    uint16_t tx_buffer_len;
    bool created;
}COMPROTO_msg_t;

void COMPROTO_ParseMsg(uint8_t* user_input, uint16_t len, COMPROTO_msg_info_t* msg_struct);
int COMPROTO_ParseHeader(uint8_t* data);
void COMPROTO_CreateMsg(COMPROTO_msg_t* msg);
