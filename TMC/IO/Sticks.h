#pragma once
#include <stdint.h>
#include "time/time.h"
void STICK_ResetSticks();
void STICK_HandleNewInput(float* data, uint8_t len);
float* STICK_GetSticks();
timeUs_t STICK_TimeSinceLastUpdate();
