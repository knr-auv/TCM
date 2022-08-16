#include "stm32f4xx.h"
#include "setup.h"
#include "scheduler/scheduler.h"
#include "tasks/tasks.h"
#include "Sensors/analog_sensors.h"
#include "operation/IMU.h"
#include "operation/ControlLoop.h"
#include "operation/CommunicationHandler.h"
#include "IO/LED.h"
#include "Config/memory.h"
#include "drivers/I2C2.h"
#include "Sensors/MS5837-30BA/depth_sensor.h"

void init(void)
{
    initSystem();
    ANALOG_Init();
    IMU_Init();
    CL_Init();
    init_depth_sensor();
    MEM_Init();
    MEM_LoadSettings();
    COMHANDLER_Init();
    initScheduler();
    initTasks();
    ANALOG_beginConversion();
    //   MEM_SaveSettings();
}

void enable()
{
    ANALOG_Enable();
}

int main(void)
{
    init();
    enable();

    while (1)
    {
        scheduler();
    }
}