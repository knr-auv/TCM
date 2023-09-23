#pragma once

typedef struct
{
    float roll;
    float pitch;
    float roll_rate_min;
    float roll_rate_max;
    float pitch_rate_min;
    float pitch_rate_max;
    float yaw_rate_min;
    float yaw_rate_max;
    float vertical_speed_min;
    float vertical_speed_max;
    float longitudinal_speed_min;
    float longitudinal_speed_max;
    float lateral_speed_min;
    float lateral_speed_max;
}LIMITS_t;

