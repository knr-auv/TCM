#pragma once

#include <stdbool.h>
#include "time/time.h"

void COMHANDLER_SendResponse(uint8_t* data, uint8_t len);
bool COMHANDLER_CheckFun(timeUs_t currentTime, timeUs_t deltaTime);
void COMHANDLER_Init();
void COMHANDLER_Task(timeUs_t t);

void COMHANDLER_SendConfirmation(uint8_t data);
timeUs_t COMHANDLER_TimeSinceLastUpdate();
void COMHANDLER_SendHeartBeat();



