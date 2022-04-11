#include "IMU.h"
#include "Sensors/UM7/UM7.h"
#include "Variables/variables.h"
quaternion_t* IMU_GetOrientationQ()
{
    return UM7_GetOrientationPointer();
}

float* IMU_GetAngVelPointer()
{
    return UM7_GetAngVelPointer();
}
bool IMU_Init()
{
    quaternion_t* temp = IMU_GetOrientationQ();
    VAR_SetSysFloat(&temp->r, VAR_ATTITUDE_Q_r);
    VAR_SetSysFloat(&temp->i, VAR_ATTITUDE_Q_i);
    VAR_SetSysFloat(&temp->j, VAR_ATTITUDE_Q_j);
    VAR_SetSysFloat(&temp->k, VAR_ATTITUDE_Q_k);
    float* t =  IMU_GetAngVelPointer();
    VAR_SetSysFloat(&t[0], VAR_GYRO_r);
    VAR_SetSysFloat(&t[1], VAR_GYRO_p);
    VAR_SetSysFloat(&t[2], VAR_GYRO_y);
    
    return UM7_Init();   
}
bool IMU_NewData()
{
    return UM7_NewData();
}