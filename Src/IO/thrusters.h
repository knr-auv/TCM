#pragma once
#include "stm32f4xx.h"
#include <stdint.h>
#define THRUSTERS_COUNT 8




void setThrusters(uint16_t* out);
void setThruster(uint32_t thruster, uint32_t value);
void initThrusters();
void deinitThrusters();
void resetThrusters_pwm_t(uint16_t *out);
void THRUSTERS_Disable();
void THRUSTERS_Enable();
float THRUSTERS_map(float x, float in_min, float in_max);