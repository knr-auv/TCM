#ifndef UM7_H
#define UM7_H
#include <stdbool.h>
#include "Common/quaternions.h"

bool UM7_Init(void);
bool UM7_NewData(void);
float *UM7_GetAngVelPointer(void);
quaternion_t *UM7_GetOrientationPointer(void);
#endif