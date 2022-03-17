#include <stdint.h>
#include "comunication_protocol.h"
#include <stdbool.h>
#include <string.h>
#include "time/time.h"
#include "IO/Sticks.h"
/*
Control packet structure:

header:
0x69 - const
0x68 - const
datalen msb
datalen lsb
msg type


data
....
....
checksum 

*/
#define BYTES_TO_FLOAT(buffer) buffer[0]|buffer[1]<<8|buffer[2]<<16|buffer[3]<<24

#define MSG_HEADER_1 0x69
#define MSG_HEADER_2 0x68

bool NewStickValues = false;
bool NewDirectThrusterValues = false;
bool NewMode = false;
bool NewCLStatus = false;
float MotorDirect[8];
float StickValues[16];

uint8_t Mode;
uint8_t CL_status;
timeUs_t lastValidInput;

bool COMPROTO_NewStickValues()
{
    return NewStickValues;
}

bool COMPROTO_NewDirectThrusterValues()
{
    return NewDirectThrusterValues;
}

bool COMPROTO_NewMode()
{
    return NewMode;
}

bool COMPROTO_NewCLStatus()
{
    return NewCLStatus;
}


uint8_t COMPROTO_GetCLStatus()
{
    NewCLStatus = false;
    return CL_status;
}

float* COMPROTO_GetDirectThrustersValues()
{
    NewStickValues =false;
    return MotorDirect;
}
float* COMPROTO_GetStickValues()
{
    NewStickValues = false;
    return StickValues;
}
uint8_t COMPROTO_GetMode()
{
    NewMode = false;
    return Mode;
}

static uint16_t calculateChecksum(uint8_t* data, uint16_t len)
{
    uint16_t sum = 0;
    for(uint16_t i =0; i< len;i++)
    {
        sum+= data[i];
    }
    return sum;
}


void COMPROTO_Parse_Stick_MSG(uint8_t* data, uint16_t len)
{
    //keep in mind that order is: roll, pitch, yaw, throttle, vertical, change mode
    uint8_t i;
    for(i =0; i<len/4; i++)
    {
        StickValues[i] = BYTES_TO_FLOAT((data+i*4));
    }
    NewStickValues = true;
   
}


void COMPROTO_Parse_DirectThrusters_MSG(uint8_t* data, uint16_t len)
{
    for(uint8_t i =0; i<8;i++)
    {
        uint8_t *buffer = data + 4*i;
        MotorDirect[i] = BYTES_TO_FLOAT(buffer);
    }
    NewDirectThrusterValues = true;
}

void COMPROTO_Parse_Mode_MSG(uint8_t* data, uint16_t len)
{
    Mode = data[0];
}
void COMPROTO_Parse_CLStatus_MSG(uint8_t* data, uint16_t len)
{
    CL_status = data[0];
}
int COMPROTO_ParseMsg(uint8_t* user_input)
{
    if((user_input[0]!= MSG_HEADER_1)&&(user_input[1]!=MSG_HEADER_2))
        return -1;
   
    uint16_t dataLen =  user_input[2] | (user_input[3]<<8);
    uint16_t checksum = (user_input[dataLen - 1]<<8)|user_input[dataLen - 2];
    if(checksum!=calculateChecksum(user_input, dataLen + 3))
    {
        return -1;
    }

    uint8_t Packet_type = user_input[4];
    uint8_t *data = user_input+5;
    switch(Packet_type)
    {
        case MSG_STICKS:
            COMPROTO_Parse_Stick_MSG(data, dataLen);
            break;
        case MSG_DIRECT_THRUSTERS_CTRL:
            COMPROTO_Parse_DirectThrusters_MSG(data, dataLen);
            break;
        case MSG_MODE:
            COMPROTO_Parse_Mode_MSG(data, dataLen);
            break;
        case MSG_CL_STATUS:
            COMPROTO_Parse_CLStatus_MSG(data, dataLen);
            break;
        default:
            Packet_type = -1;
            break;
    }
    return Packet_type;
}



