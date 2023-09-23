#pragma once
#include <stdbool.h>
#define START_ADDRESS 4096
#define MEM_BUFFER_SIZE     200
#define MEM_ID_PID          0x0001
#define MEM_ID_CL_MATRIX    0x0002

uint8_t FLASH_Init();
void FLASH_SaveData(uint8_t* buffer, uint16_t len, uint32_t address);
void FLASH_LoadData(uint8_t* buffer, uint16_t buffer_len, uint32_t address);

