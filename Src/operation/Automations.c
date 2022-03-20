#include "time/time.h"
#include "IO/Sticks.h"
#include "config.h"
#include "operation/ControlLoop.h"
#include "operation/CommunicationHandler.h"

void HandleStickTimeout();
void HandleCommHandlerTimeout();

void AUTOMATIONS_Task(timeUs_t t)
{
    HandleStickTimeout();
    HandleCommHandlerTimeout();
}

void HandleStickTimeout()
{
    if(STICK_TimeSinceLastUpdate()>AUTOMATIONS_STICK_TIMEOUT)
    {
        if(CL_GetStatus()==CL_STATUS_ARMED)
            CL_Disarm();
    }
    else
    {
        return;
    }
    
}

void HandleCommHandlerTimeout()
{
     if(COMHANDLER_TimeSinceLastUpdate()>AUTOMATIONS_COMMHANDLER_TIMEOUT)
    {
        CL_Disarm();
    }
    else
    {
        return;
    }
}