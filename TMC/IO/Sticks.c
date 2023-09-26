#include <stdint.h>
#include <string.h>
#include "time/time.h"
static float stick_input[16];

timeUs_t LastUpdated = 0;


void STICK_HandleNewInput(float* data, uint8_t len)
{
    LastUpdated = micros();
    if(len>16)
    len  = 16;
    memcpy(stick_input, data, sizeof(float)*len);
}

float* STICK_GetSticks()
{
    return stick_input;
}

timeUs_t STICK_TimeSinceLastUpdate()
{
    return micros()-LastUpdated;
}
void STICK_ResetSticks()
{
    for(int i =0; i<16;i++)
    {
        stick_input[i]=0;
    }
}