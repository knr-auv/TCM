#include <stdint.h>
#include <string.h>
float stick_input[16];


void STICK_HandleNewInput(float* data, uint16_t len)
{
    if(len>16)
    len  = 16;
    memcpy(stick_input, data, sizeof(float)*len);
}

float* STICK_GetSticks()
{
    return stick_input;
}