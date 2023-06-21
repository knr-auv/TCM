#include <stdio.h>
#include <string.h>
#include <math.h>

#include "hw.h"
#include "sh2.h"
#include "bno080.h"



struct flags_t{
	unsigned inReset:1;
	unsigned newGIRV:1;
	unsigned resetOccured:1;
	unsigned txReady:1;

	unsigned newData:1;
	unsigned newCommand:1;
	unsigned newControl:1;
};
struct FRSStatus_t{
	uint8_t status;
	uint16_t wordOffset;
};
struct FRSReceivedData_t{
	uint8_t length;
	uint8_t status;
	uint16_t wordOffset;
	uint32_t word0;
	uint32_t word1;
	uint32_t type;
};
struct ImuSetup_t{
	unsigned resetCause;
	unsigned swMajor:4;
	unsigned swMinor:4;
	uint16_t patch;
	uint32_t partNumber;
	uint32_t SwBuildNumber;
	unsigned protoSwMajor:4;
	unsigned protoSwMinor:4;
	uint16_t protoPatch;
	uint32_t protoPartNumber;
	uint32_t protoSwBuildNumber;
};

struct sensorSetup_t{
	uint8_t ID;
	uint8_t	flags;
	uint16_t sensitivity;
	uint32_t interval;
	uint32_t batch;
	uint32_t specific;
};


struct ImuDataVectors_t{
	uint8_t sensorAccuracy;
	uint16_t quati;
	uint16_t quatj;
	uint16_t quatk;
	uint16_t real;
	uint16_t vectorAccuracy;

};

struct GyroIntegratedRV_t{
	uint16_t quati;
		uint16_t quatj;
		uint16_t quatk;
		uint16_t real;
		uint16_t angVelocityX;
		uint16_t angVelocityY;
		uint16_t angVelocityZ;
};




#define true 1
#define false 0
#define debug 1






static uint8_t txData[bufferSize];
static uint8_t txBufLen=0;
static uint32_t timestamp;



static volatile struct flags_t flag={0,0,0,0,0,0,0};
static volatile struct ImuSetup_t ImuSetup;
static volatile struct sensorSetup_t sensorSetup;
static volatile struct FRSStatus_t FRSStatus;
static volatile struct FRSReceivedData_t FRSReadResponse;
static volatile struct ImuData_t Accelometer;
static volatile struct ImuData_t LinearAcceleration;
static volatile struct ImuData_t RawAccelerometer;
static volatile struct ImuData_t Gravity;
static volatile struct ImuData_t GyroRaw;
static volatile struct ImuData_t GyroCalibrated;
static volatile struct ImuData_t MagnetometerRaw;
static volatile struct ImuData_t MagnetometerCalibrated;
static volatile struct ImuDataVectors_t RotationVector;
static volatile struct ImuDataVectors_t GameRotationVector;
static volatile struct ImuDataVectors_t GeomagneticRotationVector;
static volatile struct ImuDataVectors_t ARVRStabRotationVector;
static volatile struct ImuDataVectors_t ARVRStabGameRotationVector;
static volatile struct GyroIntegratedRV_t GyroIntegratedRotationVector;



static void handleCommandReport(uint8_t* rxHeader,uint8_t* rxData){


	if(rxData[0]==1)
	{
		if(debug==1)
		{
			int16_t dataLength = ((uint16_t)rxHeader[1] << 8 | rxHeader[0]);
								dataLength &= ~(1 << 15);
			uint8_t dbg_msg[50];
			uint16_t dbg_size;
			dbg_size=sprintf((char*)dbg_msg,"error report received\n\r");
			sendMsg(dbg_msg, dbg_size);

			for(int i=0;i<dataLength;i++){
				dbg_size=sprintf((char*)dbg_msg,"%u\n\r",rxData[i]);
							sendMsg(dbg_msg, dbg_size);
			}
		}
	}
	else if(rxData[0]==0)
	{

	}
	else
	{

	}

}

