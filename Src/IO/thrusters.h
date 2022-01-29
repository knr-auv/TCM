#pragma once
#include "stm32f4xx.h"




typedef struct{
    uint16_t T1;
    uint16_t T2;
    uint16_t T3;
    uint16_t T4;
    uint16_t T5;
    uint16_t T6;
    uint16_t T7;
    uint16_t T8;
}thrusters_pwm_t;

void setThrusters(thrusters_pwm_t* out);
void setThruster(uint32_t thruster, uint32_t value);
void initThrusters();
void deinitThrusters();
void resetThrusters_pwm_t(thrusters_pwm_t *out);