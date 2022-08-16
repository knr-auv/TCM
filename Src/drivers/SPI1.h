#ifndef SPI1_H_
#define SPI1_H_
#include <stdint.h>

//	functions for communication:
void SPI1_write_DMA(uint8_t instruction, uint8_t *data, uint16_t size);
void SPI1_read_DMA(uint8_t instruction, uint8_t *data, uint16_t size);
void SPI1_write(uint8_t instruction, uint8_t *data, uint16_t size);
void SPI1_read(uint8_t instruction, uint8_t *data, uint16_t size);

#endif /* SPI1_H_ */