

#include "stm32f4xx.h"
#include "drivers/I2C2.h"
#include <stdbool.h>
#include "time/time.h"
#include "stddef.h"

// functions for I2C usage:

void I2C2_Start()
{
    I2C2->CR1 |= I2C_CR1_ACK;
    I2C2->CR1 |= I2C_CR1_START;
    while (!(I2C2->SR1 & I2C_SR1_SB))
    {
    }
}

void I2C2_Stop()
{
    I2C2->CR1 |= I2C_CR1_STOP;
}

void I2C2_Write(uint8_t address, uint8_t command, uint8_t *data, uint16_t size)
{
    I2C2_Start();
    I2C2->DR = address;
    //	wait for ADDR bit:
    while (!(I2C2->SR1 & I2C_SR1_ADDR))
    {
    }
    // for clearing ADDR bit read also SR2:
    I2C2->SR2;
    //	send command:
    while (!(I2C2->SR1 & I2C_SR1_TXE))
    {
        ;
    }
    I2C2->DR = command;
    //	send data:
    while (size)
    {
        while (!(I2C2->SR1 & I2C_SR1_TXE))
        {
        }
        I2C2->DR = (uint32_t)*data;

        data++;
        size--;
    }
    while (!(I2C2->SR1 & I2C_SR1_BTF) && !(I2C2->SR1 & I2C_SR1_TXE))
    {
    }
}

void I2C2_Read(uint8_t address, uint8_t *data, uint16_t size)
{

    I2C2_Start();

    I2C2->DR = (address | 0x1);
    // wait for ADDR bit:
    while (!(I2C2->SR1 & I2C_SR1_ADDR))
    {
    }

    // different steps for 1, 2, more bytes reception:
    if (size == 2)
    {
        I2C2->CR1 &= ~I2C_CR1_ACK;
        I2C2->CR1 |= I2C_CR1_POS;

        // for clearing ADDR bit read also SR2:
        I2C2->SR2;
        while (!(I2C2->SR1 & I2C_SR1_BTF))
        {
        }
        I2C2->CR1 |= I2C_CR1_STOP;
        // now receive 2 bytes:
        while (!(I2C2->SR1 & I2C_SR1_RXNE))
        {
        }
        *data = I2C2->DR;
        data++;
        while (!(I2C2->SR1 & I2C_SR1_RXNE))
        {
        }
        *data = I2C2->DR;
        return;
    }
    if (size == 1)
    {
        I2C2->CR1 &= ~I2C_CR1_ACK;
        // for clearing ADDR bit read also SR2:
        I2C2->SR2;
        I2C2->CR1 |= I2C_CR1_STOP;
        while (!(I2C2->SR1 & I2C_SR1_RXNE))
        {
            // wait for byte to read
        }
        *data = I2C2->DR;
        return;
    }

    // for clearing ADDR bit read also SR2:
    I2C2->SR2;

    while (size > 3)
    {

        while (!(I2C2->SR1 & I2C_SR1_RXNE))
        {
        }
        *data = I2C2->DR;
        // ACK will be send automatically
        data++;
        size--;
    }

    while (!(I2C2->SR1 & I2C_SR1_BTF))
    {
    }
    I2C2->CR1 &= ~I2C_CR1_ACK;
    *data = I2C2->DR;
    data++;
    while (!(I2C2->SR1 & I2C_SR1_BTF))
    {
    }
    I2C2->CR1 |= I2C_CR1_STOP;
    // now receive 2 last bytes:
    while (!(I2C2->SR1 & I2C_SR1_RXNE))
    {
    }
    *data = I2C2->DR;
    data++;
    while (!(I2C2->SR1 & I2C_SR1_RXNE))
    {
    }
    *data = I2C2->DR;
}

void I2C2_Read_DMA(uint8_t address, uint8_t *data, uint16_t size)
{
    DMA1_Stream2->NDTR = size;
    DMA1_Stream2->M0AR = (uint32_t)(data);

    I2C2_Start();

    I2C2->DR = (address | 0x1);
    // wait for ADDR bit:
    while (!(I2C2->SR1 & I2C_SR1_ADDR))
    {
    }
    // activate DMA:
    DMA1_Stream2->CR |= DMA_SxCR_EN;

    // for clearing ADDR bit read also SR2:
    I2C2->SR2;
}
