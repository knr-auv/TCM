#include "variables.h"
#include "IO/Sticks.h"

void register_variables()
{
    float* ptr = STICK_GetSticks();
    for(uint8_t i =VAR_SYS_STICK_INPUT_1; i<=VAR_SYS_STICK_INPUT_16;i++)
        VAR_SetSysFloat(ptr++, i);
}