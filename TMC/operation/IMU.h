#pragma once
#include <stdbool.h>
#include "Common/quaternions.h"

quaternion_t *IMU_GetOrientationQ();
float *IMU_GetAngVelPointer();
bool IMU_Init();
bool IMU_NewData();