static void handleControlReport(uint8_t* rxHeader,uint8_t* rxData){



	if(rxData[0]==SHTP_REPORT_COMMAND_RESPONSE)
	{
		if(rxData[2]==COMMAND_DCD){
			if(rxData[5]==0){
				if(debug==1)
							{
							uint8_t dbg_msg[50];
							uint16_t dbg_size;
							dbg_size=sprintf((char*)dbg_msg,"DCD saved successfully\n\r");
							sendMsg(dbg_msg, dbg_size);
							}
			}

		}
		else if(rxData[2]==COMMAND_ME_CALIBRATE){
			uint8_t status = rxData[5];
			uint8_t Acc_s=rxData[6];
			uint8_t Gyro_s=rxData[7];
			uint8_t Mag_s=rxData[8];
			uint8_t PlanarAcc_s=rxData[9];
			if(debug==1)
				{
				uint8_t dbg_msg[80];
				uint16_t dbg_size;
				dbg_size=sprintf((char*)dbg_msg,"Calibration routine set: %u, acc: %u , gyro: %u , mag %u , planarAcc: %u\n\r",status,Acc_s,Gyro_s,Mag_s,PlanarAcc_s);
				sendMsg(dbg_msg, dbg_size);
				}
		}
		else if(rxData[2]==COMMAND_OSCILLATOR){
			if(debug==1)
			{
				uint8_t dbg_msg[80];
				uint16_t dbg_size;
				dbg_size=sprintf((char*)dbg_msg,"oscillator type 0-internal, 1-external crystal,2-external clock, type: %u",rxData[5]);
				sendMsg(dbg_msg, dbg_size);
			}
		}
	}
	else if(rxData[0]==SHTP_REPORT_PRODUCT_ID_RESPONSE)
	{static uint8_t n;
	if(n==0){
		ImuSetup.resetCause=rxData[1];
		ImuSetup.swMajor=rxData[2];
		ImuSetup.swMinor=rxData[3];
		ImuSetup.partNumber=(rxData[7] << 24)+(rxData[6] << 16)+(rxData[5] << 8)+ rxData[4];
		ImuSetup.SwBuildNumber=(rxData[11] << 24)+(rxData[10] << 16)+(rxData[9] << 8)+ rxData[8];
		ImuSetup.patch=(rxData[13] << 8)+rxData[12];
		n++;
	}else{
		n=0;
		ImuSetup.protoSwMajor=rxData[2];
		ImuSetup.protoSwMinor=rxData[3];
		ImuSetup.protoPartNumber=(rxData[7] << 24)+(rxData[6] << 16)+(rxData[5] << 8)+ rxData[4];
		ImuSetup.protoSwBuildNumber=(rxData[11] << 24)+(rxData[10] << 16)+(rxData[9] << 8)+ rxData[8];
		ImuSetup.protoPatch=(rxData[13] << 8)+rxData[12];
	}

	}
	else if(rxData[0]==SHTP_REPORT_FRS_WRITE_RESPONSE){
		FRSStatus.status=rxData[1];
		FRSStatus.wordOffset=(rxData[3] << 8) + rxData[2];
		if(debug==1)
				{
				uint8_t dbg_msg[60];
				uint16_t dbg_size;
				dbg_size=sprintf((char*)dbg_msg,"Received frs write response, status: %u, offset: %u\n\r",FRSStatus.status,FRSStatus.wordOffset);
				sendMsg(dbg_msg, dbg_size);

	}
	}
	else if(rxData[0]==SHTP_REPORT_FRS_READ_RESPONSE)
	{
		FRSReadResponse.length=rxData[1]>>4;
		FRSReadResponse.status=rxData[1]&0xF;
		FRSReadResponse.wordOffset=(rxData[3]<<8)+rxData[2];
		FRSReadResponse.word0=(rxData[7] << 24) + (rxData[6] << 16) + (rxData[5]<<8)+rxData[4];
		FRSReadResponse.word1=(rxData[11] << 24) + (rxData[10] << 16) + (rxData[9]<<8)+rxData[8];
		FRSReadResponse.type=(rxData[13]<<8)+rxData[12];
		if(debug==1)
			{
			uint8_t dbg_msg[60];
			uint16_t dbg_size;
			uint16_t frs=rxData[12]+(rxData[13] << 8) ;
			dbg_size=sprintf((char*)dbg_msg,"Received frs record data: %x, word0: %lu , word1: %lu\n\r",frs,FRSReadResponse.word0,FRSReadResponse.word1);
			sendMsg(dbg_msg, dbg_size);
			}
	}
	else if(rxData[0]==SHTP_REPORT_GET_FEATURE_RESPONSE)
	{
		sensorSetup.ID=rxData[1];
		sensorSetup.interval=rxData[5]+(rxData[6] << 8)+(rxData[7] << 16)+ (rxData[8] << 24) ;
		sensorSetup.flags=rxData[2];
		sensorSetup.batch=(rxData[12] << 24) + (rxData[11] << 16) + (rxData[10] << 8) + rxData[9];
		sensorSetup.sensitivity=(rxData[4] << 8)+ rxData[3];
		sensorSetup.specific=(rxData[16] << 24) + (rxData[15] << 16) + (rxData[14] << 8) + rxData[13];
		if(debug==1)
					{
						uint8_t dbg_msg[50];
						uint16_t dbg_size;
						dbg_size=sprintf((char*)dbg_msg,"Sensor ID: %X  report interval(us): %lu \n\r",sensorSetup.ID,sensorSetup.interval);
						sendMsg(dbg_msg, dbg_size);

					}
	}
	else
	{
		if(debug==1)
		{
			uint8_t dbg_msg[50];
			uint16_t dbg_size;
			dbg_size=sprintf((char*)dbg_msg,"received unknown command request: 0x%X \n\r",rxData[0]);
			sendMsg(dbg_msg, dbg_size);
		}
	}

}
static void handleGIRV(uint8_t*rxHeader,uint8_t* rxData){
	GyroIntegratedRotationVector.quati=(rxData[6]<<8)+rxData[5];
	GyroIntegratedRotationVector.quatj=(rxData[8]<<8)+rxData[7];
	GyroIntegratedRotationVector.quatk=(rxData[10]<<8)+rxData[9];
	GyroIntegratedRotationVector.real=(rxData[12]<<8)+rxData[11];
	GyroIntegratedRotationVector.angVelocityX=(rxData[14]<<8)+rxData[13];
	GyroIntegratedRotationVector.angVelocityY=(rxData[16]<<8)+rxData[15];
	GyroIntegratedRotationVector.angVelocityZ=(rxData[18]<<8)+rxData[17];
}

