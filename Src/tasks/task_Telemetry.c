#include "time/time.h"
#include "communication/tx_handler.h"
#include "Variables/variables.h"

void TASK_Telemetry(timeUs_t t)
{
 TX_Handler_SendData((uint8_t*)VAR_GetSysFloatPtr(VAR_SYS_ATTITUDE_Q_r), VAR_SYS_COUNT*sizeof(float));
}