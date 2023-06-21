#pragma once


typedef struct
{
    float P;
    float I;
    float D;
    float ITerm;
    float lastError;
    float windup;
    float output;
} PID_t;


void PID_update(PID_t *pid, float error, float dt);
void PID_reset(PID_t *pid);

void PID_initialize(PID_t *pid, float P, float I, float D, float windup);