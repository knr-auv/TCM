#include <string.h>
#include <stdint.h>
#include "Flash/flash.h"
#include "config.h"
#include "default_config.h"


// void initControlActuatorsMatrix()
// {
//     float su[] =
//     {
//         0.f,    0.f,   -1.f,    0.f,    0.f,
//         1.f,   -1.f,    0.f,    0.f,    0.f,
//         1.f,    1.f,    0.f,    0.f,    0.f,
//         0.f,    0.f,   -1.f,    0.f,    0.f,
//         0.f,    0.f,    1.f,    0.f,    0.f,
//        -1.f,    1.f,    0.f,    0.f,    0.f,
//        -1.f,   -1.f,    0.f,    0.f,    0.f,
//         0.f,    0.f,    1.f,    0.f,    0.f
//     };
//     memcpy(control_actuators_matrix, su,CONTROL_OUTPUTS*ACTUATORS_COUNT*sizeof(float));
// }

union Config_u
{
    CONFIG_Container_t container;
    uint8_t buffer[sizeof(CONFIG_Container_t)];
};
 
static union Config_u config;

void CONFIG_LoadDefault()
{
    memset(config.buffer, sizeof(config), 0);

    config.container.task_frequency[TASK_SYSTEM] = CONFIG_TASK_SYSTEM_PERIOD_HZ;
    config.container.task_frequency[TASK_READ_ANALOG_SENSORS] = CONFIG_TASK_READ_ANALOG_SENSORS_HZ;
    config.container.task_frequency[TASK_TELEMETRY] = CONFIG_TASK_TELEMETRY_HZ;
    config.container.task_frequency[TASK_CONTROL_LOOP] = CONFIG_TASK_PID_HZ;
    config.container.task_frequency[TASK_COMM_HANDLER] = CONFIG_TASK_COMM_HANDLER_HZ;
    config.container.task_frequency[TASK_LED_TEST] = CONFIG_TASK_TELEMETRY_HZ;
    config.container.task_frequency[TASK_AUTOMATIONS] = CONFIG_TASK_AUTOMATIONS_HZ;
    config.container.task_frequency[TASK_DIRECT_MOTORS_CTRL] = CONFIG_TASK_TELEMETRY_HZ;
    config.container.task_frequency[TASK_HEART_BEAT] = CONFIG_TASK_TELEMETRY_HZ;
    config.container.task_frequency[TASK_READ_DEPTH_SENSOR] = CONFIG_TASK_TELEMETRY_HZ;

}

void CONFIG_LoadFromFlash()
{
    FLASH_LoadData(config.buffer,sizeof(CONFIG_Container_t), 4096);
}

void CONFIG_SaveToFlash()
{
    FLASH_SaveData(config.buffer,sizeof(CONFIG_Container_t), 4096);
}

CONFIG_Container_t* CONFIG_GetCurrentConfig()
{
    return &config.container;
}