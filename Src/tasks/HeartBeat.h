#pragma once

#include "time/time.h"
#include "config.h"
void HB_Task(timeUs_t t);
#ifdef HEART_BEAT_PACKET

void HB_DisablePacket();
void HB_EnablePacket();
#endif