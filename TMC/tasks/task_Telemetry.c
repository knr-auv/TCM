#include <string.h>
#include "time/time.h"
#include "communication/tx_handler.h"
#include "Variables/variables.h"
#include "helpers/CommunicationProtocol/communication_protocol.h"
static uint8_t _buffer[sizeof(float)*VAR_SYS_COUNT + 1];
void TASK_Telemetry(timeUs_t t)
{
    _buffer[0] = MSG_TYPE_FROM_OKON_TELEMETRY;
    memcpy(_buffer + 1, (uint8_t*)VAR_GetSysFloatPtr(VAR_SYS_ATTITUDE_Q_r),VAR_SYS_COUNT*sizeof(float));
 TX_Handler_SendData((uint8_t*)VAR_GetSysFloatPtr(VAR_SYS_ATTITUDE_Q_r), VAR_SYS_COUNT*sizeof(float)+1);
}