#include <stdbool.h>
#include "controls.h"
#include "time/time.h"


timeUs_t lastValidInput;
CONTROLS_flight_mode_e mode = 0;
bool armed;


static uint16_t calculateChecksum(uint8_t* data, uint16_t len)
{
    uint16_t sum = 0;
    for(uint16_t i =0; i< len;i++)
    {
        sum+= data[i];
    }
    return sum;
}
void CONTROLS_Parse_Stable_Mode(uint8_t* user_input, uint16_t len)
{

}
void CONTROLS_Parse_Acro_Mode(uint8_t* user_input, uint16_t len)
{
    
}
void (*modeParse[CONTROL_MODE_COUNT])(uint8_t* user_input, uint16_t len)= 
{
[CONTROL_MODE_STABLE] = CONTROLS_Parse_Stable_Mode,
[CONTROL_MODE_ACRO] = CONTROLS_Parse_Acro_Mode,
//[CONTROL_MODE_AUTONOMY] = AutonomyMode
};
void CONTROLS_Parse_UserInput(uint8_t* user_input)
{
    uint16_t dataLen =  user_input[0] | (user_input[1]<<8);
    uint16_t checksum = (user_input[dataLen - 1]<<8)|user_input[dataLen - 2];
    if(checksum!=calculateChecksum(user_input, dataLen + 3))
    {
        return;
    }
    mode = (user_input[2] & 0b11);
    armed = (user_input[2]>>2)&0b1;
    modeParse[mode](user_input + 3, dataLen);
}

CONTROLS_flight_mode_e CONTROLS_GetMode()
{
    return mode;
}
CONTROLS_angle_mode_input_t CONTROLS_ANGLE_GetUserInput()
{
CONTROLS_angle_mode_input_t r;
r.ref_pitch=0;
r.ref_roll=0;
r.ref_yaw =0;
return r;
}
CONTROLS_acro_mode_input_t CONTROLS_ACRO_GetUserInput()
{
CONTROLS_acro_mode_input_t r;

return r;
}
bool CONTROLS_CanSwim()
{
    return armed;
}