static void handleDataReport(uint8_t* rxHeader,uint8_t* rxData){
		int16_t dataLength = ((uint16_t)rxHeader[1] << 8 | rxHeader[0]);
		dataLength &= ~(1 << 15);
		dataLength -= 4; //Remove the header bytes from the data count
		//timestamp is inserted at the start of each batch. byte 0 is always 0xFB.
		timestamp = ((uint32_t)rxData[4] << (8 * 3)) | ((uint32_t)rxData[3] << (8 * 2)) | ((uint32_t)rxData[2] << (8 * 1)) | ((uint32_t)rxData[1] << (8 * 0));

		uint8_t status = rxData[5 + 2] & 0x03; //Get status bits
		/*	0 – Unreliable
			1 – Accuracy low
			2 – Accuracy medium
			3 – Accuracy high
		*/
		uint16_t data1 = (uint16_t)rxData[5 + 5] << 8 | rxData[5 + 4];
		uint16_t data2 = (uint16_t)rxData[5 + 7] << 8 | rxData[5 + 6];
		uint16_t data3 = (uint16_t)rxData[5 + 9] << 8 | rxData[5 + 8];
		uint16_t data4 = 0;
		uint16_t data5 = 0;
		if (dataLength - 5 > 9)
			{
				data4 = (uint16_t)rxData[5 + 11] << 8 | rxData[5 + 10];
			}
		if (dataLength - 5 > 11)
			{
				data5 = (uint16_t)rxData[5 + 13] << 8 | rxData[5 + 12];
			}
if (rxData[5] == SENSOR_REPORTID_RAW_ACCELEROMETER)
{
	RawAccelerometer.sensorAccuracy=status;
	RawAccelerometer.x= data1;
	RawAccelerometer.y = data2;
	RawAccelerometer.z = data3;
}
else if(rxData[5]==SENSOR_REPORTID_ACCELEROMETER)
{
	Accelometer.sensorAccuracy=status;
	Accelometer.x = data1;
	Accelometer.y = data2;
	Accelometer.z = data3;
}
else if(rxData[5]==SENSOR_REPORTID_LINEAR_ACCELERATION){
	LinearAcceleration.sensorAccuracy=status;
	LinearAcceleration.x=data1;
	LinearAcceleration.y=data2;
	LinearAcceleration.z=data3;
}
else if(rxData[5]==SENSOR_REPORTID_GRAVITY){
	Gravity.sensorAccuracy=status;
	Gravity.x=data1;
	Gravity.y=data2;
	Gravity.z=data3;
}
else if(rxData[5]==SENSOR_REPORTID_RAW_GYROSCOPE){
	GyroRaw.sensorAccuracy=status;
	GyroRaw.x=data1;
	GyroRaw.y=data2;
	GyroRaw.z=data3;
}
else if(rxData[5]==SENSOR_REPORTID_GYROSCOPE){
	GyroCalibrated.sensorAccuracy=status;
	GyroCalibrated.x=data1;
	GyroCalibrated.y=data2;
	GyroCalibrated.z=data3;
}
else if(rxData[5]==SENSOR_REPORTID_RAW_MAGNETOMETER){
	MagnetometerRaw.sensorAccuracy=status;
	MagnetometerRaw.x=data1;
	MagnetometerRaw.y=data2;
	MagnetometerRaw.z=data3;
}
else if(rxData[5]==SENSOR_REPORTID_MAGNETIC_FIELD){
	MagnetometerCalibrated.sensorAccuracy=status;
	MagnetometerCalibrated.x=data1;
	MagnetometerCalibrated.y=data2;
	MagnetometerCalibrated.z=data3;
}
else if(rxData[5] == SENSOR_REPORTID_ROTATION_VECTOR)
{
	RotationVector.quati=data1;
	RotationVector.quatj=data2;
	RotationVector.quatk=data3;
	RotationVector.real=data4;
	RotationVector.sensorAccuracy = status;
	RotationVector.vectorAccuracy=data5;

}
else if(rxData[5]==SENSOR_REPORTID_GAME_ROTATION_VECTOR){
	GameRotationVector.sensorAccuracy=status;
	GameRotationVector.quati=data1;
	GameRotationVector.quatj=data2;
	GameRotationVector.quatk=data3;
	GameRotationVector.real=data4;
}
else if(rxData[5]==SENSOR_REPORTID_GEOMAGNETIC_ROTATION_VECTOR){
	GeomagneticRotationVector.sensorAccuracy=status;
	GeomagneticRotationVector.quati=data1;
	GeomagneticRotationVector.quatj=data2;
	GeomagneticRotationVector.quatk=data3;
	GeomagneticRotationVector.real=data4;
}
else if(rxData[5]==SENSOR_REPORTID_ARVR_STABILIZED_ROTATION_VECTOR){
	ARVRStabRotationVector.sensorAccuracy=status;
	ARVRStabRotationVector.quati=data1;
	ARVRStabRotationVector.quatj=data2;
	ARVRStabRotationVector.quatk=data3;
	ARVRStabRotationVector.real=data4;
	ARVRStabRotationVector.vectorAccuracy=data5;
}
else if(rxData[5]==SENSOR_REPORTID_ARVR_STABILIZED_GAME_ROTATION_VECTOR){
	ARVRStabGameRotationVector.sensorAccuracy=status;
	ARVRStabGameRotationVector.quati=data1;
	ARVRStabGameRotationVector.quatj=data2;
	ARVRStabGameRotationVector.quatk=data3;
	ARVRStabGameRotationVector.real=data4;

}


}

