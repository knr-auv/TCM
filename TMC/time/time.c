#include <stdint.h>
#include "stm32f4xx.h"
#include "time.h"
#include "IO/LED.h"

volatile timeUs_t global_time;

void TicksInit(void)
{
    global_time = 0;
    TIM5->CR1 |= TIM_CR1_CEN;
}

void TIM5_IRQHandler()
{
    if (TIM_SR_UIF & TIM5->SR)
    {
        TIM5->SR &= ~TIM_SR_UIF;
        global_time += (TIM5->ARR + 1);
    }
}

timeUs_t micros(void)
{
    return global_time + TIM5->CNT;
}

void DelayUs(uint32_t us)
{
    timeUs_t t = micros();
    while (micros() - t < us)
    {
        ; // wait
    }
}
void DelayMs(uint32_t ms)
{
    timeUs_t t = micros();
    while (micros() - t < ms * 1000)
    {
        ; // wait
    }
}
