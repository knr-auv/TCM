/*
Here are the variables that are calculated by the system itself. The purpose is to provide global acces to them. 
Important variables:
orientation quaternion
angular velocity

angular velocity
ref angular velocity

*/
#include <stdint.h>

typedef enum
{
    VAR_SYS_ATTITUDE_Q_r,
    VAR_SYS_ATTITUDE_Q_i,
    VAR_SYS_ATTITUDE_Q_j,
    VAR_SYS_ATTITUDE_Q_k,
    
    VAR_SYS_ATTITUDE_E_r,
    VAR_SYS_ATTITUDE_E_p,
    VAR_SYS_ATTITUDE_E_y,

    VAR_SYS_GYRO_r,
    VAR_SYS_GYRO_p,
    VAR_SYS_GYRO_y,

    VAR_SYS_BATTERY_LEVEL,

    VAR_SYS_SENSOR_ONBOARD_TEMP,
    VAR_SYS_SENSOR_CPU_TEMP,
    VAR_SYS_SENSOR_ONBOARD_HUMMIDITY,

    VAR_SYS_DESIRED_GYRO_r,
    VAR_SYS_DESIRED_GYRO_p,
    VAR_SYS_DESIRED_GYRO_y,

    VAR_SYS_DESIRED_ATTITUDE_E_r,
    VAR_SYS_DESIRED_ATTITUDE_E_p,
    VAR_SYS_DESIRED_ATTITUDE_E_y,

    VAR_SYS_DESIRED_ATTITUDE_Q_r,
    VAR_SYS_DESIRED_ATTITUDE_Q_i,
    VAR_SYS_DESIRED_ATTITUDE_Q_j,
    VAR_SYS_DESIRED_ATTITUDE_Q_k,

    VAR_SYS_ACTUATOR_OUT_1,
    VAR_SYS_ACTUATOR_OUT_2,
    VAR_SYS_ACTUATOR_OUT_3,
    VAR_SYS_ACTUATOR_OUT_4,
    VAR_SYS_ACTUATOR_OUT_5,
    VAR_SYS_ACTUATOR_OUT_6,
    VAR_SYS_ACTUATOR_OUT_7,
    VAR_SYS_ACTUATOR_OUT_8,

    VAR_SYS_STICK_INPUT_1,
    VAR_SYS_STICK_INPUT_2,
    VAR_SYS_STICK_INPUT_3,
    VAR_SYS_STICK_INPUT_4,
    VAR_SYS_STICK_INPUT_5,
    VAR_SYS_STICK_INPUT_6,
    VAR_SYS_STICK_INPUT_7,
    VAR_SYS_STICK_INPUT_8,
    VAR_SYS_STICK_INPUT_9,
    VAR_SYS_STICK_INPUT_10,
    VAR_SYS_STICK_INPUT_11,
    VAR_SYS_STICK_INPUT_12,
    VAR_SYS_STICK_INPUT_13,
    VAR_SYS_STICK_INPUT_14,
    VAR_SYS_STICK_INPUT_15,
    VAR_SYS_STICK_INPUT_16,
    
    VAR_SYS_COUNT
}VAR_sys_float_e;


void VAR_SetSysFloat(float *value, VAR_sys_float_e name);
float *VAR_GetSysFloatPtr(VAR_sys_float_e name);
float VAR_GetSysFloat(VAR_sys_float_e name);
void VAR_Init();
