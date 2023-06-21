#include "drivers/SPI2.h"
#include "drivers/GPIO.h"
#include "stm32f4xx.h"

void SetPinCS(bool value)
{
GPIOSetPin(GPIOE, 7, value);
}
void SetPinHOLD(bool value)
{
    GPIOSetPin(GPIOD, 10, value);
}
void SetPinWP(bool value)
{
    GPIOSetPin(GPIOB, 12, value);
}


void SPI_Transmit(uint8_t* data, uint16_t size)
{
SPI2_Transmit(data, size);
}
void SPI_Receive(uint8_t* data, uint16_t size)
{
    SPI2_Receive(data, size);
}
