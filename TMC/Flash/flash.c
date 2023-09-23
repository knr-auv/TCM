#include <stdint.h>
#include "drivers/SST25_flash/SST25_flash.h"

uint8_t FLASH_Init()
{
    SST25_Init();
    return SST25_ReadID();
}

void FLASH_SaveData(uint8_t* buffer, uint16_t len, uint32_t address)
{
    SST25_Erase4K(address);
    SST25_WriteBytes(address, buffer, len);
}
void FLASH_LoadData(uint8_t* buffer, uint16_t buffer_len, uint32_t address)
{
    SST25_Read(address, buffer, buffer_len);
}
