#pragma once
#include <stdint.h>
#include <stdbool.h>

void SetPinCS(bool value);
void SetPinHOLD(bool value);
void SetPinWP(bool value);

void SPI_Transmit(uint8_t* data, uint16_t size);
void SPI_Receive(uint8_t* data, uint16_t size);