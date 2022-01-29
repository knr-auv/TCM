#include "stm32f4xx.h"

void GPIOTogglePin(GPIO_TypeDef* bus, uint16_t pin);
void GPIOSetPin(GPIO_TypeDef* bus, uint16_t pin, bool value);