#include <string.h>
#include <stdlib.h>
#include "ControlLoop.h"
#include "IMU.h"
#include "time/time.h"
#include "helpers/quaternions.h"
#include "helpers/PID.h"
#include "helpers/common.h"
#include "IO/thrusters.h"
#include "systemControl.h"
#include "IO/Sticks.h"
#include "Config/Limits.h"
#include "DirectThrustersCtrl.h"
#define MAX_OUTPUT 1.f
#define MIN_OUTPUT -1.f



/*
Initializing:
1. Read PIDs from flash memory, if failed use default
2. Read roll, pitch limits from flash, otherwise defaults
3. Read ang vel limits from flash

Control:
1. Get user input (also check if armed - )
2. Get acc data
2. calculate diference between reference position and actual position
3. Treating this diference as reference ang. velocity use (ref. ang. vel - ang. vel) as error
4. calculate motors output
5. If ang vel limit exceed lower the values
6. If ang vel above critical disarm
7. set motors.

*/

cl_status_e status;
cl_mode_e mode;

static PID_t pid_roll;
static PID_t pid_pitch;
static PID_t pid_yaw;
static float pitch_level_gain;
static float roll_level_gain;
static float yaw_level_gain;

quaternion_t Qref;
quaternion_t Qerror;
quaternion_t *QImu;
float ref_ang_velocity[3];
static timeUs_t last_time = 0;
float control_out[THRUSTERS_COUNT];

void StableMode(float dt);
void AcroMode(float dt);
void AutonomyMode(float dt);
void update_outputs();
void set_motors();

void (*modeFun[CL_MODE_COUNT])(float dt)= 
{
[CL_MODE_STABLE] = StableMode,
[CL_MODE_ACRO] = AcroMode
};


void CL_Init()
{
    status = CL_STATUS_DISARMED;
    mode = CL_MODE_STABLE;

    PID_initialize(&pid_roll, 400, 1.5, 1, 10);
    PID_initialize(&pid_pitch, 400, 1.5, 1.5, 10);
    PID_initialize(&pid_yaw, 40, 0, 0, 10);
    initThrusters();

    pitch_level_gain = 50;
    roll_level_gain = 50;
    yaw_level_gain = 50;
}


void AcroMode(float dt)
{
    float* stick_input = STICK_GetSticks();
    float roll_input = COMMON_map(stick_input[0],-1.f,1.f,-LIMITS_GetRollRateLimit(),LIMITS_GetRollRateLimit());
    float pitch_input = COMMON_map(stick_input[1],-1.f,1.f,-LIMITS_GetPitchRateLimit(),LIMITS_GetPitchRateLimit());
    float yaw_input = COMMON_map(stick_input[2],-1.f,1.f,-LIMITS_GetYawRateLimit(),LIMITS_GetYawRateLimit());
    
    ref_ang_velocity[0] = roll_input;
    ref_ang_velocity[1] = pitch_input;
    ref_ang_velocity[2] = yaw_input;
}
void StableMode(float dt) //dt is us
{
    float r[3];
    float* stick_input = STICK_GetSticks();
    float roll_input = COMMON_map(stick_input[0],-1.f,1.f,-LIMITS_GetRollLimit(),LIMITS_GetRollLimit());
    float pitch_input = COMMON_map(stick_input[1],-1.f,1.f,-LIMITS_GetPitchLimit(),LIMITS_GetPitchLimit());
    float direct_yaw_input = COMMON_map(stick_input[2],-1.f,1.f,-LIMITS_GetYawRateLimit(),LIMITS_GetYawRateLimit());
    QImu = IMU_GetOrientationQ();
    Q_toEuler(QImu,r);
    Q_fromEuler(roll_input, pitch_input, r[2], &Qref); //set reference position to 0, 0, 0

    Q_conj(QImu);
    Q_multiply(QImu, &Qref, &Qerror);
    Q_conj(QImu);

    ref_ang_velocity[0] = Qerror.i * dt * roll_level_gain;
    ref_ang_velocity[1] = Qerror.j * dt * pitch_level_gain;
    ref_ang_velocity[2] = direct_yaw_input;//Qerror.k * dt * yaw_level_gain;
}

