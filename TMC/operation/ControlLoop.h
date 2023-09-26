#pragma once
#include "IO/actuators.h"
#include "time/time.h"
#include "Common/PID.h"
#include "Config/config.h"
typedef enum 
{
    CL_STATUS_ARMED,
    CL_STATUS_DISARMED
}cl_status_e;

typedef enum
{
    CL_MODE_ACRO,
    CL_MODE_STABLE,
    CL_MODE_COUNT
}cl_mode_e;

void CL_Init();

void CL_TaskFun(timeUs_t t);

void CL_Arm();
void CL_Disarm();


cl_status_e CL_GetStatus();
cl_mode_e CL_GetMode();


float* CL_GetActuatorsMatrix();
void CL_SetActuatorMatrix(float* data, uint16_t len);

CONFIG_PID_Container_t CL_GetPID(void);

void CL_SetPID(float* buffer, uint8_t size);

void CL_SetLimits(LIMITS_t Limits);

LIMITS_t* CL_GetLimits(void);