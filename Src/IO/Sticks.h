#pragma once
#include <stdint.h>
void STICK_HandleNewInput(float* data, uint16_t len);
float* STICK_GetSticks();
timeUs_t STICK_TimeSinceLastUpdate();