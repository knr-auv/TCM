#include <stdint.h>
void SPI3_Transmit_Receive(uint8_t* tx_buffer, uint8_t* rx_buffer, uint16_t size);
void SPI3_Receive (uint8_t *data, int size);
void SPI3_Transmit (uint8_t *data, int size);