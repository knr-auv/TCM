#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    MSG_OKON_SERVICE_ENTER,
    MSG_OKON_SERVICE_REBOOT,
    MSG_OKON_SERVICE_UPDATE_CL_MATRIX,
    MSG_OKON_SERVICE_ENABLE_DIRECT_MOTORS_CTRL,
    MSG_OKON_SERVICE_DISABLE_DIRECT_MOTORS_CTRL,
    MSG_OKON_SERVICE_DIRECT_THRUSTERS_CTRL,
    MSG_OKON_SERVICE_DIRECT_MATRIX_THRUSTERS_CTRL,
    MSG_OKON_SERVICE_SAVE_SETTINGS,
    MSG_OKON_SERVICE_LOAD_SETTINGS,
    MSG_OKON_SERVICE_NEW_PIDS

}COMPROTO_msg_okon_service_e;

typedef enum{
    MSG_OKON_REQUEST_PID,
    MSG_OKON_REQUEST_CL_MATRIX
}COMPROTO_msg_okon_request_e;

typedef enum{
    MSG_OKON_REQUEST,
    MSG_OKON_SERVICE,
    MSG_OKON_STICKS,
    MSG_OKON_MODE,
    MSG_OKON_CL_STATUS,
    MSG_OKON_TYPE_COUNT
}COMPROTO_msg_okon_e;

typedef enum{
MSG_FROM_OKON_PID,
MSG_FROM_OKON_CL_MATRIX,
MSG_FROM_OKON_HEART_BEAT,
MSG_FROM_OKON_SERVICE_CONFIRM
}COMPROTO_msg_from_okon_e;

typedef struct
{
    COMPROTO_msg_okon_e msg_type;
    uint16_t specific;
    uint8_t* data;
    uint16_t len;
    bool valid;
}COMPROTO_msg_info_t;

typedef struct
{
    COMPROTO_msg_from_okon_e type;
    uint16_t user_data_len;
    uint8_t* user_data;
    uint8_t* tx_buffer;
    uint16_t tx_buffer_len;
    bool created;
}COMPROTO_msg_from_okon_t;

void COMPROTO_ParseMsg(uint8_t* user_input, uint16_t len, COMPROTO_msg_info_t* msg_struct);
int COMPROTO_ParseHeader(uint8_t* data);
void COMPROTO_CreateMsg(COMPROTO_msg_from_okon_t* msg);
