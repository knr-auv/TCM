#include "stm32f4xx.h"
#include "time/time.h"


void SPI3_Transmit (uint8_t *data, int size)
{
	int i=0;
	while(!(SPI3->SR & SPI_SR_TXE));
	SPI3->DR = data[i];
	i++;

	while (i<size)
	{
	   while (!((SPI3->SR)&SPI_SR_TXE)) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	   SPI3->DR = data[i];  // load the data into the Data Register
	   i++;
	}	

	while (!((SPI3->SR)& SPI_SR_TXE)) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	while (((SPI3->SR)&SPI_SR_BSY)) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication	
	SPI3->DR;
	SPI3->SR;
}

void SPI3_Receive (uint8_t *data, int size)
{

	while (size>0)
	{
		while (!((SPI3->SR)&SPI_SR_TXE)) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
		SPI3->DR = 0xFF;  // send dummy data
		while (!((SPI3->SR) & SPI_SR_RXNE)){};  // Wait for RXNE to set -> This will indicate that the Rx buffer is not empty
	  *data++ = (SPI3->DR);
		size--;
	}	
	while (!((SPI3->SR) & SPI_SR_TXE)){};
	while (((SPI3->SR) & SPI_SR_BSY)){};
	SPI3->DR;
	SPI3->SR;
}

void SPI3_Transmit_Receive(uint8_t* tx_buffer, uint8_t* rx_buffer, uint16_t size){
for(uint16_t i=0;i<size; i++) 
	{
		while (((SPI3->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
		 while (!((SPI3->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	   		SPI3->DR = tx_buffer[i];
		
		while (!((SPI3->SR) &(1<<0))){};  // Wait for RXNE to set -> This will indicate that the Rx buffer is not empty
	  		rx_buffer[i] = (SPI3->DR);
	}	
	while (!((SPI3->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	while (((SPI3->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication	
	
	//  Clear the Overrun flag by reading DR and SR
	SPI3->DR;
	SPI3->SR;
	
}