void CL_TaskFun(timeUs_t t)
{
    switch (status)
    {
    case CL_STATUS_ARMED:
    {
        float dt = (t - last_time)/(float)1000000;
        last_time = t;
        modeFun[mode](dt);
        PID_update(&pid_roll, ref_ang_velocity[0], dt);
        PID_update(&pid_pitch, ref_ang_velocity[1], dt);
        PID_update(&pid_yaw, ref_ang_velocity[2], dt);
        update_outputs();
        break;
    }
    case CL_STATUS_DIRECT:
        if(DTCTRL_Timeout())
            {
                CL_Disarm();
                return;
            }
        float* v = DTCRTL_GetValues();
        for(uint8_t i =0; i<8;i++)
            control_out[i] = v[i];
        break;
        
    default:
        return;
    }

    set_motors();
}

void controll_roll()
{
    control_out[1] += pid_roll.output;
    control_out[2] += pid_roll.output;
    control_out[5] -= pid_roll.output;
    control_out[6] -= pid_roll.output;
}
void controll_pitch()
{
    control_out[1] -= pid_pitch.output;
    control_out[2] += pid_pitch.output;
    control_out[5] += pid_pitch.output;
    control_out[6] -= pid_pitch.output;
}
void controll_yaw()
{
    control_out[0] -= pid_yaw.output;
    control_out[3] -= pid_yaw.output;
    control_out[4] += pid_yaw.output;
    control_out[7] += pid_yaw.output;
}
void update_outputs()
{
    for(uint8_t i = 0;i<THRUSTERS_COUNT;i++)
    {
        control_out[i] = 0.f;
    }
    controll_roll();
    controll_pitch();
    //controll_yaw();
    for(uint8_t i = 0;i<THRUSTERS_COUNT;i++)
    {
        if (control_out[i] > MAX_OUTPUT)
        control_out[i] = MAX_OUTPUT;
    else if (control_out[i]< MIN_OUTPUT)
        control_out[i] = MIN_OUTPUT;
    }
    
}
void set_motors()
{
    uint16_t out[THRUSTERS_COUNT];
    for(uint8_t i =0; i<THRUSTERS_COUNT;i++)
        out[i] = THRUSTERS_map(control_out[i], MIN_OUTPUT, MAX_OUTPUT);
    setThrusters(out);
}

void CL_Arm()
{
    if(status!=CL_STATUS_DISARMED)
        return;
    status = CL_STATUS_ARMED;
    THRUSTER_Enable();
}
void CL_Disarm()
{
    status = CL_STATUS_DISARMED;
    THRUSTERS_Disable();
}
void CL_SetMode(cl_mode_e mode)
{
    mode = mode;
}
void CL_SetDirectMode()
{
    if(status!=CL_STATUS_DISARMED)
        return;
    status = CL_STATUS_DIRECT;
    THRUSTER_Enable();
}
float *CL_GetMotorOutputs()
{
    return control_out;
}


void CL_SerializePIDs(uint8_t* buffer, uint16_t* len)
{
    //5*floatstatic PID_t pid_roll;
  
    *len =5*3*sizeof(float);

    memcpy(buffer,&roll_level_gain, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer,&pid_roll.P, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer,&pid_roll.I, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer,&pid_roll.D, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer,&pid_roll.windup, sizeof(float));
    buffer += sizeof(float);

    memcpy(buffer,&pitch_level_gain, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer,&pid_pitch.P, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer,&pid_pitch.I, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer,&pid_pitch.D, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer,&pid_pitch.windup, sizeof(float));
    buffer += sizeof(float);

    memcpy(buffer,&yaw_level_gain, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer,&pid_yaw.P, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer,&pid_yaw.I, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer,&pid_yaw.D, sizeof(float));
    buffer += sizeof(float);
    memcpy(buffer,&pid_yaw.windup, sizeof(float));
}
void CL_LoadPIDs(uint8_t* buffer, uint16_t len)
{
    if(len<15*sizeof(float))
        return;
    memcpy(&roll_level_gain,buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_roll.P,buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_roll.I,buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_roll.D,buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_roll.windup,buffer, sizeof(float));
    buffer += sizeof(float);

    memcpy(&pitch_level_gain,buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_pitch.P,buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_pitch.I,buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_pitch.D,buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_pitch.windup,buffer, sizeof(float));
    buffer += sizeof(float);

    memcpy(&yaw_level_gain,buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_yaw.P,buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_yaw.I,buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_yaw.D,buffer, sizeof(float));
    buffer += sizeof(float);
    memcpy(&pid_yaw.windup,buffer, sizeof(float));
}