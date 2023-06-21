#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "operation/ControlLoop.h"

#include "drivers/SST25_flash/SST25_flash.h"

/*
This module is using 2nd sector (addresses from 4096).
*/
#define START_ADDRESS 4096
#define MEM_BUFFER_SIZE     200
#define MEM_ID_PID          0x0001
#define MEM_ID_CL_MATRIX    0x0002

typedef struct
{
    uint16_t ID;
    uint16_t len;
}mem_data_block_t;



bool MEM_Init()
{
SST25_Init();
uint8_t id = SST25_ReadID();
return id;
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
    uint8_t *dataBuffer;;
    uint16_t len;
    uint16_t address = START_ADDRESS;

    SST25_Erase4K(address);
  
    dataBuffer = CL_SerializePIDs( &len);
    MEM_CreateHeader(header, MEM_ID_PID, len);
    SST25_WriteBytes(address ,header, 4);
    address+=4;
    SST25_WriteBytes(address, dataBuffer, len);
    free(dataBuffer);
    address+=len;

    dataBuffer = CL_SerializeControlActuatorsMatrix(&len);
    MEM_CreateHeader(header, MEM_ID_CL_MATRIX, len);
    SST25_WriteBytes(address ,header, 4);
    address+=4;
    SST25_WriteBytes(address, dataBuffer, len);
 
    address+=len;


}

void MEM_ParseHeader(uint8_t* buffer, uint16_t* ID, uint16_t* len)
{
    *ID = buffer[0]<<8|buffer[1];
    *len = buffer[2]<<8|buffer[3];
}

void MEM_LoadSettings()
{
    uint8_t data_buffer[MEM_BUFFER_SIZE];
    uint8_t header[4];
    uint16_t ID, len;
    uint16_t address = START_ADDRESS;
    
    SST25_Read(address, header, 4);
    address +=4;
    MEM_ParseHeader(header, &ID, &len);
    if(ID!= MEM_ID_PID)
        return;
    if(len > MEM_BUFFER_SIZE)
        return;
    SST25_Read(address, data_buffer, len);
    address+=len;
    CL_LoadPIDs(data_buffer, len);

    SST25_Read(address, header, 4);
    address +=4;
    MEM_ParseHeader(header, &ID, &len);
    if(ID!= MEM_ID_CL_MATRIX)
        return;
    if(len > MEM_BUFFER_SIZE)
        return;
    SST25_Read(address, data_buffer, len);
    CL_LoadControlActuatorsMatrix(data_buffer, len);

}
