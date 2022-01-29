#include "PID.h"
#include "IMU.h"
#include "time/time.h"
#include "helpers/quaternions.h"
#include "IO/thrusters.h"
#include "IO/controls.h"

#define MAX_OUTPUT 2000
#define MIN_OUTPUT 1000

typedef struct
{
    float P;
    float I;
    float D;
    float ITerm;
    float lastError;
    float windup;
    float output;
} PID_t;

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

static PID_t pid_roll;
static PID_t pid_pitch;
static PID_t pid_yaw;
static float pitch_level_gain;
static float roll_level_gain;
static float yaw_level_gain;
static float roll_rate;
static float pitch_rate;
static float yaw_rate;
quaternion_t Qref;
quaternion_t Qerror;
quaternion_t *QImu;
float ref_ang_velocity[3];
static timeUs_t last_time = 0;
thrusters_pwm_t thrusters_out;
void StableMode(float dt);
void AcroMode(float dt);
void AutonomyMode(float dt);
void update_motors();

void (*modeFun[CONTROL_MODE_COUNT])(float dt)= 
{
[CONTROL_MODE_STABLE] = StableMode,
[CONTROL_MODE_ACRO] = AcroMode,
[CONTROL_MODE_AUTONOMY] = AutonomyMode
};

void PID_update(PID_t *pid, float error, float dt)
{
    float DTerm = 0;

    pid->ITerm += dt * error;
    if (pid->ITerm > pid->windup)
        pid->ITerm = pid->windup;
    else if (pid->ITerm < -pid->windup)
        pid->ITerm = -pid->windup;

    if (dt > 0)
    {
        DTerm = (error - pid->lastError) / dt;
    }

    pid->output = error * pid->P + pid->ITerm * pid->I + DTerm * pid->D;
    pid->lastError = error;
}

void PID_reset(PID_t *pid)
{
    pid->lastError = 0;
    pid->ITerm = 0;
}

void PID_initialize(PID_t *pid, float P, float I, float D, float windup)
{
    PID_reset(pid);
    pid->P = P;
    pid->I = I;
    pid->D = D;
    pid->windup = windup;
}
void PID_Init()
{
    PID_initialize(&pid_roll, 400, 1.5, 1, 10);
    PID_initialize(&pid_pitch, 400, 1.5, 1.5, 10);
    PID_initialize(&pid_yaw, 40, 0, 0, 10);
    initThrusters();

    pitch_level_gain = 50;
    roll_level_gain = 50;
    yaw_level_gain = 50;
}

void AutonomyMode(float dt)
{

}
void AcroMode(float dt)
{
    CONTROLS_acro_mode_input_t input = CONTROLS_ACRO_GetUserInput();
    ref_ang_velocity[0] = input.ref_roll_v*roll_rate;
    ref_ang_velocity[1] = input.ref_pitch_v*pitch_rate;
    ref_ang_velocity[2] = input.ref_yaw_v*yaw_rate;
}
void StableMode(float dt) //dt is us
{
    float r[3];
    CONTROLS_angle_mode_input_t input = CONTROLS_ANGLE_GetUserInput();
    QImu = IMU_GetOrientationQ();
    Q_toEuler(QImu,r);
   Q_fromEuler(input.ref_roll, input.ref_pitch, r[2], &Qref); //set reference position to 0, 0, 0

    Q_conj(QImu);
    Q_multiply(QImu, &Qref, &Qerror);
    Q_conj(QImu);

    ref_ang_velocity[0] = Qerror.i * dt * roll_level_gain;
    ref_ang_velocity[1] = Qerror.j * dt * pitch_level_gain;
    ref_ang_velocity[2] = Qerror.k * dt * yaw_level_gain;
}

void PID_TaskFun(timeUs_t t)
{
    float dt = (t - last_time)/(float)1000000;
    last_time = t;

    modeFun[CONTROLS_GetMode()](dt);
    PID_update(&pid_roll, ref_ang_velocity[0], dt);
    PID_update(&pid_pitch, ref_ang_velocity[1], dt);
    PID_update(&pid_yaw, ref_ang_velocity[2], dt);
    update_motors();
}

void controll_roll()
{
    thrusters_out.T2 += pid_roll.output;
    thrusters_out.T3 += pid_roll.output;
    thrusters_out.T6 -= pid_roll.output;
    thrusters_out.T7 -= pid_roll.output;
}
void controll_pitch()
{
    thrusters_out.T2 -= pid_pitch.output;
    thrusters_out.T3 += pid_pitch.output;
    thrusters_out.T6 += pid_pitch.output;
    thrusters_out.T7 -= pid_pitch.output;
}
void controll_yaw()
{
    thrusters_out.T1 -= pid_yaw.output;
    thrusters_out.T4 -= pid_yaw.output;
    thrusters_out.T5 += pid_yaw.output;
    thrusters_out.T8 += pid_yaw.output;
}
void update_motors()
{
    resetThrusters_pwm_t(&thrusters_out);
    controll_roll();
    controll_pitch();
    //controll_yaw();
    if (thrusters_out.T1 > MAX_OUTPUT)
        thrusters_out.T1 = MAX_OUTPUT;
    else if (thrusters_out.T1 < MIN_OUTPUT)
        thrusters_out.T1 = MIN_OUTPUT;

    if (thrusters_out.T2 > MAX_OUTPUT)
        thrusters_out.T2 = MAX_OUTPUT;
    else if (thrusters_out.T2 < MIN_OUTPUT)
        thrusters_out.T2 = MIN_OUTPUT;

    if (thrusters_out.T3 > MAX_OUTPUT)
        thrusters_out.T3 = MAX_OUTPUT;
    else if (thrusters_out.T3 < MIN_OUTPUT)
        thrusters_out.T3 = MIN_OUTPUT;

    if (thrusters_out.T4 > MAX_OUTPUT)
        thrusters_out.T4 = MAX_OUTPUT;
    else if (thrusters_out.T4 < MIN_OUTPUT)
        thrusters_out.T4 = MIN_OUTPUT;

    if (thrusters_out.T5 > MAX_OUTPUT)
        thrusters_out.T5 = MAX_OUTPUT;
    else if (thrusters_out.T5 < MIN_OUTPUT)
        thrusters_out.T5 = MIN_OUTPUT;

    if (thrusters_out.T6 > MAX_OUTPUT)
        thrusters_out.T6 = MAX_OUTPUT;
    else if (thrusters_out.T6 < MIN_OUTPUT)
        thrusters_out.T6 = MIN_OUTPUT;

    if (thrusters_out.T7 > MAX_OUTPUT)
        thrusters_out.T7 = MAX_OUTPUT;
    else if (thrusters_out.T7 < MIN_OUTPUT)
        thrusters_out.T7 = MIN_OUTPUT;

    if (thrusters_out.T8 > MAX_OUTPUT)
        thrusters_out.T8 = MAX_OUTPUT;
    else if (thrusters_out.T8 < MIN_OUTPUT)
        thrusters_out.T8 = MIN_OUTPUT;
    setThrusters(&thrusters_out);
}

thrusters_pwm_t *PID_GetMotorOutputs()
{
    return &thrusters_out;
}