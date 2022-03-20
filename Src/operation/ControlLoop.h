#pragma once
#include "IO/thrusters.h"
#include "time/time.h"

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
float* CL_GetMotorOutputs();
void CL_TaskFun(timeUs_t t);

void CL_Arm();
void CL_Disarm();


cl_status_e CL_GetStatus();
cl_mode_e CL_GetMode();

void CL_SetMode(cl_mode_e mode);
float* CL_GetThrustersMatrix();
void CL_LoadControlThrustersMatrix(uint8_t* buffer, uint16_t len);
void CL_SerializeControlThrustersMatrix(uint8_t **buffer, uint16_t* len);
void CL_SerializePIDs(uint8_t** buffer, uint16_t* len);
void CL_LoadPIDs(uint8_t* buffer, uint16_t len);