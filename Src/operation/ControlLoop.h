#pragma once
#include "IO/thrusters.h"
#include "time/time.h"

typedef enum 
{
    CL_STATUS_ARMED,
    CL_STATUS_DISARMED,
    CL_STATUS_DIRECT
}cl_status_e;

typedef enum
{
    CL_MODE_ACRO,
    CL_MODE_STABLE,
    CL_MODE_COUNT
}cl_mode_e;

void CL_Init();
float* CL_GetMotorOutputs();
void CL_TaskFun(timeUs_t t);

void CL_Arm();
void CL_Disarm();
void CL_SetDirectMode();

void CL_SetMode(cl_mode_e mode);

void CL_SerializePIDs(uint8_t* buffer, uint16_t* len);
void CL_LoadPIDs(uint8_t* buffer, uint16_t len);