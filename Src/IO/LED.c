#include <stdbool.h>
#include "stm32f4xx.h"
#include "drivers/GPIO.h"

void LED_Green_Toggle(void){
    GPIOTogglePin(GPIOC,GPIO_ODR_ODR_8);
}
void LED_Red_Toggle(void){
     GPIOTogglePin(GPIOD,GPIO_ODR_ODR_11);
}

void LED_HeartBeat_Toggle(void)
{
    GPIOTogglePin(GPIOB,GPIO_ODR_ODR_7);
}
void LED_HeartBeat_Set(bool state){
        GPIOSetPin(GPIOD,7,state);
}
void LED_Green_Set(bool state){
    GPIOSetPin(GPIOC,8,state);
}
void LED_Red_Set(bool state){
        GPIOSetPin(GPIOD,11,state);
}