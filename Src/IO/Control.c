#include <stdbool.h>
#include <string.h>
#include "time/time.h"
#include "IO/Sticks.h"

#define BYTES_TO_FLOAT(buffer) buffer[0]|buffer[1]<<8|buffer[2]<<16|buffer[3]<<24

#define MSG_HEADER_1 0x69
#define MSG_HEADER_2 0x68


enum{
    MSG_STICKS,
    MSG_DIRECT_THRUSTERS_CTRL,
    MSG_ENABLE_TASK,
    MSG_COUNT
}msg_e;

float MotorDirect[8];

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
timeUs_t lastValidInput;



static uint16_t calculateChecksum(uint8_t* data, uint16_t len)
{
    uint16_t sum = 0;
    for(uint16_t i =0; i< len;i++)
    {
        sum+= data[i];
    }
    return sum;
}
void CTR_Parse_Stick_MSG(uint8_t* data, uint16_t len)
{
    //keep in mind that order is: roll, pitch, yaw, throttle, vertical, change mode
    float out[16];
    uint8_t i;
    for(i =0; i<len/4; i++)
    {
        out[i] = BYTES_TO_FLOAT((data+i*4));
    }
    STICK_HandleNewInput(out, i);
   
}

void CTR_Parse_DirectThrusters_MSG(uint8_t* data, uint16_t len)
{
    for(uint8_t i =0; i<8;i++)
    {
        uint8_t *buffer = data + 4*i;
        MotorDirect[i] = BYTES_TO_FLOAT(buffer);
    }
}
void CTR_Parse_EnableTask_MSG(uint8_t* data, uint16_t len)
{

}

void (*modeParse[MSG_COUNT])(uint8_t* user_input, uint16_t len)= 
{
[MSG_STICKS] = CTR_Parse_Stick_MSG,
[MSG_DIRECT_THRUSTERS_CTRL] = CTR_Parse_DirectThrusters_MSG,
[MSG_ENABLE_TASK] = CTR_Parse_EnableTask_MSG

};

void CTR_Parse_UserInput(uint8_t* user_input)
{
    if((user_input[0]!= MSG_HEADER_1)&&(user_input[1]!=MSG_HEADER_2))
        return;
   
    uint16_t dataLen =  user_input[2] | (user_input[3]<<8);
    uint16_t checksum = (user_input[dataLen - 1]<<8)|user_input[dataLen - 2];
    if(checksum!=calculateChecksum(user_input, dataLen + 3))
    {
        return;
    }

    uint8_t Packet_type = user_input[4];
    uint8_t *data = user_input+5;

    modeParse[Packet_type](data, dataLen);
}



void CTR_Task(timeUs_t t);

float* CTR_GetMotorsDirectVal()
{
    return MotorDirect;
}
