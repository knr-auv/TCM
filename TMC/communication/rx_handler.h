#pragma once
#include <stdint.h>

void RX_Handler_init();
void RX_Handler_receive();
bool RX_Handler_CheckNewMessage();
uint16_t RX_Handler_GetNewMsg(uint8_t *rx_buffer);