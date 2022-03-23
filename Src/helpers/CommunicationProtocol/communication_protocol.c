#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "communication_protocol.h"


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


static uint16_t calculateChecksum(uint8_t* data, uint16_t len)
{
    uint16_t sum = 0;
    for(uint16_t i =0; i< len;i++)
    {
        sum+= data[i];
    }
    return sum;
}


void COMPROTO_ParseMsg(uint8_t* user_input, uint16_t len, COMPROTO_msg_info_t* msg_struct)
{
    if((user_input[0]!= MSG_HEADER_1)&&(user_input[1]!=MSG_HEADER_2))
    {
        msg_struct->valid = false;
        return;
    }
   
    uint16_t dataLen =  (user_input[2] <<8)| (user_input[3]);
    uint16_t checksum = (user_input[len - 2]<<8)|user_input[len - 1];

    if(checksum!=calculateChecksum(user_input, dataLen + 5))
    {
        msg_struct->valid = false;
        return;
    }
    msg_struct->len = dataLen;
   
    uint8_t Packet_type = user_input[4];
    uint8_t *data = user_input+5;


    if((Packet_type>=0)&(Packet_type<MSG_OKON_TYPE_COUNT))
    {
        msg_struct->valid = true;
        msg_struct->data = data;
        msg_struct->msg_type = Packet_type;
        msg_struct->len = dataLen;
        msg_struct->specific = 0;
    }
    else
    {
        msg_struct->valid = false;
        return;
    }
        
    if((Packet_type==MSG_OKON_REQUEST)||(Packet_type==MSG_OKON_SERVICE))
    {
        msg_struct->specific = data[0];
        msg_struct->data++;
        msg_struct->len--;
    }

    return;
}

void COMPROTO_CreateMsg(COMPROTO_msg_from_okon_t* msg)
{
    msg->tx_buffer_len = 7 + msg->user_data_len;
    msg->tx_buffer[0] = MSG_HEADER_1;
    msg->tx_buffer[1] = MSG_HEADER_2;
    msg->tx_buffer[2] = msg->user_data_len>>8;
    msg->tx_buffer[3] = msg->user_data_len;
    msg->tx_buffer[4] = msg->type;
    memcpy(msg->tx_buffer+5, msg->user_data, msg->user_data_len);
    uint16_t checksum = calculateChecksum(msg->tx_buffer, msg->tx_buffer_len-2);
    msg->tx_buffer[msg->tx_buffer_len-2] = checksum>>8;
    msg->tx_buffer[msg->tx_buffer_len-1] = checksum;
    msg->created = true;
}

