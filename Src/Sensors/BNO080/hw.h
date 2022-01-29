

#ifndef HW_HAL_H_
#define HW_HAL_H_
#define bufferSize 255
#define headerSize 4
#include <stdint.h>


 //developer should  provide these functions to use this library with specific hardware
//hardware initialization should be done separately

void delay(uint32_t time);
void sendMsg(uint8_t* msg,uint16_t length);
void bootn(uint8_t state);
void rstn(uint8_t state);
void ps0_waken(uint8_t state);
void ps1(uint8_t state);
void csn(uint8_t state);
void enableInts(void);
void disableInts();
void SPI_TransmitReceive(uint8_t* txBuffer,uint8_t* rxBuffer,uint16_t length);

#endif /* HW_HAL_H_ */
