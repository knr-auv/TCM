#include <stdint.h>
#include "stm32f4xx.h"
#include "time.h"
#include "IO/LED.h"
static volatile timeUs_t seconds;

void TicksInit(void)
{
    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler()
{
    if(TIM2->SR &&TIM_SR_UIF)
    {
        TIM2->SR &=~TIM_SR_UIF;
        seconds+=1;
    }

    
}
timeUs_t micros(void)
{
    return seconds*1000000 + TIM2->CNT;
}

void DelayUs(uint32_t us)
{
    timeUs_t t = micros();
    while (micros() - t < us)
        ;
}
void DelayMs(uint32_t ms)
{
    timeUs_t t = micros();
    while (micros() - t < ms*1000 )
        ;
}