static void handleRxData(uint8_t *rxHeader,uint8_t *rxData){

	int16_t dataLength = ((uint16_t)rxHeader[1] << 8 | rxHeader[0]);
	dataLength &= ~(1 << 15);
	if(debug==1)
				{
			uint8_t dbg_msg[50];
			uint16_t dbg_size;

			dbg_size=sprintf((char*)dbg_msg,"Msg on channel: %u , length: %u\n\r",rxHeader[2],dataLength);
			sendMsg(dbg_msg, dbg_size);
				}
		if(rxHeader[2]==CHANNEL_REPORTS||rxHeader[2]==CHANNEL_WAKE_REPORTS){
			flag.newData=true;
			handleDataReport(rxHeader,rxData);
			return;
		}
		else if(rxHeader[2]==CHANNEL_GYRO){
			flag.newGIRV=true;
			handleGIRV(rxHeader, rxData);
		}
		else if(rxHeader[2]==CHANNEL_CONTROL){
			flag.newControl=true;
			handleControlReport(rxHeader,rxData);
			return;
		}else if(rxHeader[2]==CHANNEL_COMMAND){
			flag.newCommand=true;
			handleCommandReport(rxHeader,rxData);
			return;
		}
		else if(rxHeader[2]==CHANNEL_EXECUTABLE){
			flag.resetOccured=true;
			flag.inReset=false;
		}else{
			if(debug==1){
							uint8_t dbg_msg[50];
							uint16_t dbg_size;
							dbg_size=sprintf((char*)dbg_msg,"Unsuported channel: %u \n\r",rxHeader[2]);
							sendMsg(dbg_msg, dbg_size);
					}
			return;
		}
return;
}
static uint8_t sendData(uint8_t channelNumber, uint8_t datalength){
	uint16_t packetlength=datalength+4;
	if(flag.txReady==false)
		return false;
	flag.txReady=false;
	memmove(txData+4,txData,datalength);
	static uint8_t sequenceNumber[6];
	txData[0]=(packetlength&0xFF);
	txData[1]=(packetlength>>8);
	txData[2]=channelNumber;
	txData[3]=sequenceNumber[channelNumber]++;
	txBufLen=datalength+4;
	disableInts();
	ps0_waken(false);
	enableInts();

	return (true);
	
	}

void BNO080_interruptHandler(){

disableInts();
uint8_t zero[bufferSize];
memset(zero,0,bufferSize);
uint8_t rxHeader[headerSize];
uint8_t rxData[bufferSize];


if(txBufLen!=0){
	ps0_waken(true);
	csn(false);

	SPI_TransmitReceive(txData, rxHeader, headerSize);
	int16_t dataLen = ((uint16_t)rxHeader[1] << 8 | rxHeader[0]);
	dataLen &=~(1 << 15);
	SPI_TransmitReceive(txData+4, rxData, (txBufLen>dataLen)?(txBufLen-4):(dataLen-4));
	csn(true);
	if(dataLen!=0)
		handleRxData(rxHeader, rxData);

	txBufLen=0;
	flag.txReady=true;
	}
else
{

	csn(false);
	SPI_TransmitReceive(zero, rxHeader, headerSize);
	uint16_t dataLen = ((uint16_t)rxHeader[1] << 8 | rxHeader[0]);
	dataLen &=~(1 << 15);
	if(dataLen>bufferSize)
		return;
	if(dataLen==0){
		csn(true);
		return ;
	}
	dataLen-=4;
	SPI_TransmitReceive(zero, rxData, dataLen);
	csn(true);
	handleRxData(rxHeader, rxData);
}
enableInts();
	}




