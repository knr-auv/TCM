

#ifndef BNO080_H_
#define BNO080_H_


struct ImuData_t{
	uint8_t sensorAccuracy;
	uint16_t x;
	uint16_t y;
	uint16_t z;
};
struct ImuDataFloat_t{
	uint8_t sensorAccuracy;
	float x;
	float y;
	float z;
};
struct GyroIntegratedRVFloat_t{
	float quati;
	float quatj;
	float quatk;
	float real;
	float angVelocityX;
	float angVelocityY;
	float angVelocityZ;
};

struct ImuDataVectorsFloat_t{
	uint8_t sensorAccuracy;
	float quati;
	float quatj;
	float quatk;
	float real;
	float vectorAccuracy;
};

uint8_t BNO080Init();			//initialization function necessary to perform before any operation
void BNO080_interruptHandler();		//should be called only by interrupts

//these functions are intended to check flags and reset them
uint8_t dataAvailable();
uint8_t controlAvailable();
uint8_t commandAvailable();
uint8_t GIRVAvailable();
uint8_t resetOccured();

//receiving data
struct ImuDataFloat_t getAcc();
struct ImuDataFloat_t getLinearAcc();
struct ImuData_t getRawAcc();
struct ImuDataFloat_t getGravity();
struct ImuData_t getRawGyro();
struct ImuDataFloat_t getGyro();
struct ImuData_t getRawMag();
struct ImuDataFloat_t getMag();
struct ImuDataVectorsFloat_t getGameRV();
struct ImuDataVectorsFloat_t getRotationVector();
struct ImuDataVectorsFloat_t getGeoMagRV();
struct ImuDataVectorsFloat_t getARVRStabRV();
struct ImuDataVectorsFloat_t getARVRStabGameRV();
struct GyroIntegratedRVFloat_t getGIRV();

//System bust be restarted for changes to take effect--> see sh2 data sheet 3.4  page 14
uint8_t setUARTformat(uint8_t format);
uint8_t setTimeSource(uint8_t source);
uint8_t setGIRV(uint16_t refDataType,uint32_t synchroInterval, float maxError, float PredictionAmount, float a,float b,float g );
uint8_t setARVRGameRVStabilization(float scaling, float maxRotation, float maxError,float stabilityMagnitude);//typical: 0.2, 0.127rad, 0.785rad, 0rad
uint8_t setARVRRVStabilization(float scaling, float maxRotation, float maxError,float stabilityMagnitude);
uint8_t setSensorOrientation(float quati, float quatj,float quatk,float real);


//enabling sensors
void enableGameRotationVector(uint32_t timeBetweenReportsus);
void enableGIRV(uint32_t timeBetweenReportsus);
void enableAcc(uint32_t timeBetweenReportsus);
void enableGyro(uint32_t timeBetweenReportsus);
void enableMag(uint32_t timeBetweenReportsus);
void enableLinearAcc(uint32_t timeBetweenReportsus);
void enableRotationVector(uint32_t timeBetweenReportsus);
void enableGravity(uint32_t timeBetweenReportsus);
void enableGeoMagRV(uint32_t timeBetweenReportsus);
void eanbleARVRStabRV(uint32_t timeBetweenReportsus);
void enableARVRStabGameRV(uint32_t timeBetweenReportsus);
void enableRawAcc(uint32_t timeBetweenReportsus);
void enableRawGyro(uint32_t timeBetweenReportsus);
void enableRawMag(uint32_t timeBetweenReportsus);

//unless you call getFeatureMsg getFeatureData will return last enabled feature data
void getFeatureMsg(uint8_t FeatureID);
struct sensorSetup_t getFeatureData();
//getIMUData needs to be preceded by getIdMsg
void getIdMsg();
struct ImuSetup_t getIMUData();

void sensorFlush(uint8_t sensorID);
void initialize();
void eraseFRS(uint16_t frstype);
void softReset();
void getOscillatorType();
void getFRS(uint16_t FRStype,uint16_t blocksize);


#endif /* BNO080_H_ */
