#include "stm32f4xx.h"
#include "time/time.h"


void SPI3_Transmit (uint8_t *data, int size)
{
	
	/************** STEPS TO FOLLOW *****************
	1. Wait for the TXE bit to set in the Status Register
	2. Write the data to the Data Register
	3. After the data has been transmitted, wait for the BSY bit to reset in Status Register
	4. Clear the Overrun flag by reading DR and SR
	************************************************/		
	
	int i=0;
	while (i<size)
	{
	   while (!((SPI3->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	   SPI3->DR = data[i];  // load the data into the Data Register
	   i++;
	}	
	
	
/*During discontinuous communications, there is a 2 APB clock period delay between the
write operation to the SPI_DR register and BSY bit setting. As a consequence it is
mandatory to wait first until TXE is set and then until BSY is cleared after writing the last
data.
*/
	while (!((SPI3->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	while (((SPI3->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication	
	
	//  Clear the Overrun flag by reading DR and SR
	uint8_t temp = SPI3->DR;
					temp = SPI3->SR;
	
}

void SPI3_Receive (uint8_t *data, int size)
{
	/************** STEPS TO FOLLOW *****************
	1. Wait for the BSY bit to reset in Status Register
	2. Send some Dummy data before reading the DATA
	3. Wait for the RXNE bit to Set in the status Register
	4. Read data from Data Register
	************************************************/		

	while (size)
	{
		while (((SPI3->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
		SPI3->DR = 0;  // send dummy data
		while (!((SPI3->SR) &(1<<0))){};  // Wait for RXNE to set -> This will indicate that the Rx buffer is not empty
	  *data++ = (SPI3->DR);
		size--;
	}	
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
	uint8_t temp = SPI3->DR;
					temp = SPI3->SR;
	
}
