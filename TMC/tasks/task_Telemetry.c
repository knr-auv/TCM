#include <string.h>
#include "time/time.h"
#include "Communication/CommunicationHandler.h"
#include "Variables/variables.h"


static uint8_t buffer[VAR_SYS_COUNT*sizeof(float)];

void TASK_Telemetry(timeUs_t t)
{
    float* ptr = (float*)buffer;
    for(int i =0; i<VAR_SYS_COUNT;i++)
    {
        ptr[i] = VAR_GetSysFloat(i);
    }
    COMHANDLER_SendTelemetry(buffer, VAR_SYS_COUNT * sizeof(float));
}