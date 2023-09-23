#include <stdint.h>
#include <string.h>
#include "protocol_data_types.h"
#define MSG_HEADER_1    105
#define MSG_HEADER_2    104
#define HEADER_LEN      4
#define CRC_LEN         2

typedef enum {
    STATE_H1,
    STATE_H2,
    STATE_LEN,
    STATE_TYPE,
    STATE_PAYLOAD,
    STATE_CRC1,
    STATE_CRC2
}parse_state_e;

static uint16_t calculateChecksum(uint8_t *data_p, uint16_t length, uint16_t crc)
{
    uint8_t x;
    while (length--)
    {
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
    }
    return crc;
}

bool COM_PROTO_ParseByte(uint8_t byte, COM_PROTO_msg_t *const msg)
{
    static parse_state_e rx_state = 0;
    static uint16_t received_crc = 0;
    static uint16_t crc = 0;
    static uint8_t expected_len = 0;
    switch (rx_state)
    {
    case STATE_H1:
    {
        if (byte == MSG_HEADER_1)
        {
            crc = calculateChecksum(&byte, 1, 0xFFFF);
            rx_state++;
        }
        break;
    }
    case STATE_H2:
    {
        if (byte == MSG_HEADER_2)
        {
            crc = calculateChecksum(&byte, 1, crc);
            rx_state++;
        }
        else
            rx_state = STATE_H1;
        break;
    }
    case STATE_LEN:
    {
        expected_len = byte;
        rx_state++;
        crc = calculateChecksum(&byte, 1, crc);
        break;
    }
    case STATE_TYPE:
    {
        crc = calculateChecksum(&byte, 1, crc);
        msg->type = byte;
        msg->payload_len = 0;
        rx_state++;
        break;
    }
    case STATE_PAYLOAD:
    {
        msg->p_payload[msg->payload_len++] = byte;
        crc = calculateChecksum(&byte, 1, crc);
        if (msg->payload_len >= expected_len)
        {
            rx_state++;
        }
        break;
    }
    case STATE_CRC1:
    {
        received_crc = byte << 8;
        rx_state++;
        break;
    }
    case STATE_CRC2:
    {
        received_crc |= byte;
        rx_state = STATE_H1;
        if (received_crc == crc)
            return true;
        break;
    }
    default:
    {
        rx_state = STATE_H1;
        break;
    }
    }
    return false;
}

const uint16_t COM_PROTO_CreateMsg(COM_PROTO_msg_t *const msg, uint8_t *const tx_buffer, const uint16_t tx_buffer_size)
{
    if ((msg->payload_len + HEADER_LEN + CRC_LEN) > tx_buffer_size)
        return 0;
    const uint16_t bytes_to_send = HEADER_LEN + CRC_LEN + msg->payload_len;
    tx_buffer[0] = MSG_HEADER_1;
    tx_buffer[1] = MSG_HEADER_2;
    tx_buffer[2] = msg->payload_len;
    tx_buffer[3] = msg->type;
    memcpy(tx_buffer + 4, msg->p_payload, msg->payload_len);
    uint16_t checksum = calculateChecksum(tx_buffer, bytes_to_send - 2, 0xFFFF);
    tx_buffer[bytes_to_send - 2] = checksum >> 8;
    tx_buffer[bytes_to_send - 1] = checksum;
    return bytes_to_send;
}