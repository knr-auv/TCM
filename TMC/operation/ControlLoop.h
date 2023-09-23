#pragma once
#include "IO/actuators.h"
#include "time/time.h"
#include "Common/PID.h"
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

void CL_GetPID(PID_t* roll,float* roll_gain, PID_t* pitch, float* pitch_gain, PID_t* yaw, float* yaw_gain, PID_t* vertical);
void CL_SetPID(PID_t* roll,float roll_gain, PID_t* pitch, float pitch_gain, PID_t* yaw, float yaw_gain, PID_t* vertical);