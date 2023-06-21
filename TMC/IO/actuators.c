#include "actuators.h"
#include "time/time.h"
#include "helpers/common.h"
#include "IO/LED.h"
#include <stdbool.h>

#define T_INIT_VALUE 1500.f
#define T_MAX_VALUE 1900.f
#define T_MIN_VALUE 1100.f

#define ACTUATOR_1  TIM1->CCR3
#define ACTUATOR_2  TIM3->CCR4
#define ACTUATOR_3  TIM1->CCR4
#define ACTUATOR_4  TIM3->CCR3
#define ACTUATOR_5  TIM1->CCR2
#define ACTUATOR_6  TIM3->CCR2
#define ACTUATOR_7  TIM1->CCR1
#define ACTUATOR_8  TIM3->CCR1

bool allow_update = false;
void setActuator(uint32_t actuator, uint32_t value)
{
    actuator = value;
}
void resetActuators_pwm_t(uint16_t *out)
{
    for(uint8_t i =0; i<ACTUATORS_COUNT; i++)
    {
        out[i] = T_INIT_VALUE;
    }
}

void setActuators(uint16_t *out)
{
    if(!allow_update)
         return;
    ACTUATOR_1 = (uint32_t)out[0];
    ACTUATOR_2 = (uint32_t)out[1];
    ACTUATOR_3 = (uint32_t)out[2];
    ACTUATOR_4 = (uint32_t)out[3];
    ACTUATOR_5 = (uint32_t)out[4];
    ACTUATOR_6 = (uint32_t)out[5];
    ACTUATOR_7 = (uint32_t)out[6];
    ACTUATOR_8 = (uint32_t)out[7];
}

void initActuators()
{
    allow_update = false;
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->CR1 |= TIM_CR1_CEN;
    TIM3->CR1 |= TIM_CR1_CEN;
    ACTUATOR_1 = T_INIT_VALUE;
    ACTUATOR_2 = T_INIT_VALUE;
    ACTUATOR_3 = T_INIT_VALUE;
    ACTUATOR_4 = T_INIT_VALUE;
    ACTUATOR_5 = T_INIT_VALUE;
    ACTUATOR_6 = T_INIT_VALUE;
    ACTUATOR_7 = T_INIT_VALUE;
    ACTUATOR_8 = T_INIT_VALUE;
    DelayMs(500);
}

void deinitActuators()
{
ACTUATORS_Disable();
ACTUATOR_1 = T_INIT_VALUE;
ACTUATOR_2 = T_INIT_VALUE;
ACTUATOR_3 = T_INIT_VALUE;
ACTUATOR_4 = T_INIT_VALUE;
ACTUATOR_5 = T_INIT_VALUE;
ACTUATOR_6 = T_INIT_VALUE;
ACTUATOR_7 = T_INIT_VALUE;
ACTUATOR_8 = T_INIT_VALUE;
TIM1->BDTR &= ~TIM_BDTR_MOE;
TIM1->CR1 &= ~TIM_CR1_CEN;
TIM3->CR1 &= ~TIM_CR1_CEN;
}

void ACTUATORS_Enable()
{
    allow_update = true;
    LED_Red_Set(true);

}
void ACTUATORS_Disable()
{
allow_update=false;
LED_Red_Set(false);
ACTUATOR_1 = T_INIT_VALUE;
ACTUATOR_2 = T_INIT_VALUE;
ACTUATOR_3 = T_INIT_VALUE;
ACTUATOR_4 = T_INIT_VALUE;
ACTUATOR_5 = T_INIT_VALUE;
ACTUATOR_6 = T_INIT_VALUE;
ACTUATOR_7 = T_INIT_VALUE;
ACTUATOR_8 = T_INIT_VALUE;
}
float ACTUATORS_map(float x, float in_min, float in_max) {
  return (x - in_min) * (T_MAX_VALUE - T_MIN_VALUE) / (in_max - in_min) + T_MIN_VALUE;
}

void ACTUATORS_IronManFan(uint16_t pwm)
{
    TIM2->CCR1 = pwm;
}