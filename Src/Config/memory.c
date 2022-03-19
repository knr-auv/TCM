#include <stdbool.h>
#include "drivers/SST25_flash/SST25_flash.h"

bool MEM_Init()
{
SST25_Init();
return SST25_SelfTest();
}

uint8_t MEM_ReadByte(uint32_t address)
{
    uint8_t ret;
    SST25_Read(address,&ret,1);
    return ret;
}
void MEM_WriteByte(uint32_t address, uint8_t data)
{
    SST25_WriteByte(address,data);
}
