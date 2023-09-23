#pragma once
#include "drivers/USART.h"
#include "Limits.h"
#include "Common/PID.h"
#include "scheduler/scheduler.h"


#define TIME_MS(x) (1000*x)


/* Watchdog configuration*/

#define HEART_BEAT_PACKET //enables or disables heartbeat msg sended via commhandler by system task


/*comunication config

UART2 is marked as UM7 on TCM module
UART1 is usb port on TCM module
UART3 is marked as external UART
*/
#define USART1_BAUD 115200
#define USART2_BAUD 115200 
#define USART3_BAUD 115200

#define CONFIG_COMM_HANDLER_BUFFER_LEN     254
#define CONFIG_COMM_HANDLER_USART          UART3

#define CONFIG_UM7_RX_BUFFER_SIZE          100    
#define CONFIG_UM7_TX_BUFFER_SIZE          40
#define CONFIG_UM7_USART                   UART2


/* automations config*/
#define AUTOMATIONS_STICK_TIMEOUT           TIME_MS(400)
#define AUTOMATIONS_COMMHANDLER_TIMEOUT     TIME_MS(400)



typedef struct 
{
    PID_t pid_roll;
    float roll_gain;
    PID_t pid_pitch;
    float pitch_gain;
    PID_t pid_yaw;
    float yaw_gain;
    PID_t pid_depth;
    LIMITS_t limits;
    float ctrl_matrix[6*8];
    uint32_t task_frequency[TASK_COUNT];
}CONFIG_Container_t;



void CONFIG_LoadDefault();

void CONFIG_LoadFromFlash();
void CONFIG_SaveToFlash();
CONFIG_Container_t* CONFIG_GetCurrentConfig();