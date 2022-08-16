
#ifndef I2C2_H_
#define I2C2_H_

void I2C2_Start();
void I2C2_Stop();
void I2C2_Write(uint8_t address, uint8_t command, uint8_t *data, uint16_t size);
void I2C2_Read(uint8_t address, uint8_t *data, uint16_t size);
void I2C2_Read_DMA(uint8_t address, uint8_t *data, uint16_t size);

#endif