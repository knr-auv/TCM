#pragma once

#include <stdbool.h>
#include "time/time.h"


timeUs_t COMHANDLER_TimeSinceLastUpdate();

bool COMHANDLER_CheckFun(timeUs_t currentTime, timeUs_t deltaTime);
void COMHANDLER_Init();
void COMHANDLER_Task(timeUs_t t);
void COMHANDLER_SendTelemetry(const uint8_t *data, const uint8_t len);