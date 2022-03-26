#include <stdint.h>
#include "stm32f4xx.h"
#include "time.h"
#include "IO/LED.h"
static volatile timeUs_t seconds;

void TicksInit(void)
{
    seconds = 0;
    TIM5->CR1 |= TIM_CR1_CEN;
}

void TIM5_IRQHandler()
{
    if(TIM5->SR &&TIM_SR_UIF)
    {
        TIM5->SR &=~TIM_SR_UIF;
        seconds+=1;
    }

    
}
timeUs_t micros(void)
{
    return seconds*1000000 + TIM5->CNT;
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

