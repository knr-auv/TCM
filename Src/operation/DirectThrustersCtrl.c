
#include <stdbool.h>
#include "time/time.h"
#define TIMEOUT 4000

timeUs_t lastTime = 0;

float Motors[8];

bool DTCTRL_Timeout()
{
    if((micros()-lastTime)>=TIMEOUT)
        return true;
    else return false;
}
float*  DTCRTL_GetValues()
{
    return Motors;
}
void DTCRTL_HandlNewValues(float* values)
{
    for(uint8_t i =0; i< 8;i++)
        Motors[i] = values[i];
}