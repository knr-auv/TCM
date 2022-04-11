#include "setup.h"
#include "scheduler/scheduler.h"
#include "tasks/tasks.h"
#include "Sensors/analog_sensors.h"
#include "operation/IMU.h"
#include "operation/ControlLoop.h"
#include "operation/CommunicationHandler.h"
#include "IO/LED.h"
#include "Config/memory.h"
void init(void)
{
    initSystem();
    ANALOG_Init();
    IMU_Init();
    CL_Init();

    MEM_Init();
    MEM_LoadSettings();
    COMHANDLER_Init();
    initScheduler();
    initTasks();
    ANALOG_beginConversion();
   // MEM_SaveSettings();
}

void enable()
{
    ANALOG_Enable();
}

int main(void)
{
    init();
    enable();
    while(1)
    {
        scheduler();
    }
}