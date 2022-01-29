#include "setup.h"
#include "scheduler/scheduler.h"
#include "tasks/tasks.h"
#include "Sensors/analog_sensors.h"
#include "flight/IMU.h"
#include "flight/PID.h"
#include "drivers/USART1.h"

void init(void)
{
    initSystem();
    ANALOG_Enable();
    IMU_Init();
    PID_Init();
    
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