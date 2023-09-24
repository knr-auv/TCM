#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "Config/config.h"
#include "drivers/USART.h"

#define TX_HANDLER_BUFFERS 5

static void _tx_complete_cb();

typedef struct
{
    uint8_t buffer[CONFIG_COMM_HANDLER_BUFFER_LEN];
    uint16_t len;
} fifo_el_t;

static fifo_el_t *elementsBuffer[TX_HANDLER_BUFFERS];
static uint8_t head_index = 0;
static uint8_t tail_index = 0;
static uint8_t queue_size = 0;
static fifo_el_t tx_buffers[TX_HANDLER_BUFFERS];
void TX_Task();
static fifo_el_t *BufferGetFirst()
{
    if (queue_size == 0)
        return NULL;
    fifo_el_t *ret = elementsBuffer[tail_index];
    return ret;
}
static void BufferRemoveFirst()
{
    elementsBuffer[tail_index] = NULL;
    tail_index = (tail_index + 1) % TX_HANDLER_BUFFERS;
    queue_size--;
}

static bool BufferInsert(fifo_el_t* el)
{
    if (queue_size >= TX_HANDLER_BUFFERS)
    {
        return false;
    }
    elementsBuffer[head_index] = el;
    head_index = (head_index + 1) % TX_HANDLER_BUFFERS;
    queue_size++;
    return true;
}

static bool BufferIsIn(fifo_el_t *el)
{
    for (uint8_t i = 0; i < queue_size; i++)
    {
        if (elementsBuffer[(tail_index + i) % 5] == el)
            return true;
    }
    return false;
}

static USART_t *port;
static bool tx_complete;
static bool send_with_fifo = false;
void TX_Init()
{
    port = USART_GetUSART(CONFIG_COMM_HANDLER_USART);
    port->TXCompleteCallback = _tx_complete_cb;
    queue_size = 0;
    head_index = 0;
    tail_index = 0;
    tx_complete = true;
}

uint8_t* TX_GetFreeBuffer(void)
{
    for (uint8_t i = 0; i < TX_HANDLER_BUFFERS; i++)
    {
        if (!BufferIsIn(&tx_buffers[i]))
        {
            return tx_buffers[i].buffer;
        }
    }
    return (void*)0;
}

void TX_SendData(uint8_t *buffer, uint16_t len)
{
    // if (tx_complete && (queue_size==0))
    // {
    //     tx_complete = false;
    //     port->TransmitDMA(buffer, len);
    //     return;
    // }

    if (queue_size < TX_HANDLER_BUFFERS)
    {
        fifo_el_t *el = NULL;
        for (uint8_t i = 0; i < TX_HANDLER_BUFFERS; i++)
        {
            if (tx_buffers[i].buffer == buffer)
            {
                el = &tx_buffers[i];
                el->len = len;
                break;
            }
        }
        if(el==NULL)
            return;
        BufferInsert(el);
        if(queue_size==1)
            TX_Task();
    }
    //TX_Task();
}

bool TX_CheckEmpty()
{
    if (queue_size > 0)
        return false;
    return true;
}

void TX_Task()
{
    static fifo_el_t* prev_el = NULL;
    if (tx_complete == false)
        return;
    if(queue_size==0)
        return;
    fifo_el_t* el = BufferGetFirst();
    if ((el != NULL))
    {
       prev_el = el;
       send_with_fifo = true;
       tx_complete = false;
       port->TransmitDMA(el->buffer, el->len);
    }
}

static void _tx_complete_cb()
{
    if (send_with_fifo)
        BufferRemoveFirst();
    send_with_fifo = false;
    tx_complete = true;
}