uint8_t BNO080Init(){

	flag.inReset=true;
	if(debug==1){
		uint8_t dbg_msg[50];
		uint16_t dbg_size;
		dbg_size=sprintf((char*)dbg_msg,"Init started \n\r");
		sendMsg(dbg_msg, dbg_size);
		}
	int i=0;

	rstn(false);
	csn(true);
	ps0_waken(true);
	ps1(true);
	bootn(true);
	delay(100);
	enableInts();
	rstn(true);
	i=0;
	while(flag.inReset==true){
		i++;
		delay(1);
		if(i>3000)
			return false;
	}
	if(debug==1){
			uint8_t dbg_msg[50];
			uint16_t dbg_size;
			dbg_size=sprintf((char*)dbg_msg,"Init complete \n\r");
			sendMsg(dbg_msg, dbg_size);
			}
	flag.resetOccured=false;
	flag.txReady=true;
	flag.newCommand=false;
	flag.newControl=false;
	flag.newData=false;


	return true;
}










static void setFeatureCommand(uint8_t reportID, uint32_t timeBetweenReportsus, uint32_t specificConfig)
{
	/* sensivity -->16-bit signed fixed point integer representing the value a
	sensor output must exceed in order to trigger another
	input report.*/

	txData[0] = SHTP_REPORT_SET_FEATURE_COMMAND;	 	//Set feature command
	txData[1] = reportID;
	txData[2] = 0;								   		//Feature flags
	txData[3] = 0;								   		//Change sensitivity (LSB)
	txData[4] = 0;								   		//Change sensitivity (MSB)
	txData[5] = (timeBetweenReportsus >> 0) & 0xFF;  	//Report interval (LSB) in microseconds
	txData[6] = (timeBetweenReportsus >> 8) & 0xFF;
	txData[7] = (timeBetweenReportsus >> 16) & 0xFF;
	txData[8] = (timeBetweenReportsus >> 24) & 0xFF; 	//Report interval (MSB)
	txData[9] = 0;								  		//Batch Interval (LSB)
	txData[10] = 0;
	txData[11] = 0;
	txData[12] = 0;								  		//Batch Interval (MSB)
	txData[13] = (specificConfig); 						//Sensor-specific config (LSB)
	txData[14] = (specificConfig >> 8);
	txData[15] = (specificConfig >> 16);
	txData[16] = (specificConfig >> 24);	  			//Sensor-specific config (MSB)
	sendData(CHANNEL_CONTROL, 17);



}

static uint32_t floatToQ(float floatValue, uint8_t qValue){
	
	uint32_t retval=(uint32_t)(floatValue*(2<<qValue));
	return retval;
}
static void FRSWriteRequest(uint16_t frstype, uint16_t length){
	txData[0]=SHTP_REPORT_FRS_WRITE_REQUEST;
	txData[1]=0;
	txData[2]=length;
	txData[3]=length>>8;
	txData[4]=frstype;
	txData[5]=frstype>>8;
	FRSStatus.status=100;
	FRSStatus.wordOffset=100;
	sendData(CHANNEL_CONTROL, 6);
}
static void FRSWriteData(uint16_t offset, uint32_t data0,uint32_t data1){
	txData[0]=SHTP_REPORT_FRS_WRITE_DATA_REQUEST;
	txData[1]=0;
	txData[2]=offset;
	txData[3]=offset>>8;
	txData[4]=data0;
	txData[5]=data0>>8;
	txData[6]=data0>>16;
	txData[7]=data0>>24;
	txData[8]=data1;
	txData[9]=data1>>8;
	txData[10]=data1>>16;
	txData[11]=data1>>24;
	FRSStatus.status=100;
	FRSStatus.wordOffset=100;
	sendData(CHANNEL_CONTROL, 12);

}
uint8_t setUARTformat(uint8_t format){
	FRSWriteRequest(0xa1a1, 1);
	delay(100);
	if(FRSStatus.status!=4)
		return false;
	FRSWriteData(0, format, 0);
	delay(100);
		if(FRSStatus.status!=3)

			return false;
		return true;
}
uint8_t setGIRV(uint16_t refDataType,uint32_t synchroInterval, float maxError, float PredictionAmount, float a,float b,float g ){
	uint32_t time=10;
FRSWriteRequest(FRS_CONFIG_GIRV, 7);
delay(time);
if(FRSStatus.status!=4)
	return false;
uint32_t word0;
word0=0;
word0=refDataType;
FRSWriteData(0, word0, synchroInterval);
delay(time);
if(FRSStatus.status!=0)
	return false;
FRSWriteData(2,floatToQ(maxError, 29),floatToQ(PredictionAmount, 10));
delay(time);
if(FRSStatus.status!=0)
	return false;
FRSWriteData(4, floatToQ(a, 20), floatToQ(b, 20));
delay(time);
if(FRSStatus.status!=0)
	return false;
FRSWriteData(6, floatToQ(g, 20), floatToQ(g, 20));
delay(time);
if(FRSStatus.status!=3)
	return false;
return true;

}
uint8_t setTimeSource(uint8_t source){
	FRSWriteRequest(FRS_CONFIG_ME_TIME_SOURCE_SELECTION, 1);
	delay(100);
	if(FRSStatus.status!=4)
		return false;

	FRSWriteData(0, source, 0);
	delay(100);
	if(FRSStatus.status!=3)

		return false;
	return true;
}
uint8_t setSensorOrientation(float quati, float quatj,float quatk,float real){

//status 0 is word received status 3 is write completed 4 is ready for receiving
FRSWriteRequest(FRS_CONFIG_SYSTEM_ORIENTATION, 4);
delay(100);
if(FRSStatus.status!=4)
	return false;
uint32_t i,j,k,r;
i=floatToQ(quati, 30);
j=floatToQ(quatj, 30);
k=floatToQ(quatk, 30);
r=floatToQ(real, 30);
FRSWriteData(0, i, j);
delay(100);
if(FRSStatus.status!=0)
	return false;
FRSWriteData(2, k, r);
delay(100);
if(FRSStatus.status!=3)
	return false;
return true;
}
uint8_t setARVRRVStabilization(float scaling, float maxRotation, float maxError,float stabilityMagnitude){
	uint32_t time=1;
	FRSWriteRequest(FRS_CONFIG_ARVR_STABILIZATION_RV, 4);
	delay(time);
	if(FRSStatus.status!=4)
		return false;
	uint32_t sc,mR,mE,sM;
	sc=floatToQ(scaling, 30);
	mR=floatToQ(maxRotation, 29);
	mE=floatToQ(maxError, 29);
	sM=floatToQ(stabilityMagnitude, 29);
	FRSWriteData(0, sc, mR);
	delay(time);
	if(FRSStatus.status!=0)
		return false;
	FRSWriteData(2, mE, sM);
	delay(time);
	if(FRSStatus.status!=3)
		return false;
	return true;
}
uint8_t setARVRGameRVStabilization(float scaling, float maxRotation, float maxError,float stabilityMagnitude){
	FRSWriteRequest(FRS_CONFIG_ARVR_STABILIZATION_GAME_RV, 4);
	delay(100);
	if(FRSStatus.status!=4)
		return false;
	uint32_t sc,mR,mE,sM;
	sc=floatToQ(scaling, 30);
	mR=floatToQ(maxRotation, 29);
	mE=floatToQ(maxError, 29);
	sM=floatToQ(stabilityMagnitude, 29);
	FRSWriteData(0, sc, mR);
	delay(100);
	if(FRSStatus.status!=0)
		return false;
	FRSWriteData(2, mE, sM);
	delay(100);
	if(FRSStatus.status!=3)
		return false;
	return true;
}
void sensorFlush(uint8_t sensorID){
	txData[0]=SHTP_FLUSH_SENSOR;
	txData[1]=sensorID;
	sendData(CHANNEL_CONTROL, 2);

}

