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
    ANALOG_Enable();
    IMU_Init();
    CL_Init();

    MEM_Init();
    MEM_LoadSettings();
    COMHANDLER_Init();
    initScheduler();
    initTasks();
    
}

int main(void)
{
    init();

    while(1)
    {
        scheduler();
    }
}