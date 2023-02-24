#include "time/time.h"
#include "config.h"
#include "IO/LED.h"
#include "communication/CommunicationHandler.h"
#ifdef HEART_BEAT_PACKET
bool enable_packet = true;
void HB_DisablePacket()
{
	enable_packet = false;
}
void HB_EnablePacket()
{
	enable_packet = true;
}
#endif
void HB_Task(timeUs_t t)
{
    LED_HeartBeat_Toggle();
	#ifdef HEART_BEAT_PACKET
	if(enable_packet)
	COMHANDLER_SendHeartBeat();
	#endif
}