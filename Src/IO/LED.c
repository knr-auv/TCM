#include <stdbool.h>
#include "stm32f4xx.h"
#include "drivers/GPIO.h"

void LED_Green_Toggle(void){
    GPIOTogglePin(GPIOC,GPIO_ODR_ODR_8);
}
void LED_Red_Toggle(void){
     GPIOTogglePin(GPIOD,GPIO_ODR_ODR_11);
}

void LED_Green_Set(bool state){
    GPIOSetPin(GPIOC,GPIO_ODR_ODR_8,state);
}
void LED_Red_Set(bool state){
        GPIOSetPin(GPIOD,GPIO_ODR_ODR_11,state);
}