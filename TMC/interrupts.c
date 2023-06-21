#include "stm32f4xx.h"
#include "Sensors/BNO080/bno080.h"
void EXTI9_5_IRQHandler(void){

    if(EXTI_PR_PR9 && EXTI->PR){
        BNO080_interruptHandler();
        EXTI->PR =(EXTI_PR_PR9);    //clear pending bit
    }
}
