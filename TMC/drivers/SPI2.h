#include <stdint.h>
void SPI2_Transmit_Receive(uint8_t* tx_buffer, uint8_t* rx_buffer, uint16_t size);
void SPI2_Receive (uint8_t *data, int size);
void SPI2_Transmit (uint8_t *data, int size);