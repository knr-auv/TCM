#include <string.h>
#include "telemetry_debug.h"
#include "operation/ControlLoop.h"
#include "Sensors/analog_sensors.h"
#include "operation/IMU.h"
#include "helpers/quaternions.h"
#include "drivers/USART1.h"

#define HEADER_SIZE 3
#define CHECKSUM_VAR uint16_t
#define CHECKSUM_SIZE sizeof(uint16_t)
#define HEADER      0x69
#define THRUSTERS   0x1
#define SENSORS     0x2
#define ORIENTATION 0x3

uint8_t tx_buffer[255];
uint8_t* header_buffer = tx_buffer;
uint8_t* msg_buffer = tx_buffer + HEADER_SIZE;

void constructHeader(uint8_t msg_len)
{
    header_buffer[0] = HEADER;
    header_buffer[1] = HEADER - 1;
    header_buffer[2] = msg_len;
}
/*
debug telemetry frame
sensors data:
    cpu temp
    onboard temp
    onboard hummidity
thruster data:
    t1
    .
    t8
IMU data
    Orientation quaternion
*/

uint16_t createFrame()
{
    /*
    2bytes header 0x69 len
    12 bytes sensor data - float
    16 bytes thrusters
    16 bytes orientation
    2bytes of checksum
    */
    float analog_sensors[3];
    analog_sensors[0] = ANALOG_CPUTemp();
    analog_sensors[1] = ANALOG_onboardTemp();
    analog_sensors[2] = ANALOG_onboardHumidity();
    uint16_t *thrusters = CL_GetThrustersValue();
    quaternion_t* orientation = IMU_GetOrientationQ();
    uint16_t len = 0;
    memcpy(msg_buffer, analog_sensors, 3*sizeof(float));
    len += 3*sizeof(float);
    memcpy(msg_buffer + len, thrusters, sizeof(uint16_t)*THRUSTERS_COUNT);
    len += sizeof(uint16_t)*THRUSTERS_COUNT;
    memcpy(msg_buffer + len, orientation, sizeof(quaternion_t));
    len += sizeof(quaternion_t);
    constructHeader(len + HEADER_SIZE+CHECKSUM_SIZE);
    CHECKSUM_VAR checksum = 0;

    for(uint8_t i = 0; i<len+HEADER_SIZE; i++)
    {
        checksum+=tx_buffer[i];
    }
    memcpy(msg_buffer + len, &checksum, CHECKSUM_SIZE);
    return len + HEADER_SIZE+CHECKSUM_SIZE;
}


void TELEMETRY_DEBUG_task(timeUs_t t)
{
   // USART1_Transmit_DMA(tx_buffer, createFrame());
}

