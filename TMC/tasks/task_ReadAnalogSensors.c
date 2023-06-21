#include "time/time.h"
#include "Sensors/analog_sensors.h"


void TASK_read_analog_sensors(timeUs_t t)
{
	ANALOG_beginConversion();
	ANALOG_ProcesRAW();
}