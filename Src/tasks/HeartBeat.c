#include "time/time.h"
#include "config.h"
#include "IO/LED.h"
#include "operation/CommunicationHandler.h"
void HB_Task(timeUs_t t)
{
    LED_HeartBeat_Toggle();
	#ifdef HEART_BEAT_PACKET
	COMHANDLER_SendHeartBeat();
	#endif
}