#pragma once
#include <stdint.h>
#include <stdbool.h>

/*
address range:
0x0
0x3FFFF

262 144 bytes

64  4k sectors
8   32Kbyte addres
4   64kbyte addres
*/

/*
    Library user should provide metods declared in SST25_platform.h:
    void SetPinCS(bool value);
    void SetPinHOLD(bool value);
    void SetPinWP(bool value);
    void SPI_Transmit(uint8_t* buffer, uint16_t size);
    void SPI_Receiv(uint8_t* buffer, uint16_t size);

    It is adviced to perform a chip self test sometimes :) It uses last memory block
*/



void SST25_Init();
uint8_t SST25_ReadID();
uint16_t SST25_ReadJEDEC_ID();
uint8_t SST25_ReadStatusReg();
uint8_t SST25_ReadStatusReg1();
void SST25_ChipErase();
void SST25_Erase4K(uint32_t dst);
void SST25_Erase32K(uint32_t dst);
void SST25_Erase64K(uint32_t dst);


void SST25_Read(uint32_t address,uint8_t* buffer, uint16_t len);
void SST25_FastRead(uint32_t address,uint8_t* buffer, uint16_t len);
void SST25_WriteByte(uint32_t address, uint8_t data);
void SST25_WriteBytes(uint32_t address, uint8_t* buffer, uint16_t len);

bool SST25_SelfTest();