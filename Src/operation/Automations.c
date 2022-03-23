#include "time/time.h"
#include "IO/Sticks.h"
#include "config.h"
#include "operation/ControlLoop.h"
#include "operation/CommunicationHandler.h"

void HandleStickTimeout();
void HandleCommHandlerTimeout();
void StickArm();
void Mission();

timeUs_t missionStartTime = 0;

bool missionShouldStart = false;
bool missionStarted = false;
void AUTOMATIONS_Task(timeUs_t t)
{
    Mission();
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

void Mission()
{
    if(!missionShouldStart)
        return;
    if(!missionStarted)
    {
        CL_Arm();
        missionStartTime = micros();
    }
        
    missionStarted = true;
    if(STICK_TimeSinceLastUpdate()<AUTOMATIONS_STICK_TIMEOUT)
    {
        missionShouldStart = false;
        missionStarted = false;
        counter = 0;
        Stage = FORWARD_1;
    }
    timeUs_t phaseTime = micros()-missionStartTime;
    if(phaseTime<10000000)
    {
        Stage = FORWARD_1;
    }
    else if(phaseTime<13000000)
    {
        Stage = TURN_LEFT;
    }
    else if(phaseTime<23000000)
    {
        Stage = FORWARD_2;
    }
    else if(phaseTime<26000000)
    {
        Stage = TURN_RIGHT;
    }else if(phaseTime>26000000)
    {
        missionStartTime = micros();
    }

    float sticks[5];
    switch (Stage)
    {
    case FORWARD_1:
        sticks[0] = 0;
        sticks[1] = 0;
        sticks[2] = 0;
        sticks[3] = 0;
        sticks[4] = 0.2;
        
        break;
        case TURN_LEFT:
        sticks[0] = 0;
        sticks[1] = 0;
        sticks[2] = -0.2;
        sticks[3] = 0;
        sticks[4] = 0;
        break;
        case FORWARD_2:
        sticks[0] = 0;
        sticks[1] = 0;
        sticks[2] = 0;
        sticks[3] = 0;
        sticks[4] = 0.2;
        break;
        case TURN_RIGHT:
          sticks[0] = 0;
        sticks[1] = 0;
        sticks[2] = 0.2;
        sticks[3] = 0;
        sticks[4] = 0;
        break;
    
    default:
        break;
    }
STICK_HandleNewInputFloat(sticks, 5);
}


void StickArm()
{
    float* v =STICK_GetSticks();
    if(v[5]>0.5f)
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
        float* v =STICK_GetSticks();
        if(v[6]>0.5f)
        {
            missionShouldStart = true;
        }
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