#pragma once

#include <stdbool.h>
#include <stdint.h>


typedef enum{
    MSG_STICKS,
    MSG_DIRECT_THRUSTERS_CTRL,
    MSG_MODE,
    MSG_CL_STATUS
}COMMPROTO_msg_e;

int COMPROTO_ParseMsg(uint8_t* user_input);

bool COMPROTO_NewStickValues();
bool COMPROTO_NewDirectThrusterValues();
bool COMPROTO_NewMode();
bool COMPROTO_NewCLStatus();

float* COMPROTO_GetDirectThrustersValues();
float* COMPROTO_GetStickValues();
uint8_t COMPROTO_GetMode();
uint8_t COMPROTO_GetCLStatus();
