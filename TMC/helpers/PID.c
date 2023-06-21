#include "PID.h"


void PID_update(PID_t *pid, float error, float dt)
{
    float DTerm = 0;

    pid->ITerm += dt * error;
    if (pid->ITerm > pid->windup)
        pid->ITerm = pid->windup;
    else if (pid->ITerm < -pid->windup)
        pid->ITerm = -pid->windup;

    if (dt > 0)
    {
        DTerm = (error - pid->lastError) / dt;
    }

    pid->output = error * pid->P + pid->ITerm * pid->I + DTerm * pid->D;
    pid->lastError = error;
}

void PID_reset(PID_t *pid)
{
    pid->lastError = 0;
    pid->ITerm = 0;
}

void PID_initialize(PID_t *pid, float P, float I, float D, float windup)
{
    PID_reset(pid);
    pid->P = P;
    pid->I = I;
    pid->D = D;
    pid->windup = windup;
}