void initialize(){
	txData[0]=SHTP_REPORT_COMMAND_REQUEST;
	static int i;
	txData[1]=i++;
	txData[2]=0x04;
	txData[3]=1;
	for(int j =4;j<12;j++)
		txData[j]=0;
	sendData(2, 12);
}
void softReset(){
	txData[0]=1;
	sendData(CHANNEL_EXECUTABLE, 1);
	flag.inReset=true;
	delay(200);
}

void eraseFRS(uint16_t frstype){
	FRSWriteRequest(frstype, 0);
}
void getFRS(uint16_t FRStype,uint16_t blocksize){
	txData[0]=SHTP_REPORT_FRS_READ_REQUEST;
	txData[1]=0;		//reserved
	txData[2]=0;		//read offset LSB
	txData[3]=0;		//read offset LSB
	txData[4]=(uint8_t)FRStype;
	txData[5]=(uint8_t)(FRStype >>8);
	txData[6]=blocksize;
	txData[7]=blocksize>>8;
sendData(CHANNEL_CONTROL, 8);
}

void getOscillatorType(){
	txData[0]=SHTP_REPORT_COMMAND_REQUEST;
	static uint8_t seqnum;
	txData[1]=seqnum++;
	txData[2]=COMMAND_OSCILLATOR;
	for(int i =3;i<12;i++)
		txData[i]=0;
	sendData(CHANNEL_CONTROL, 12);
}
void getIdMsg(){
		txData[0] = SHTP_REPORT_PRODUCT_ID_REQUEST;
		txData[1] = 0;
		sendData(CHANNEL_CONTROL,2);
}

void getFeatureMsg(uint8_t FeatureID){
	txData[0]=SHTP_REPORT_GET_FEATURE_REQUEST;
	txData[1]=FeatureID;
	sendData(CHANNEL_CONTROL, 2);

}
struct sensorSetup_t getFeatureData(){
	return sensorSetup;
}
struct ImuSetup_t getIMUData(){
	return ImuSetup;
}
uint8_t GIRVAvailable(){
	uint8_t retval=flag.newGIRV;
	flag.newGIRV=false;
	return retval;
}
uint8_t resetOccured(){
	uint8_t retval=flag.resetOccured;
	flag.resetOccured=false;
	return retval;
}
uint8_t commandAvailable(){
	uint8_t retval=flag.newCommand;
		flag.newData=false;
		return retval;
}
uint8_t controlAvailable(){
	uint8_t retval=flag.newControl;
		flag.newData=false;
		return retval;
}
uint8_t dataAvailable(){
	uint8_t retval=flag.newData;
	flag.newData=false;
	return retval;
}



