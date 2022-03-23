#pragma once
#include "drivers/USART.h"
#define TIME_MS(x) (1000*x)

#define USART1_BAUD 115200
#define USART2_BAUD 115200 
#define USART3_BAUD 115200


/* automations config*/
#define AUTOMATIONS_STICK_TIMEOUT           TIME_MS(400)
#define AUTOMATIONS_COMMHANDLER_TIMEOUT     TIME_MS(400)

#define SYSTEM_HEART_BEAT_ENABLE

#define CONFIG_COMM_HANDLER_BUFFER_LEN     254
#define CONFIG_COMM_HANDLER_USART          UART1

#define CONFIG_UM7_RX_BUFFER_SIZE          100    
#define CONFIG_UM7_TX_BUFFER_SIZE          40
#define CONFIG_UM7_USART                   UART2