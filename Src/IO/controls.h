#pragma once
#include "helpers/quaternions.h"
#include <stdbool.h>
typedef enum
{
    CONTROL_MODE_STABLE,
    CONTROL_MODE_ACRO,
    CONTROL_MODE_AUTONOMY,
    CONTROL_MODE_COUNT
}CONTROLS_flight_mode_e;

typedef struct
{
    float ref_roll;
    float ref_pitch;
    float ref_yaw;
}CONTROLS_angle_mode_input_t;

typedef struct
{
    float ref_roll_v;
    float ref_pitch_v;
    float ref_yaw_v;
}CONTROLS_acro_mode_input_t;

CONTROLS_flight_mode_e CONTROLS_GetMode();
CONTROLS_angle_mode_input_t CONTROLS_ANGLE_GetUserInput();
CONTROLS_acro_mode_input_t CONTROLS_ACRO_GetUserInput();
bool CONTROLS_CanSwim();
