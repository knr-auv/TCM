#include <string.h>
#include <stdbool.h>
#include "config.h"
#include "drivers/USART.h"

#define HEADER_SIZE 3
#define CHECKSUM_VAR uint16_t
#define CHECKSUM_SIZE sizeof(uint16_t)
#define HEADER 0x69

static USART_t *port;
static bool pending_message;
uint8_t tx_buffer[255];
uint8_t *header_buffer = tx_buffer;
uint8_t *msg_buffer = tx_buffer + HEADER_SIZE;

void _tx_completed_callback();

void TX_Handler_init()
{
    port = USART_GetUSART(CONFIG_COMM_HANDLER_USART);
    port->TXCompleteCallback = _tx_completed_callback;
    pending_message = false;
}
void constructHeader(uint8_t msg_len)
{
    header_buffer[0] = HEADER;
    header_buffer[1] = HEADER - 1;
    header_buffer[2] = msg_len;
}

uint16_t _createFrame(uint8_t *buffer, uint8_t len)
{
    memcpy(msg_buffer, buffer, len);
    constructHeader(len + HEADER_SIZE + CHECKSUM_SIZE);
    CHECKSUM_VAR checksum = 0;

    for (uint8_t i = 0; i < len + HEADER_SIZE; i++)
    {
        checksum += tx_buffer[i];
    }
    memcpy(msg_buffer + len, &checksum, CHECKSUM_SIZE);
    return len + CHECKSUM_SIZE + HEADER_SIZE;
}

bool TX_Handler_SendData(uint8_t *buffer, uint8_t len)
{
    if (pending_message)
        return false;
    uint16_t buffer_len = _createFrame(buffer, len);
    port->TransmitDMA(tx_buffer, buffer_len);
    pending_message = true;
    return true;
}

void _tx_completed_callback()
{
    pending_message = false;
}