#pragma once
#include "IO/thrusters.h"
#include "IO/controls.h"
#include "time/time.h"

void PID_Init();
thrusters_pwm_t* PID_GetMotorOutputs();
void PID_TaskFun(timeUs_t t);