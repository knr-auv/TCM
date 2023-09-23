#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "ControlLoop.h"
#include "IMU.h"
#include "time/time.h"
#include "Common/quaternions.h"
#include "Common/PID.h"
#include "Common/common.h"
#include "IO/actuators.h"

#include "IO/Sticks.h"
#include "Config/Limits.h"
#include "DirectThrustersCtrl.h"
#include "Variables/variables.h"
#include "Config/config.h"
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
#define CONTROL_OUTPUTS 6


static float* control_actuators_matrix;
static float control_out[CONTROL_OUTPUTS];
float actuators_out[ACTUATORS_COUNT];
uint16_t PWMout[ACTUATORS_COUNT];

static cl_status_e status;
static cl_mode_e mode;

static PID_t pid_roll;
static PID_t pid_pitch;
static PID_t pid_yaw;
static PID_t pid_vertical;
static float pitch_level_gain;
static float roll_level_gain;
static float yaw_level_gain;
static LIMITS_t* limits;

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



void CL_Init()
{
    CONFIG_Container_t* config = CONFIG_GetCurrentConfig();
    status = CL_STATUS_DISARMED;
    mode = CL_MODE_STABLE;
    // memcpy(control_actuators_matrix, config->ctrl_matrix, ACTUATORS_COUNT*CONTROL_OUTPUTS*sizeof(float));
    // memcpy(&limits, &config->limits, sizeof(LIMITS_t));
    control_actuators_matrix = config->ctrl_matrix;
    limits = &config->limits;
    ang_vel= IMU_GetAngVelPointer();
    PID_initialize(&pid_roll, config->pid_roll.P, config->pid_roll.I, config->pid_roll.D, config->pid_roll.IMax);
    PID_initialize(&pid_pitch, config->pid_pitch.P, config->pid_pitch.I, config->pid_pitch.D, config->pid_pitch.IMax);
    PID_initialize(&pid_yaw, config->pid_yaw.P, config->pid_yaw.I, config->pid_yaw.D, config->pid_yaw.IMax);
    initActuators();

    pitch_level_gain = config->pitch_gain;
    roll_level_gain = config->roll_gain;
    yaw_level_gain = config->yaw_gain;

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
    float roll_input = COMMON_map(stick_input[0],-1.f,1.f,limits->roll_rate_min,limits->roll_rate_max);
    float pitch_input = COMMON_map(stick_input[1],-1.f,1.f,-limits->pitch_rate_min,limits->pitch_rate_max);
    float yaw_input = COMMON_map(stick_input[2],-1.f,1.f,-limits->yaw_rate_min,limits->yaw_rate_max);
    
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
    float roll_input = COMMON_map(stick_input[0],-1.f,1.f,-limits->roll,limits->roll);
    float pitch_input = COMMON_map(stick_input[1],-1.f,1.f,-limits->pitch,limits->pitch);
    float direct_yaw_input = COMMON_map(stick_input[2],-1.f,1.f,-limits->yaw_rate_min,limits->yaw_rate_max);
    QImu = IMU_GetOrientationQ();
    Q_toEuler(QImu,r);
    Q_fromEuler(roll_input, pitch_input, r[2], &Qref); //set reference position to 0, 0, 0

    Q_conj(QImu);
    Q_multiply(QImu, &Qref, &Qerror);
    Q_conj(QImu);

    ref_ang_velocity[0] = Qerror.i * dt * roll_level_gain;
    ref_ang_velocity[1] = Qerror.j * dt * pitch_level_gain;
    ref_ang_velocity[2] = direct_yaw_input;//Qerror.k * dt * yaw_level_gain;
    control_out[3] = stick_input[3]*0.3f;
    control_out[4] = stick_input[4]*0.3f;
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
void CL_SetMode(cl_mode_e _mode)
{
    mode = _mode;
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

void CL_SetActuatorMatrix(float* data, uint16_t len)
{
    if(len!=CONTROL_OUTPUTS*ACTUATORS_COUNT)
        return;
    for(uint8_t i =0; i<len;i++ )
        control_actuators_matrix[i] = data[i];
}

void CL_GetPID(PID_t* roll,float* roll_gain, PID_t* pitch, float* pitch_gain, PID_t* yaw, float* yaw_gain, PID_t* vertical)
{
    memcpy(roll, &pid_roll,sizeof(PID_t));
    memcpy(pitch, &pid_pitch,sizeof(PID_t));
    memcpy(yaw, &pid_yaw,sizeof(PID_t));
    memcpy(vertical, &pid_vertical,sizeof(PID_t));
    *roll_gain = roll_level_gain;
    *pitch_gain = pitch_level_gain;
    *yaw_gain = yaw_level_gain;
}
void CL_SetPID(PID_t* roll,float roll_gain, PID_t* pitch, float pitch_gain, PID_t* yaw, float yaw_gain, PID_t* vertical)
{
    memcpy(&pid_roll, roll,sizeof(PID_t));
    memcpy(&pid_pitch, pitch,sizeof(PID_t));
    memcpy(&pid_yaw, yaw,sizeof(PID_t));
    memcpy(&pid_vertical, vertical,sizeof(PID_t));
    roll_level_gain = roll_gain;
    pitch_level_gain = pitch_gain;
    yaw_level_gain = yaw_gain;
}