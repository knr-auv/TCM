#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "ControlLoop.h"
#include "IMU.h"
#include "time/time.h"
#include "helpers/quaternions.h"
#include "helpers/PID.h"
#include "helpers/common.h"
#include "IO/actuators.h"

#include "IO/Sticks.h"
#include "Config/Limits.h"
#include "DirectThrustersCtrl.h"
#include "Variables/variables.h"
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
#define CONTROL_OUTPUTS 5


static float control_actuators_matrix[CONTROL_OUTPUTS*ACTUATORS_COUNT];
static float control_out[CONTROL_OUTPUTS];
float actuators_out[ACTUATORS_COUNT];
uint16_t PWMout[ACTUATORS_COUNT];

static cl_status_e status;
static cl_mode_e mode;

static PID_t pid_roll;
static PID_t pid_pitch;
static PID_t pid_yaw;
static float pitch_level_gain;
static float roll_level_gain;
static float yaw_level_gain;

quaternion_t Qref;
quaternion_t Qerror;
quaternion_t *QImu;
static float ref_ang_velocity[3];
static float *ang_vel;


static timeUs_t last_time = 0;


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



void initControlActuatorsMatrix()
{
    float su[] =
    {
        0.f,    0.f,   -1.f,    0.f,    0.f,
        1.f,   -1.f,    0.f,    0.f,    0.f,
        1.f,    1.f,    0.f,    0.f,    0.f,
        0.f,    0.f,   -1.f,    0.f,    0.f,
        0.f,    0.f,    1.f,    0.f,    0.f,
       -1.f,    1.f,    0.f,    0.f,    0.f,
       -1.f,   -1.f,    0.f,    0.f,    0.f,
        0.f,    0.f,    1.f,    0.f,    0.f
    };
    memcpy(control_actuators_matrix, su,CONTROL_OUTPUTS*ACTUATORS_COUNT*sizeof(float));
}
void CL_Init()
{
    status = CL_STATUS_DISARMED;
    mode = CL_MODE_STABLE;
    ang_vel= IMU_GetAngVelPointer();
    PID_initialize(&pid_roll, 4, 1.5, 1, 10);
    PID_initialize(&pid_pitch, 4, 1.5, 1.5, 10);
    PID_initialize(&pid_yaw, 4, 0, 0, 10);
    initControlActuatorsMatrix();
    initActuators();

    pitch_level_gain = 5;
    roll_level_gain = 5;
    yaw_level_gain = 5;

    VAR_SetSysFloat(&Qref.r, VAR_SYS_DESIRED_ATTITUDE_Q_r);
    VAR_SetSysFloat(&Qref.i, VAR_SYS_DESIRED_ATTITUDE_Q_i);
    VAR_SetSysFloat(&Qref.j, VAR_SYS_DESIRED_ATTITUDE_Q_j);
    VAR_SetSysFloat(&Qref.k, VAR_SYS_DESIRED_ATTITUDE_Q_k);

    VAR_SetSysFloat(&QImu->r, VAR_SYS_ATTITUDE_Q_r);
    VAR_SetSysFloat(&QImu->i, VAR_SYS_ATTITUDE_Q_i);
    VAR_SetSysFloat(&QImu->j, VAR_SYS_ATTITUDE_Q_j);
    VAR_SetSysFloat(&QImu->k, VAR_SYS_ATTITUDE_Q_k);

    VAR_SetSysFloat(&ang_vel[0], VAR_SYS_GYRO_r);
    VAR_SetSysFloat(&ang_vel[1], VAR_SYS_GYRO_p);
    VAR_SetSysFloat(&ang_vel[2], VAR_SYS_GYRO_y);
    VAR_SetSysFloat(&ref_ang_velocity[0], VAR_SYS_DESIRED_GYRO_r);
    VAR_SetSysFloat(&ref_ang_velocity[1], VAR_SYS_DESIRED_GYRO_p);
    VAR_SetSysFloat(&ref_ang_velocity[2], VAR_SYS_DESIRED_GYRO_y);

    for(uint8_t i =0; i< ACTUATORS_COUNT; i++)
    {
        VAR_SetSysFloat(&actuators_out[i], VAR_SYS_ACTUATOR_OUT_1 + i);
    }
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
    control_out[3] = stick_input[3];
    control_out[4] = stick_input[4];;
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
    control_out[3] = stick_input[3]*0.3;
    control_out[4] = stick_input[4]*0.3;
}

