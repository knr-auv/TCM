#pragma once
#include "stm32f4xx.h"
#include <stdint.h>
#define ACTUATORS_COUNT 8



void ACTUATORS_IronManFan(uint16_t pwm);
void setActuators(uint16_t* out);
void setActuator(uint32_t actuator, uint32_t value);
void initActuators();
void deinitActuators();
void resetActuators_pwm_t(uint16_t *out);
void ACTUATORS_Disable();
void ACTUATORS_Enable();
float ACTUATORS_map(float x, float in_min, float in_max);