void enableGameRotationVector(uint32_t timeBetweenReportsus)
{
	setFeatureCommand(SENSOR_REPORTID_GAME_ROTATION_VECTOR, timeBetweenReportsus,0);

}

void enableGIRV(uint32_t timeBetweenReportsus){
	setFeatureCommand(SENSOR_REPORTID_GYRO_INTEGRATED_GAME_ROTATION_VECTOR, timeBetweenReportsus, 0);
}
void enableAcc(uint32_t timeBetweenReportsus){
	setFeatureCommand(SENSOR_REPORTID_ACCELEROMETER, timeBetweenReportsus, 0);
}
void enableGyro(uint32_t timeBetweenReportsus){
	setFeatureCommand(SENSOR_REPORTID_GYROSCOPE, timeBetweenReportsus, 0);
}
void enableMag(uint32_t timeBetweenReportsus){
	setFeatureCommand(SENSOR_REPORTID_MAGNETIC_FIELD, timeBetweenReportsus, 0);
}
void enableLinearAcc(uint32_t timeBetweenReportsus){
	setFeatureCommand(SENSOR_REPORTID_LINEAR_ACCELERATION, timeBetweenReportsus, 0);
}
void enableRotationVector(uint32_t timeBetweenReportsus){
	setFeatureCommand(SENSOR_REPORTID_ROTATION_VECTOR, timeBetweenReportsus, 0);
}
void enableGravity(uint32_t timeBetweenReportsus){
	setFeatureCommand(SENSOR_REPORTID_GRAVITY, timeBetweenReportsus, 0);
}
void enableGeoMagRV(uint32_t timeBetweenReportsus){
	setFeatureCommand(SENSOR_REPORTID_GEOMAGNETIC_ROTATION_VECTOR, timeBetweenReportsus, 0);
}
void eanbleARVRStabRV(uint32_t timeBetweenReportsus){
	setFeatureCommand(SENSOR_REPORTID_ARVR_STABILIZED_ROTATION_VECTOR, timeBetweenReportsus, 0);
}
void enableARVRStabGameRV(uint32_t timeBetweenReportsus){
	setFeatureCommand(SENSOR_REPORTID_ARVR_STABILIZED_GAME_ROTATION_VECTOR, timeBetweenReportsus, 0);
}
void enableRawAcc(uint32_t timeBetweenReportsus){
	setFeatureCommand(SENSOR_REPORTID_RAW_ACCELEROMETER, timeBetweenReportsus, 0);
}
void enableRawGyro(uint32_t timeBetweenReportsus){
	setFeatureCommand(SENSOR_REPORTID_RAW_GYROSCOPE, timeBetweenReportsus, 0);
}
void enableRawMag(uint32_t timeBetweenReportsus){
	setFeatureCommand(SENSOR_REPORTID_RAW_MAGNETOMETER, timeBetweenReportsus, 0);
}
static float qToFloat(int16_t fixedPointValue, uint8_t qPoint)
{
	float qFloat = fixedPointValue;
	qFloat *= 2>>qPoint;//pow(2, qPoint * -1);
	return (qFloat);
}
struct ImuDataFloat_t getAcc(){
	struct ImuDataFloat_t ret;
	ret.x=qToFloat(Accelometer.x,acc_q);
	ret.y=qToFloat(Accelometer.y,acc_q);
	ret.z=qToFloat(Accelometer.z,acc_q);
	ret.sensorAccuracy=Accelometer.sensorAccuracy;
	return ret;
}
struct ImuDataFloat_t getLinearAcc(){
		struct ImuDataFloat_t ret;
		ret.x=qToFloat(LinearAcceleration.x,linearAcc_q);
		ret.y=qToFloat(LinearAcceleration.y,linearAcc_q);
		ret.z=qToFloat(LinearAcceleration.z,linearAcc_q);
		ret.sensorAccuracy=LinearAcceleration.sensorAccuracy;
		return ret;
}
struct ImuData_t getRawAcc(){
	return RawAccelerometer;
}
struct ImuDataFloat_t getGravity(){
	struct ImuDataFloat_t ret;
	ret.x=qToFloat(Gravity.x, gravity_q);
	ret.y=qToFloat(Gravity.y, gravity_q);
	ret.z=qToFloat(Gravity.z, gravity_q);
	ret.sensorAccuracy=Gravity.sensorAccuracy;
	return ret;
}
struct ImuData_t getRawGyro(){
	return GyroRaw;
}
struct ImuDataFloat_t getGyro(){
	struct ImuDataFloat_t ret;
	ret.x=qToFloat(GyroCalibrated.x, gyroCalibrated_q);
	ret.y=qToFloat(GyroCalibrated.y, gyroCalibrated_q);
	ret.z=qToFloat(GyroCalibrated.z, gyroCalibrated_q);
	ret.sensorAccuracy=GyroCalibrated.sensorAccuracy;
	return ret;
}

struct ImuData_t getRawMag(){
	return MagnetometerRaw;
}

