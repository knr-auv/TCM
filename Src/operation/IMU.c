#include "IMU.h"
#include "Sensors/UM7/UM7.h"
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
    return UM7_Init();
    
}
bool IMU_NewData()
{
    return UM7_NewData();
}