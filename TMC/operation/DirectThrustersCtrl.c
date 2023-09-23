
#include <stdbool.h>
#include "time/time.h"
#include "ControlLoop.h"
#include "IO/actuators.h"
#include "Common/common.h"
#include "scheduler/scheduler.h"
#define TIMEOUT 1000000

timeUs_t lastTimeDirect = 0;
timeUs_t lastTimeMatrix = 0;

static float Motors[8];
static float Control_out[5];

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
    if((micros()-lastTime)>=TIMEOUT)
        return true;
    else return false;
}

float*  DTCTRL_GetValues()
{
    return Motors;
}


void DTCTRL_HandleNewDirectMatrixActuatorsValues(float* values)
{
    lastTimeMatrix = micros();
    for(uint8_t i =0; i<5;i++)
        Control_out[i] = values[i];
}

void DTCTRL_HandlNewDirectActuatorsValues(float* values)
{
    lastTimeDirect = micros();
    for(uint8_t i =0; i< 8;i++)
        Motors[i] = values[i];
}

void DTCTRL_Enable()
{
    lastTimeDirect = micros();
    lastTimeMatrix = micros();
    ACTUATORS_Enable();
}
void DTCTRL_Disable()
{
    ACTUATORS_Disable();
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
    float* mat = CL_GetActuatorsMatrix();
    COMMON_mat_vec_mul(mat, Control_out, Motors, ACTUATORS_COUNT, 5);
    COMMON_linear_saturation(Motors,ACTUATORS_COUNT, 1.f);
    }
    else
    {
        //perform direct
    }


    uint16_t out[ACTUATORS_COUNT];
    for(uint8_t i =0; i<ACTUATORS_COUNT;i++)
        out[i] = ACTUATORS_map(Motors[i], -1.f, 1.f);
    setActuators(out);
}