void CL_TaskFun(timeUs_t t)
{
    float dt = (t - last_time)/(float)1000000;
    last_time = t;
    
    modeFun[mode](dt);

    PID_update(&pid_roll, ref_ang_velocity[0] - ang_vel[0], dt);
    PID_update(&pid_pitch, ref_ang_velocity[1] - ang_vel[1], dt);
    PID_update(&pid_yaw, ref_ang_velocity[2]- ang_vel[2], dt);
    control_out[0] = pid_roll.output;
    control_out[1] = pid_pitch.output;
    control_out[2] = pid_yaw.output;
    update_outputs();
    set_motors();
}


void update_outputs()
{

    COMMON_mat_vec_mul(control_actuators_matrix, control_out, actuators_out, ACTUATORS_COUNT, CONTROL_OUTPUTS);
    COMMON_linear_saturation(actuators_out,ACTUATORS_COUNT, MAX_OUTPUT);    
}

void set_motors()
{
    
    for(uint8_t i =0; i<ACTUATORS_COUNT;i++)
        PWMout[i] = ACTUATORS_map(actuators_out[i], MIN_OUTPUT, MAX_OUTPUT);
    setActuators(PWMout);
}

void CL_Arm()
{
    if(status!=CL_STATUS_DISARMED)
        return;
    status = CL_STATUS_ARMED;
    ACTUATORS_Enable();
}
void CL_Disarm()
{
    status = CL_STATUS_DISARMED;
    ACTUATORS_Disable();
}
void CL_SetMode(cl_mode_e mode)
{
    mode = mode;
}

cl_status_e CL_GetStatus()
{
    return status;
}
cl_mode_e CL_GetMode()
{
    return mode;
}
float* CL_GetActuatorsMatrix()
{
    return control_actuators_matrix;
}
uint8_t* CL_SerializeControlActuatorsMatrix(uint16_t* len)
{
    *len = CONTROL_OUTPUTS*ACTUATORS_COUNT*sizeof(float);
    uint8_t *buffer = (uint8_t*)control_actuators_matrix;
    return buffer;
}
void CL_LoadControlActuatorsMatrix(uint8_t* buffer, uint16_t len)
{
    memcpy(control_actuators_matrix, buffer, len);
}
uint8_t* CL_SerializePIDs( uint16_t* len)
{
    //5*floatstatic PID_t pid_roll;
    uint8_t* buff;
    uint8_t *head;
    *len =5*3*sizeof(float);
    buff = malloc(*len);
    head = buff;

    memcpy(buff,&roll_level_gain, sizeof(float));
    buff += sizeof(float);
    memcpy(buff,&pid_roll.P, sizeof(float));
    buff += sizeof(float);
    memcpy(buff,&pid_roll.I, sizeof(float));
    buff += sizeof(float);
    memcpy(buff,&pid_roll.D, sizeof(float));
    buff += sizeof(float);
    memcpy(buff,&pid_roll.windup, sizeof(float));
    buff += sizeof(float);

    memcpy(buff,&pitch_level_gain, sizeof(float));
    buff += sizeof(float);
    memcpy(buff,&pid_pitch.P, sizeof(float));
    buff += sizeof(float);
    memcpy(buff,&pid_pitch.I, sizeof(float));
    buff += sizeof(float);
    memcpy(buff,&pid_pitch.D, sizeof(float));
    buff += sizeof(float);
    memcpy(buff,&pid_pitch.windup, sizeof(float));
    buff += sizeof(float);

    memcpy(buff,&yaw_level_gain, sizeof(float));
    buff += sizeof(float);
    memcpy(buff,&pid_yaw.P, sizeof(float));
    buff += sizeof(float);
    memcpy(buff,&pid_yaw.I, sizeof(float));
    buff += sizeof(float);
    memcpy(buff,&pid_yaw.D, sizeof(float));
    buff += sizeof(float);
    memcpy(buff,&pid_yaw.windup, sizeof(float));
    return head;
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