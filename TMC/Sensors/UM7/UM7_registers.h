#ifndef UM7_REGISTERS_H
#define UM7_REGISTERS_H
/*
Packet structure:
's'|'n'|'p'|packet_type|Address|data|checksum1|checksum0
///
Read operation:
has_data = 0, addrress = register to read
Write operation:
has_data = 1, address = register to write
*****Calculating checksum****
    sum = 's' + 'n' + 'p' + packet_type + Address + data
    checksum1 = (sum>>8)
    checksum = sum
*****************************
*/
//this function handles data and converts it to packet
#define CREG_COM_SETTINGS           0x00
#define CREG_COM_RATES1             0x01
#define CREG_COM_RATES2             0x02
#define CREG_COM_RATES3             0x03
#define CREG_COM_RATES4             0x04
#define CREG_COM_RATES5             0x05
#define CREG_COM_RATES6             0x06
#define CREG_COM_RATES7             0x07
#define CREG_MISC_SETTINGS          0x08
#define CREG_GYRO_TRIM_X            0x0C
#define CREG_GYRO_TRIM_Y            0x0D
#define CREG_GYRO_TRIM_Z            0x0E

#define DREG_HEALTH                 0x55
#define DREG_GYRO_RAW_XY            0x56
#define DREG_GYRO_RAW_Z             0x57
#define DREG_GYRO_TIME              0x58
#define DREG_ACCEL_RAW_XY           0x59
#define DREG_ACCEL_RAW_Z            0x5A
#define DREG_ACCEL_TIME             0x5B
#define DREG_MAG_RAW_XY             0x5C
#define DREG_MAG_RAW_Z              0x5D
#define DREG_MAG_RAW_TIME           0x5E
#define DREG_TEMPERATURE            0x5F
#define DREG_TEMPERATURE_TIME       0x60
#define DREG_GYRO_PROC_X            0x61
#define DREG_GYRO_PROC_Y            0x62
#define DREG_GYRO_PROC_Z            0x63
#define DREG_GYRO_PROC_TIME         0x64
#define DREG_ACCEL_PROC_X           0x65
#define DREG_ACCEL_PROC_Y           0x66
#define DREG_ACCEL_PROC_Z           0x67
#define DREG_ACCEL_PROC_TIME        0x68
#define DREG_MAG_PROC_X             0x69
#define DREG_MAG_PROC_Y             0x6A
#define DREG_MAG_PROC_Z             0x6B
#define DREG_MAG_PROC_TIME          0x6C
#define DREG_QUAT_AB                0x6D
#define DREG_QUAT_CD                0x6E
#define DREG_QUAT_TIME              0x6F
#define DREG_EULER_PHI_THETA        0x70
#define DREG_EULER_PSI              0x71
#define DREG_EULER_PSI_THETA_DOT    0x72
#define DREG_EULER_PSI_DOT          0x73
#define DREG_EULER_TIME             0x74

#define GET_FW_VERSION              0xAA
#define ZERO_GYROS                  0xAD
#define RESET_EKF                   0xB3
#define RESET_TO_FACTORY            0xAC

#define PT_CF       0     //Must be 0 for all packets written to the UM7
#define PT_Hidden   1     //If set the packet address is hidden address (for factory calibration)
#define PT_BL0      2     //Batch lenght
#define PT_BL1      3     //Batch lenght
#define PT_BL2      4     //Batch lenght
#define PT_BL3      5     //Batch lenght
#define PT_IS_BATCH 6     //If packet is batch operation the bit is set
#define PT_HAS_DATA 7     //If packet contains data the bit is set

typedef enum{
    PI_GOOD,
    PI_BAD
}packet_info_e;

typedef struct{
    uint8_t address;
    bool is_batch;
    bool has_data;
    uint8_t data_lenght;
    uint8_t data[30];
}UM7_packet_t;

packet_info_e UM7_HandleSerialData(uint8_t* data, uint16_t len, UM7_packet_t* packet);
/* MISC OPTIONS
PPS -> for gps
ZG->measure gyro bias at startup
Q ->enable quaternion mode
MAG -> use magnetometer in state updates
*/
void UM7_MISC(bool pps, bool zg, bool mag, bool quatMode);
/*functions for setting broadcast rate. from 0 to 255Hz*/
void UM7_RAW_Rates(uint8_t acc, uint8_t gyro, uint8_t mag);         //RAW data  
void UM7_PROCCESED_Rates(uint8_t acc, uint8_t gyro, uint8_t mag);   //processed data
void UM7_QUAT_EUL_POS_VEL_Rates(uint8_t quat, uint8_t eul, uint8_t pos, uint8_t vel);
void UM7_NMEA_PACKETS(uint8_t health, uint8_t pose, uint8_t att, uint8_t sensor);
void UM7_FACTORY_RESET(void);
void UM7_ZERO_GYROS(void);
void UM7_HEALTH_RATE(uint8_t health);
#endif