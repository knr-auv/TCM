#include "stm32f4xx.h"
#include "setup.h"
#include "scheduler/scheduler.h"
#include "tasks/tasks.h"
#include "Sensors/analog_sensors.h"
#include "operation/IMU.h"
#include "operation/ControlLoop.h"
#include "communication/CommunicationHandler.h"
#include "IO/LED.h"
#include "drivers/I2C2.h"
#include "Sensors/MS5837-30BA/depth_sensor.h"
#include "Config/config.h"
#include "Flash/flash.h"

void init(void)
{
    initSystem();
    FLASH_Init();
    CONFIG_SaveToFlash();
    CONFIG_LoadFromFlash();
    
    ANALOG_Init();
    IMU_Init();
    CL_Init();
    //init_depth_sensor();

    COMHANDLER_Init();
    initScheduler();
    initTasks();
    ANALOG_beginConversion();
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