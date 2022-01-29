

#ifndef SH2_H_
#define SH2_H_



/*Handling msg from channel 0
 * error list is sent whenever error apears
 * new errors are added to the end of the list
 * error list is cleared after reset
 */
#define CHANNEL_COMMAND_GET_ADVERTISEMENT 0
#define CHANNEL_COMMAND_ERROR_LIST 1
//error list
#define ERROR_CODE_NO_ERROR 0
#define ERROR_CODE_HUB_OVERFLOW 1		//Hub application attempted to exceed maximum read cargo length
#define ERROR_CODE_HOST_WRITE_SHORT 2	//Host write was too short (need at least a 4-byte header)
#define ERROR_CODE_MSG_TO_LONG 3		//Host wrote a header with length greater than maximum write cargo length.
#define ERROR_CODE_WRONG_MSG_LENGHT 4	//Host wrote a header with length less than or equal to header length (either invalid or no payload). Note that a length of 0 is permitted, indicating “no cargo.”




#define SHTP_FLUSH_SENSOR 0xF0
#define SHTP_FLUSH_COMPLETE 0xEF
#define SHTP_REPORT_COMMAND_RESPONSE 0xF1
#define SHTP_REPORT_COMMAND_REQUEST 0xF2
#define SHTP_REPORT_FRS_READ_RESPONSE 0xF3
#define SHTP_REPORT_FRS_READ_REQUEST 0xF4
#define SHTP_REPORT_FRS_WRITE_RESPONSE 0xF5
#define SHTP_REPORT_FRS_WRITE_DATA_REQUEST 0xF6
#define SHTP_REPORT_FRS_WRITE_REQUEST 0xF7
#define SHTP_REPORT_PRODUCT_ID_RESPONSE 0xF8
#define SHTP_REPORT_PRODUCT_ID_REQUEST 0xF9
#define SHTP_REPORT_BASE_TIMESTAMP 0xFB
#define SHTP_REPORT_SET_FEATURE_COMMAND 0xFD
#define SHTP_REPORT_GET_FEATURE_REQUEST 0xFE
#define SHTP_REPORT_GET_FEATURE_RESPONSE 0xFC

#define COMMAND_RESPONSE_INITIALIZE 0x84


//All the different sensors and features we can get reports from
//These are used when enabling a given sensor
#define SENSOR_REPORTID_ACCELEROMETER 0x01
#define SENSOR_REPORTID_GYROSCOPE 0x02
#define SENSOR_REPORTID_MAGNETIC_FIELD 0x03
#define SENSOR_REPORTID_LINEAR_ACCELERATION 0x04
#define SENSOR_REPORTID_ROTATION_VECTOR 0x05
#define SENSOR_REPORTID_GRAVITY 0x06
#define SENSOR_REPORTID_GAME_ROTATION_VECTOR 0x08
#define SENSOR_REPORTID_GEOMAGNETIC_ROTATION_VECTOR 0x09
#define SENSOR_REPORTID_TAP_DETECTOR 0x10
#define SENSOR_REPORTID_STEP_COUNTER 0x11
#define SENSOR_REPORTID_STABILITY_CLASSIFIER 0x13
#define SENSOR_REPORTID_RAW_ACCELEROMETER 0x14
#define SENSOR_REPORTID_RAW_GYROSCOPE 0x15
#define SENSOR_REPORTID_RAW_MAGNETOMETER 0x16
#define SENSOR_REPORTID_PERSONAL_ACTIVITY_CLASSIFIER 0x1E
#define SENSOR_REPORTID_ARVR_STABILIZED_ROTATION_VECTOR 0x28
#define SENSOR_REPORTID_ARVR_STABILIZED_GAME_ROTATION_VECTOR 0x29
#define SENSOR_REPORTID_GYRO_INTEGRATED_GAME_ROTATION_VECTOR 0x2A

//Record IDs from figure 29, page 29 reference manual
//These are used to read the metadata for each sensor type

#define FRS_RECORDID_ACCELEROMETER 0xE302
#define FRS_RECORDID_GYROSCOPE_CALIBRATED 0xE306
#define FRS_RECORDID_MAGNETIC_FIELD_CALIBRATED 0xE309
#define FRS_RECORDID_ROTATION_VECTOR 0xE30B


#define FRS_CONFIG_GIRV 0xA1A2
#define FRS_CONFIG_ME_TIME_SOURCE_SELECTION 0xD403
#define FRS_CONFIG_ARVR_STABILIZATION_RV 0x3E2D
#define FRS_CONFIG_ARVR_STABILIZATION_GAME_RV 0x3E2E
#define FRS_CONFIG_SYSTEM_ORIENTATION 0x2D3E
#define FRS_CONFIG_TIME_SOURCE_SELECTION 0xD403


#define COMMAND_ERRORS 1
#define COMMAND_COUNTER 2
#define COMMAND_TARE 3
#define COMMAND_INITIALIZE 4
#define COMMAND_DCD 6
#define COMMAND_ME_CALIBRATE 7
#define COMMAND_DCD_PERIOD_SAVE 9
#define COMMAND_OSCILLATOR 10
#define COMMAND_CLEAR_DCD 11

#define CALIBRATE_ACCEL 0
#define CALIBRATE_GYRO 1
#define CALIBRATE_MAG 2
#define CALIBRATE_PLANAR_ACCEL 3
#define CALIBRATE_ACCEL_GYRO_MAG 4
#define CALIBRATE_STOP 5

const uint8_t CHANNEL_COMMAND = 0;
const uint8_t CHANNEL_EXECUTABLE = 1;
const uint8_t CHANNEL_CONTROL = 2;
const uint8_t CHANNEL_REPORTS = 3;
const uint8_t CHANNEL_WAKE_REPORTS = 4;
const uint8_t CHANNEL_GYRO = 5;

const int16_t linearAcc_q=8;
const int16_t acc_q=8;
const int16_t gravity_q=8;
const int16_t gyroCalibrated_q=9;
const int16_t magCalibrated_q=4;
const int16_t rotationVector_q=13;
const int16_t gameRotationVector_q=14;
const int16_t geomagneticRotationVector_q=12;
const int16_t ARVRStabRotationVector_data_q=14;
const int16_t ARVRStabRotationVector_accuracy_q=12;
const int16_t ARVRStabGameRotationVector_q=13;
const int16_t GIRotationVector_vector_q=14;
const int16_t GIRotationVector_velocity_q=10;

#endif /* SH2_H_ */
