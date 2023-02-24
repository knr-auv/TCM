#pragma once
#include <stdint.h>
#include <stdbool.h>

void TX_Handler_init();
bool TX_Handler_SendData(uint8_t *buffer, uint8_t len);