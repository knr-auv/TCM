#include "setup.h"
#include "scheduler/scheduler.h"
#include "tasks/tasks.h"
#include "Sensors/analog_sensors.h"
#include "operation/IMU.h"
#include "operation/ControlLoop.h"
#include "IO/LED.h"
#include "Config/memory.h"
void init(void)
{
    initSystem();
    ANALOG_Enable();
    IMU_Init();
    CL_Init();
    
    initScheduler();
    initTasks();
    MEM_Init();
}

int main(void)
{
    init();
    LED_Red_Set(false);

    if(MEM_Init())
        LED_Red_Set(true);
     //MEM_SaveSettings();   
    MEM_LoadSettings();
    while(1)
    {
        scheduler();
    }
}