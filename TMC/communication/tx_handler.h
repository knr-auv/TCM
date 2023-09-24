#pragma once
#include <stdint.h>


bool TX_CheckEmpty();
uint8_t *TX_GetFreeBuffer(void);
void TX_SendData(uint8_t* buffer, uint16_t len);
void TX_Init();
void TX_Task();