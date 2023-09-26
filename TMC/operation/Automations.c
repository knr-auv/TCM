#include "time/time.h"
#include "IO/Sticks.h"
#include "Config/config.h"
#include "operation/ControlLoop.h"
#include "communication/CommunicationHandler.h"

void HandleStickTimeout();
void HandleCommHandlerTimeout();
void StickArm();
void Mission();

timeUs_t missionStartTime = 0;

bool missionShouldStart = false;
bool missionStarted = false;
void AUTOMATIONS_Task(timeUs_t t)
{

    if(!missionStarted){
    HandleStickTimeout();
    HandleCommHandlerTimeout();
    StickArm();
    }
}

int counter = 0;
enum
{
FORWARD_1,
TURN_LEFT,
FORWARD_2,
TURN_RIGHT
}Stage;



void StickArm()
{
    float* v =STICK_GetSticks();
    if(v[0]>0.5f)
    {
        CL_Arm();
    }else{
        CL_Disarm();
    }
}

void HandleStickTimeout()
{
    if(STICK_TimeSinceLastUpdate()>AUTOMATIONS_STICK_TIMEOUT)
    {
         STICK_ResetSticks();
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