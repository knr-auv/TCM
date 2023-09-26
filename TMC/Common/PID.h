#pragma once


typedef struct
{
    float P;
    float I;
    float D;
    float IMax;
    float ITerm;
    float lastError;
    float output;
} PID_t;


void PID_update(PID_t *pid, float error, float dt);
void PID_reset(PID_t *pid);

void PID_initialize(PID_t *pid, float P, float I, float D, float IMax);