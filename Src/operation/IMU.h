#pragma once
#include <stdbool.h>
#include "helpers/quaternions.h"

quaternion_t *IMU_GetOrientationQ();
float *IMU_GetAngVelPointer();
bool IMU_Init();
bool IMU_NewData();