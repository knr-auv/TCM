/*
Important variables:
orientation quaternion
angular velocity

angular velocity
ref angular velocity

*/
#include <stdint.h>

typedef enum
{
    VAR_ATTITUDE_Q_r,
    VAR_ATTITUDE_Q_i,
    VAR_ATTITUDE_Q_j,
    VAR_ATTITUDE_Q_k,
    VAR_ATTITUDE_E_r,
    VAR_ATTITUDE_E_p,
    VAR_ATTITUDE_E_y,
    VAR_GYRO_r,
    VAR_GYRO_p,
    VAR_GYRO_y,

    VAR_SENSOR_ONBOARD_TEMP,
    VAR_SENSOR_CPU_TEMP,
    VAR_SENSOR_ONBOARD_HUMMIDITY,

    VAR_CL_GYRO_REF_r,
    VAR_CL_GYRO_REF_p,
    VAR_CL_GYRO_REF_y,
    VAR_CL_ATTITUDE_REF_Q_r,
    VAR_CL_ATTITUDE_REF_Q_i,
    VAR_CL_ATTITUDE_REF_Q_j,
    VAR_CL_ATTITUDE_REF_Q_k,
    VAR_CL_THRUSTER1_OUT,
    VAR_CL_THRUSTER2_OUT,
    VAR_CL_THRUSTER3_OUT,
    VAR_CL_THRUSTER4_OUT,
    VAR_CL_THRUSTER5_OUT,
    VAR_CL_THRUSTER6_OUT,
    VAR_CL_THRUSTER7_OUT,
    VAR_CL_THRUSTER8_OUT,
    
    VAR_SYS_COUNT
}VAR_sys_float_e;

void VAR_SetSysFloat(float *value, VAR_sys_float_e name);
float *VAR_GetSysFloatPtr(VAR_sys_float_e name);
float VAR_GetSysFloat(VAR_sys_float_e name);

