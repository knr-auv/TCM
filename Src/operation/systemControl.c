#include <stdbool.h>
#include <string.h>
#include "time/time.h"
#include "helpers/comunication_protocol.h"
#include "DirectThrustersCtrl.h"
#include "operation/ControlLoop.h"
#include "IO/Sticks.h"
#include "drivers/USART3.h"

#define RX_BUFFER_SIZE 100

uint8_t rx_buffer[RX_BUFFER_SIZE];

void SYSCTRL_Init()
{
    USART3_Receive_DMA(rx_buffer);
}


void SYSCTRL_Task(timeUs_t t)
{
    if(!USART3_NewData())
        return;
    //uint16_t data_len = USART2_GetReceivedBytes();

    int msg_type = COMPROTO_ParseMsg(rx_buffer);
    if(msg_type == MSG_MODE)
    {
        CL_SetMode(COMPROTO_GetMode());
    }
    else if(msg_type ==MSG_DIRECT_THRUSTERS_CTRL)
    {
        DTCRTL_HandlNewValues(COMPROTO_GetDirectThrustersValues());
    }
    else if(msg_type == MSG_STICKS)
    {
        float* sticks = COMPROTO_GetStickValues();
        STICK_HandleNewInput(sticks, 6);
    }
    else if(msg_type == MSG_CL_STATUS)
    {
        uint8_t status = COMPROTO_GetCLStatus();
        if(status == CL_STATUS_DISARMED)
            CL_Disarm();
        else if (status==CL_STATUS_ARMED)
            CL_Arm();
        else if (status == CL_STATUS_DIRECT)
            CL_SetDirectMode();
        {
            /* code */
        }
        
    }
}