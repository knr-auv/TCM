#include <string.h>
#include <stdbool.h>
#include "drivers/SPI3.h"
#include "stm32f4xx.h"
#include "drivers/GPIO.h"
#include "drivers/USART1.h"
#include "time/time.h"




void delay(uint32_t time){
	DelayMs(time);
}
void sendMsg(uint8_t* msg,uint16_t length){
    USART1_Transmit_DMA(msg, length);
//HAL_UART_Transmit(&huart2, msg, length, 1000);
}

void SPI_TransmitReceive(uint8_t* txBuffer,uint8_t* rxBuffer,uint16_t length){
SPI3_Transmit_Receive(txBuffer, rxBuffer, length);
}

void enableInts(void)
{
   NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void disableInts()
{
    NVIC_DisableIRQ(EXTI9_5_IRQn);
}

void bootn(uint8_t state)
{
GPIOSetPin(GPIOA, 8,state ? true : false);

}
void rstn(uint8_t state)
{
GPIOSetPin(GPIOC, 7,state ? true : false);
}
void ps0_waken(uint8_t state)
{
GPIOSetPin(GPIOA, 12,state ? true : false);
}

void ps1(uint8_t state)
{
//GPIOSetPin(GPIOA, 8,state ? true : false);
}

void csn(uint8_t state)
{
GPIOSetPin(GPIOA, 15,state ? true : false);
}

