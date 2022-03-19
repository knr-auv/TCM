#pragma once


bool MEM_Init();
float MEM_ReadFloat(uint32_t address);
void MEM_WriteFloat(uint32_t address, float data);
uint8_t MEM_ReadByte(uint32_t byte);
void MEM_WriteByte(uint32_t address, uint8_t data);