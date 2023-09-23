#include <string.h>
#include "time/time.h"
#include "Communication/CommunicationHandler.h"
#include "Variables/variables.h"

void TASK_Telemetry(timeUs_t t)
{
    COMHANDLER_SendTelemetry((uint8_t *)VAR_GetSysFloatPtr(VAR_SYS_ATTITUDE_Q_r), VAR_SYS_COUNT * sizeof(float));
}