#include <stdint.h>
#include "scheduler/scheduler.h"
#include "ControlLoop.h"
#include "Communication/CommunicationHandler.h"
#include "DirectThrustersCtrl.h"

#include "IO/LED.h"
#include "tasks/HeartBeat.h"
bool ServiceModeActive = false;

void EnterServiceMode();




void SERVICE_HandleMsg()
{
    // if(msg->data[0] == MSG_SERVICE_NEW_PIDS)
    // {
    //     CL_LoadPIDs(msg->data, msg->len);
    //     COMHANDLER_SendConfirmation(MSG_SERVICE_NEW_PIDS);
    //     return;
    // }

    // if(CL_GetStatus()==CL_STATUS_ARMED)
    //     return;

    // if(msg->data[0] == MSG_SERVICE_ENTER)
    // {
    //     EnterServiceMode();
    //     COMHANDLER_SendConfirmation(MSG_SERVICE_ENTER);
    //     return;
    // }
    // if(ServiceModeActive)
    // {
    //     if(msg->data[0] == MSG_SERVICE_REBOOT)
    //     {
    //         COMHANDLER_SendConfirmation(MSG_SERVICE_REBOOT);
    //         DelayMs(10);
    //         NVIC_SystemReset();
    //     }
    //     else if(msg->data[0] == MSG_SERVICE_UPDATE_CL_MATRIX)
    //     {
    //         CL_LoadControlActuatorsMatrix(msg->data, msg->len);
    //         COMHANDLER_SendConfirmation(MSG_SERVICE_UPDATE_CL_MATRIX);
    //     }
    //     else if (msg->data[0] == MSG_SERVICE_ENABLE_DIRECT_MOTORS_CTRL)
    //     {
    //         DTCTRL_Enable();
    //         enableTask(TASK_DIRECT_MOTORS_CTRL, true);
    //         COMHANDLER_SendConfirmation(MSG_SERVICE_ENABLE_DIRECT_MOTORS_CTRL);
    //     }
    //     else if (msg->data[0] == MSG_SERVICE_DISABLE_DIRECT_MOTORS_CTRL)
    //     {

    //          DTCTRL_Disable();
    //         enableTask(TASK_DIRECT_MOTORS_CTRL, false);
    //         COMHANDLER_SendConfirmation(MSG_SERVICE_DISABLE_DIRECT_MOTORS_CTRL);
    //     }
    //     else if (msg->data[0] == MSG_SERVICE_DIRECT_ACTUATORS_CTRL)
    //     {
    //         DTCTRL_HandlNewDirectActuatorsValues((float*)msg->data);
    //     }
    //     else if (msg->data[0] == MSG_SERVICE_DIRECT_MATRIX_ACTUATORS_CTRL)
    //     {
    //         DTCTRL_HandleNewDirectMatrixActuatorsValues((float*)msg->data);
    //     }
    //     else if(msg->data[0] == MSG_SERVICE_SAVE_SETTINGS)
    //     {
    //         MEM_SaveSettings();
    //         COMHANDLER_SendConfirmation(MSG_SERVICE_SAVE_SETTINGS);
    //     }
    //     else if(msg->data[0] == MSG_SERVICE_LOAD_SETTINGS)
    //     {
    //         MEM_LoadSettings();
    //         COMHANDLER_SendConfirmation(MSG_SERVICE_LOAD_SETTINGS);
    //     }
        
    // }

}

void EnterServiceMode()
{
    ServiceModeActive = true;
	enableTask(TASK_CONTROL_LOOP, false);
	enableTask(TASK_AUTOMATIONS, false);
    LED_HeartBeat_Set(true);
    //HB_DisablePacket();
}
