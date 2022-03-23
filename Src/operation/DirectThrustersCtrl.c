
#include <stdbool.h>
#include "time/time.h"
#include "ControlLoop.h"
#include "IO/thrusters.h"
#include "helpers/common.h"
#include "scheduler/scheduler.h"
#define TIMEOUT 500000

timeUs_t lastTimeDirect = 0;
timeUs_t lastTimeMatrix = 0;

float Motors[8];
float Control_out[5];

bool DTCTRL_Timeout()
{
    timeUs_t lastTime;
    if(lastTimeDirect<lastTimeMatrix)
    {
        lastTime = lastTimeMatrix;
    }
    else
    {
        lastTime = lastTimeDirect;
    }
    timeUs_t out = micros()-lastTime;
    if((micros()-lastTime)>=TIMEOUT)
        return true;
    else return false;
}

float*  DTCTRL_GetValues()
{
    return Motors;
}


void DTCTRL_HandleNewDirectMatrixThrustersValues(float* values)
{
    lastTimeMatrix = micros();
    for(uint8_t i =0; i<5;i++)
        Control_out[i] = values[i];
}

void DTCTRL_HandlNewDirectThrustersValues(float* values)
{
    lastTimeDirect = micros();
    for(uint8_t i =0; i< 8;i++)
        Motors[i] = values[i];
}

void DTCTRL_Enable()
{
    lastTimeDirect = micros();
    lastTimeMatrix = micros();
    THRUSTERS_Enable();
}
void DTCTRL_Disable()
{
    THRUSTERS_Disable();
}

void DTCTRL_Task(timeUs_t t)
{
    if(DTCTRL_Timeout())
    {
        DTCTRL_Disable();
        enableTask(TASK_DIRECT_MOTORS_CTRL, false);
    }
    if(lastTimeDirect<lastTimeMatrix)
    {
        //perform matrix
    float* mat = CL_GetThrustersMatrix();
    COMMON_mat_vec_mul(mat, Control_out, Motors, THRUSTERS_COUNT, 5);
    COMMON_linear_saturation(Motors,THRUSTERS_COUNT, 1.f);
    }
    else
    {
        //perform direct
    }


    uint16_t out[THRUSTERS_COUNT];
    for(uint8_t i =0; i<THRUSTERS_COUNT;i++)
        out[i] = THRUSTERS_map(Motors[i], -1.f, 1.f);
    setThrusters(out);
}