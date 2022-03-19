#include <stdbool.h>
#include <stdint.h>
#include "operation/ControlLoop.h"

#include "drivers/SST25_flash/SST25_flash.h"

/*
This module is using 2nd sector (addresses from 4096).
*/
#define START_ADDRESS 0x2048

#define MEM_ID_PID      0x0001


typedef struct
{
    uint16_t ID;
    uint16_t len;
}mem_data_block_t;



bool MEM_Init()
{
SST25_Init();
return SST25_ReadID();//SST25_SelfTest();
}

void MEM_CreateHeader(uint8_t* buffer, uint16_t ID, uint16_t len)
{
    buffer[0] = ID>>8;
    buffer[1] = ID;
    buffer[2] = len >>8;
    buffer[3] = len;
}
void MEM_SaveSettings()
{
    uint8_t header[4];
    uint8_t dataBuffer[200];
    uint16_t len;
    SST25_Erase4K(START_ADDRESS);
  
    CL_SerializePIDs(dataBuffer, &len);
    uint16_t address = START_ADDRESS;

    MEM_CreateHeader(header, MEM_ID_PID, len);
    SST25_WriteBytes(address ,header, 4);
    address+=4;
    SST25_WriteBytes(address, dataBuffer, len);
}

void MEM_ParseHeader(uint8_t* buffer, uint16_t* ID, uint16_t* len)
{
    *ID = buffer[0]<<8|buffer[1];
    *len = buffer[2]<<8|buffer[3];
}
void MEM_LoadSettings()
{
    uint8_t data_buffer[200];
    uint8_t header[4];
    uint16_t ID, len;
    uint16_t address = START_ADDRESS;
    
    SST25_Read(address, header, 4);
    address +=4;
    MEM_ParseHeader(header, &ID, &len);
    if(len > 200)
    return;
    SST25_Read(address, data_buffer, len);
    CL_LoadPIDs(data_buffer, len);

}
