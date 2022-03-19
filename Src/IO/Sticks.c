#include <stdint.h>
#include <string.h>
#include "time/time.h"
float stick_input[16];

timeUs_t LastUpdated = 0;


void STICK_HandleNewInput(float* data, uint16_t len)
{
    if(len>16)
    len  = 16;
    memcpy(stick_input, data, sizeof(float)*len);
}

float* STICK_GetSticks()
{
    LastUpdated = micros();
    return stick_input;
}

timeUs_t STICK_TimeSinceLastUpdate()
{
    return micros()-LastUpdated;
}