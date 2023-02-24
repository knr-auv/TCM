#pragma once
#include "scheduler/scheduler.h"

void initTasks(void);
extern task_t tasks[];

void TASK_Telemetry_Init();
void TASK_Telemetry(timeUs_t t);
void TASK_read_analog_sensors(timeUs_t t);