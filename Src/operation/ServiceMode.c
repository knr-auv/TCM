#include <stdint.h>
#include "scheduler/scheduler.h"
#include "ControlLoop.h"
#include "helpers/CommunicationProtocol/communication_protocol.h"
#include "CommunicationHandler.h"
#include "DirectThrustersCtrl.h"
#include "Config/memory.h"

bool ServiceModeActive = false;

void EnterServiceMode();




void SERVICE_HandleMsg(COMPROTO_msg_info_t *msg)
{
    if(msg->specific == MSG_OKON_SERVICE_NEW_PIDS)
    {
        CL_LoadPIDs(msg->data, msg->len);
        COMHANDLER_SendConfirmation(MSG_OKON_SERVICE_NEW_PIDS);
        return;
    }

    if(CL_GetStatus()==CL_STATUS_ARMED)
        return;

    if(msg->specific == MSG_OKON_SERVICE_ENTER)
    {
        EnterServiceMode();
        COMHANDLER_SendConfirmation(MSG_OKON_SERVICE_ENTER);
        return;
    }
    if(ServiceModeActive)
    {
        if(msg->specific == MSG_OKON_SERVICE_REBOOT)
        {
            COMHANDLER_SendConfirmation(MSG_OKON_SERVICE_REBOOT);
            NVIC_SystemReset();
        }
        else if(msg->specific == MSG_OKON_SERVICE_UPDATE_CL_MATRIX)
        {
            CL_LoadControlThrustersMatrix(msg->data, msg->len);
            COMHANDLER_SendConfirmation(MSG_OKON_SERVICE_UPDATE_CL_MATRIX);
        }
        else if (msg->specific == MSG_OKON_SERVICE_ENABLE_DIRECT_MOTORS_CTRL)
        {
            enableTask(TASK_DIRECT_MOTORS_CTRL, true);
            COMHANDLER_SendConfirmation(MSG_OKON_SERVICE_ENABLE_DIRECT_MOTORS_CTRL);
        }
        else if (msg->specific == MSG_OKON_SERVICE_DISABLE_DIRECT_MOTORS_CTRL)
        {
            enableTask(TASK_DIRECT_MOTORS_CTRL, false);
            COMHANDLER_SendConfirmation(MSG_OKON_SERVICE_DISABLE_DIRECT_MOTORS_CTRL);
        }
        else if (msg->specific == MSG_OKON_SERVICE_DIRECT_THRUSTERS_CTRL)
        {
            DTCTRL_HandlNewDirectThrustersValues((float*)msg->data);
        }
        else if (msg->specific == MSG_OKON_SERVICE_DIRECT_MATRIX_THRUSTERS_CTRL)
        {
            DTCTRL_HandleNewDirectMatrixThrustersValues((float*)msg->data);
        }
        else if(msg->specific == MSG_OKON_SERVICE_SAVE_SETTINGS)
        {
            MEM_SaveSettings();
            COMHANDLER_SendConfirmation(MSG_OKON_SERVICE_SAVE_SETTINGS);
        }
        else if(msg->specific == MSG_OKON_SERVICE_LOAD_SETTINGS)
        {
            MEM_LoadSettings();
            COMHANDLER_SendConfirmation(MSG_OKON_SERVICE_LOAD_SETTINGS);
        }
        
    }

}

void EnterServiceMode()
{
    ServiceModeActive = true;
	enableTask(TASK_CONTROL_LOOP, false);
	enableTask(TASK_AUTOMATIONS, false);
}
