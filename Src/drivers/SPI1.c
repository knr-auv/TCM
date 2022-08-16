
#include <stdbool.h>
#include "stm32f4xx.h"
#include "drivers/SPI1.h"

//	practical usage:
static void CS_SPI1_disable();
static void CS_SPI1_enable();
static void SPI1_enable();
static void SPI1_disable();
static void SPI1_receive_DMA(uint8_t *data, uint16_t size);
static void SPI1_transmit_DMA(uint8_t *data, uint16_t size);
static void SPI1_receive(uint8_t *data, uint16_t size);
static void SPI1_transmit(uint8_t *data, uint16_t size);

//	Function for basic SPI usage:
static void
CS_SPI1_enable()
{
    GPIOB->BSRRH |= 1 << 6;
}

static void CS_SPI1_disable()
{
    GPIOB->BSRRL |= 1 << 6;
}

static void SPI1_enable()
{
    SPI1->CR1 |= SPI_CR1_SPE; //	enabling SPI1
}

static void SPI1_disable()
{
    SPI1->CR1 &= ~SPI_CR1_SPE; //	disabling SPI1
}

// Example functions for SPI with DMA usage

// DMA interruptions handlers:

// RX:
void DMA2_Stream2_IRQHandler(void)
{

    // if stream2 transfer is completed:
    if (DMA2->LISR & DMA_LISR_TCIF2)
    {
        DMA2->LIFCR |= DMA_LIFCR_CTCIF2;
        // robust implementation needs extra code
        //	Szymon ma gotowe kody wiec nie ma sie co tym zajmowac
        SPI1->DR;
        SPI1->SR;
        DMA2_Stream2->CR &= ~DMA_SxCR_EN;

        CS_SPI1_disable();
        SPI1_disable();
    }
}

// TX:
void DMA2_Stream3_IRQHandler(void)
{

    // if stream3 transfer is completed:
    if (DMA2->LISR & DMA_LISR_TCIF3)
    {
        DMA2->LIFCR |= DMA_LIFCR_CTCIF3;
        //	jak wyzej
        SPI1->DR;
        SPI1->SR;
        DMA2_Stream3->CR &= ~DMA_SxCR_EN;

        CS_SPI1_disable();
        SPI1_disable();
    }
}

static void SPI1_receive_DMA(uint8_t *data, uint16_t size)
{
    // set parameters of reception:
    DMA2_Stream2->M0AR = (uint32_t)(data);
    DMA2_Stream2->NDTR = size;
    DMA2_Stream2->CR |= DMA_SxCR_EN;

    // DMA need to send sth. to maintain SPI communication:
    DMA2_Stream3->M0AR = (uint32_t)(data);
    DMA2_Stream3->NDTR = size;
    DMA2_Stream3->CR |= DMA_SxCR_EN;
}

static void SPI1_transmit_DMA(uint8_t *data, uint16_t size)
{

    DMA2_Stream3->M0AR = (uint32_t)(data);
    DMA2_Stream3->NDTR = size;
    DMA2_Stream3->CR |= DMA_SxCR_EN;
}

static void SPI1_receive(uint8_t *data, uint16_t size)
{
    // WARNING all while() should have some extra code for robust implementation
    // (Szymon ma to ogarniete)

    while (size > 0)
    {
        while (!((SPI1->SR) & SPI_SR_TXE))
        {
        }
        SPI1->DR = 0xFF; // send anything IMPORTANT!
        while (!((SPI1->SR) & SPI_SR_RXNE))
        {
        }
        *data++ = SPI1->DR;
        size--;
    }
    // wait for TXE flag
    while (!((SPI1->SR) & SPI_SR_TXE))
    {
    }
    // wait for BSY flag
    while (((SPI1->SR) & SPI_SR_BSY))
    {
    }
    SPI1->DR;
    SPI1->SR;
}

static void SPI1_transmit(uint8_t *data, uint16_t size)
{
    // WARNING: all while() should have some extra code for robust implementation
    // (Szymon ma to ogarniete)

    int i = 0;

    while (!((SPI1->SR) & SPI_SR_TXE))
    {
    }
    SPI1->DR = data[i]; // first data
    i++;

    while (i < size)
    {
        while (!((SPI1->SR) & SPI_SR_TXE))
        {
        }
        SPI1->DR = data[i]; // second and following data sending as soon as TX flag is set
        i++;
    }
    while (!((SPI1->SR) & SPI_SR_TXE))
    {
    }

    while (((SPI1->SR) & SPI_SR_BSY))
    {
    }
    SPI1->DR;
    SPI1->SR;
}

// functions for DMA usage:
void SPI1_write_DMA(uint8_t instruction, uint8_t *data, uint16_t size)
{

    SPI1_enable();
    CS_SPI1_enable();
    // if you want send sth. without DMA use:
    SPI1_transmit(&instruction, 1);
    // if you want send sth. with DMA use:
    SPI1_transmit_DMA(data, size);
}

void SPI1_read_DMA(uint8_t instruction, uint8_t *data, uint16_t size)
{

    SPI1_enable();
    CS_SPI1_enable();

    SPI1_transmit(&instruction, 1);
    SPI1_receive_DMA(data, size);
}

// functions same as above but without DMA:
void SPI1_write(uint8_t instruction, uint8_t *data, uint16_t size)
{

    CS_SPI1_enable();

    SPI1_transmit(&instruction, 1);
    SPI1_transmit(data, size);

    CS_SPI1_disable();
}

void SPI1_read(uint8_t instruction, uint8_t *data, uint16_t size)
{

    CS_SPI1_enable();

    SPI1_transmit(&instruction, 1);
    SPI1_receive(data, size);

    CS_SPI1_disable();
}