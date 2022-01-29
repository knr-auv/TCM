

#include "main.h"
#include "bno080.h"
#include <stdio.h>
#include  "hw.h"


extern UART_HandleTypeDef huart2;
uint8_t msg[100];
uint8_t len;

void MPUinit(void){

while(BNO080Init()==0);

enableGIRV(2000000);

}

void MPUapp(void){

if(GIRVAvailable()==1){

	struct GyroIntegratedRVFloat_t gameRV;
	gameRV=getGIRV();

len = sprintf((char*)msg,"quatI = %f\n\rquatJ = %f\n\rquatK = %f\n\rquatReal = %f \n\r", gameRV.quati,gameRV.quatj,gameRV.quatk,gameRV.real);
HAL_UART_Transmit(&huart2, msg, len, HAL_MAX_DELAY);
		}



}
