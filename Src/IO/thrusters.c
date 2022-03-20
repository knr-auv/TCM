#include "thrusters.h"
#include "time/time.h"
#include "helpers/common.h"
#include <stdbool.h>

#define T_INIT_VALUE 1500.f
#define T_MAX_VALUE 2000.f
#define T_MIN_VALUE 1000.f
#define THRUSTER_1  TIM1->CCR4
#define THRUSTER_2  TIM3->CCR4
#define THRUSTER_3  TIM1->CCR4
#define THRUSTER_4  TIM3->CCR3
#define THRUSTER_5  TIM1->CCR2
#define THRUSTER_6  TIM3->CCR2
#define THRUSTER_7  TIM1->CCR1
#define THRUSTER_8  TIM3->CCR1

bool allow_update = false;
void setThruster(uint32_t thruster, uint32_t value)
{
    thruster = value;
}
void resetThrusters_pwm_t(uint16_t *out)
{
    for(uint8_t i =0; i<THRUSTERS_COUNT; i++)
    {
        out[i] = T_INIT_VALUE;
    }
}

void setThrusters(uint16_t *out)
{
    if(!allow_update)
        return;
    THRUSTER_1 = (uint32_t)out[0];
    THRUSTER_2 = (uint32_t)out[1];
    THRUSTER_3 = (uint32_t)out[2];
    THRUSTER_4 = (uint32_t)out[3];
    THRUSTER_5 = (uint32_t)out[4];
    THRUSTER_6 = (uint32_t)out[5];
    THRUSTER_7 = (uint32_t)out[6];
    THRUSTER_8 = (uint32_t)out[7];
}

void initThrusters()
{
    allow_update = true;
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->CR1 |= TIM_CR1_CEN;
    TIM3->CR1 |= TIM_CR1_CEN;
    THRUSTER_1 = T_INIT_VALUE;
    THRUSTER_2 = T_INIT_VALUE;
    THRUSTER_3 = T_INIT_VALUE;
    THRUSTER_4 = T_INIT_VALUE;
    THRUSTER_5 = T_INIT_VALUE;
    THRUSTER_6 = T_INIT_VALUE;
    THRUSTER_7 = T_INIT_VALUE;
    THRUSTER_8 = T_INIT_VALUE;
    DelayMs(500);
}

void deinitThrusters()
{
    allow_update=false;
THRUSTER_1 = T_INIT_VALUE;
THRUSTER_2 = T_INIT_VALUE;
THRUSTER_3 = T_INIT_VALUE;
THRUSTER_4 = T_INIT_VALUE;
THRUSTER_5 = T_INIT_VALUE;
THRUSTER_6 = T_INIT_VALUE;
THRUSTER_7 = T_INIT_VALUE;
THRUSTER_8 = T_INIT_VALUE;
TIM1->BDTR &= ~TIM_BDTR_MOE;
TIM1->CR1 &= ~TIM_CR1_CEN;
TIM3->CR1 &= ~TIM_CR1_CEN;
}

void THRUSTERS_Enable()
{
    allow_update = true;
}
void THRUSTERS_Disable()
{
allow_update=false;
THRUSTER_1 = T_INIT_VALUE;
THRUSTER_2 = T_INIT_VALUE;
THRUSTER_3 = T_INIT_VALUE;
THRUSTER_4 = T_INIT_VALUE;
THRUSTER_5 = T_INIT_VALUE;
THRUSTER_6 = T_INIT_VALUE;
THRUSTER_7 = T_INIT_VALUE;
THRUSTER_8 = T_INIT_VALUE;
}
float THRUSTERS_map(float x, float in_min, float in_max) {
  return (x - in_min) * (T_MAX_VALUE - T_MIN_VALUE) / (in_max - in_min) + T_MIN_VALUE;
}