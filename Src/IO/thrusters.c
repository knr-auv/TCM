#include "thrusters.h"
#include "time/time.h"

#define T_INIT_VALUE 1500
#define THRUSTER_1  TIM1->CCR4
#define THRUSTER_2  TIM3->CCR4
#define THRUSTER_3  TIM1->CCR4
#define THRUSTER_4  TIM3->CCR3
#define THRUSTER_5  TIM1->CCR2
#define THRUSTER_6  TIM3->CCR2
#define THRUSTER_7  TIM1->CCR1
#define THRUSTER_8  TIM3->CCR1
void setThruster(uint32_t thruster, uint32_t value)
{
    thruster = value;
}
void resetThrusters_pwm_t(thrusters_pwm_t *out)
{
out->T1 = T_INIT_VALUE;
out->T2 = T_INIT_VALUE;
out->T3 = T_INIT_VALUE;
out->T4 = T_INIT_VALUE;
out->T5 = T_INIT_VALUE;
out->T6 = T_INIT_VALUE;
out->T7 = T_INIT_VALUE;
out->T8 = T_INIT_VALUE;
}

void setThrusters(thrusters_pwm_t *out)
{
    THRUSTER_1 = (uint32_t)out->T1;
    THRUSTER_2 = (uint32_t)out->T2;
    THRUSTER_3 = (uint32_t)out->T3;
    THRUSTER_4 = (uint32_t)out->T4;
    THRUSTER_5 = (uint32_t)out->T5;
    THRUSTER_6 = (uint32_t)out->T6;
    THRUSTER_7 = (uint32_t)out->T7;
    THRUSTER_8 = (uint32_t)out->T8;
}

void initThrusters()
{
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