struct ImuDataFloat_t getMag(){
	struct ImuDataFloat_t ret;
	ret.x=qToFloat(MagnetometerCalibrated.x, magCalibrated_q);
	ret.y=qToFloat(MagnetometerCalibrated.y, magCalibrated_q);
	ret.z=qToFloat(MagnetometerCalibrated.z, magCalibrated_q);
	ret.sensorAccuracy=MagnetometerCalibrated.sensorAccuracy;
	return ret;
}
struct ImuDataVectorsFloat_t getRotationVector(){
	struct ImuDataVectorsFloat_t ret;
	ret.quati=qToFloat(RotationVector.quati,rotationVector_q);
	ret.quatj=qToFloat(RotationVector.quatj,rotationVector_q);
	ret.quatk=qToFloat(RotationVector.quatk,rotationVector_q);
	ret.real=qToFloat(RotationVector.real,rotationVector_q);
	ret.vectorAccuracy=qToFloat(RotationVector.vectorAccuracy, rotationVector_q);
	ret.sensorAccuracy=RotationVector.sensorAccuracy;
	return ret;
}
struct ImuDataVectorsFloat_t getGameRV(){
	struct ImuDataVectorsFloat_t ret;
		ret.quati=qToFloat(GameRotationVector.quati,gameRotationVector_q);
		ret.quatj=qToFloat(GameRotationVector.quatj,gameRotationVector_q);
		ret.quatk=qToFloat(GameRotationVector.quatk,gameRotationVector_q);
		ret.real=qToFloat(GameRotationVector.real,gameRotationVector_q);
		ret.vectorAccuracy=qToFloat(GameRotationVector.vectorAccuracy, gameRotationVector_q);
		ret.sensorAccuracy=GameRotationVector.sensorAccuracy;
		return ret;
}

struct ImuDataVectorsFloat_t getGeoMagRV(){
	struct ImuDataVectorsFloat_t ret;
		ret.quati=qToFloat(GeomagneticRotationVector.quati,geomagneticRotationVector_q);
		ret.quatj=qToFloat(GeomagneticRotationVector.quatj,geomagneticRotationVector_q);
		ret.quatk=qToFloat(GeomagneticRotationVector.quatk,geomagneticRotationVector_q);
		ret.real=qToFloat(GeomagneticRotationVector.real,geomagneticRotationVector_q);
		ret.vectorAccuracy=qToFloat(GeomagneticRotationVector.vectorAccuracy, geomagneticRotationVector_q);
		ret.sensorAccuracy=GeomagneticRotationVector.sensorAccuracy;
		return ret;
}
struct ImuDataVectorsFloat_t getARVRStabRV(){
	struct ImuDataVectorsFloat_t ret;
	ret.quati=qToFloat(ARVRStabRotationVector.quati, ARVRStabRotationVector_data_q);
	ret.quatj=qToFloat(ARVRStabRotationVector.quatj, ARVRStabRotationVector_data_q);
	ret.quatk=qToFloat(ARVRStabRotationVector.quatk, ARVRStabRotationVector_data_q);
	ret.real=qToFloat(ARVRStabRotationVector.real, ARVRStabRotationVector_data_q);
	ret.sensorAccuracy=ARVRStabRotationVector.sensorAccuracy;
	ret.vectorAccuracy=qToFloat(ARVRStabRotationVector.vectorAccuracy, ARVRStabRotationVector_accuracy_q);
	return ret;
}
struct ImuDataVectorsFloat_t getARVRStabGameRV(){
	struct ImuDataVectorsFloat_t ret;
	ret.quati=qToFloat(ARVRStabGameRotationVector.quati, ARVRStabGameRotationVector_q);
	ret.quatj=qToFloat(ARVRStabGameRotationVector.quatj, ARVRStabGameRotationVector_q);
	ret.quatk=qToFloat(ARVRStabGameRotationVector.quatk, ARVRStabGameRotationVector_q);
	ret.real=qToFloat(ARVRStabGameRotationVector.real, ARVRStabGameRotationVector_q);
	return ret;
}

struct GyroIntegratedRVFloat_t getGIRV(){
	struct GyroIntegratedRVFloat_t ret;
	ret.quati=qToFloat(GyroIntegratedRotationVector.quati, GIRotationVector_vector_q);
	ret.quatj=qToFloat(GyroIntegratedRotationVector.quatj, GIRotationVector_vector_q);
	ret.quatk=qToFloat(GyroIntegratedRotationVector.quatk, GIRotationVector_vector_q);
	ret.real=qToFloat(GyroIntegratedRotationVector.real, GIRotationVector_vector_q);
	ret.angVelocityX=qToFloat(GyroIntegratedRotationVector.angVelocityX,GIRotationVector_velocity_q);
	ret.angVelocityY=qToFloat(GyroIntegratedRotationVector.angVelocityY,GIRotationVector_velocity_q);
	ret.angVelocityZ=qToFloat(GyroIntegratedRotationVector.angVelocityZ,GIRotationVector_velocity_q);
	return ret;
}
