#include <stdbool.h>

#include "SST25_platform.h"


#define SST25_READ                  0x03    /* Read data bytes           3   0  >=1 */
#define SST25_FAST_READ             0x0b    /* Higher speed read         3   1  >=1 */
#define SST25_ERASE4K               0x20    /* 4Kb Sector erase          3   0   0  */
#define SST25_ERASE32K              0x52    /* 32Kbit block Erase        3   0   0  */
#define SST25_ERASE64K              0xd8    /* 64Kbit block Erase        3   0   0  */
#define SST25_CHIPERASE             0xc7    /* Chip erase                0   0   0  */   
#define SST25_BP                    0x02    /* Byte program              3   0   1  */
#define SST25_AAI                   0xAF    /* Auto address increment    3   0  >=2 */
#define SST25_RDSR                  0x05    /* Read status register      0   0  >=1 */
#define SST25_RDSR1                 0x35    /*Read status register 1*/
#define SST25_EWSR                  0x50    /* Write enable status       0   0   0  */
#define SST25_WRSR                  0x01    /* Write Status Register     0   0   1  */
#define SST25_WREN                  0x06    /* Write Enable              0   0   0  */
#define SST25_WRDI                  0x04    /* Write Disable             0   0   0  */
#define SST25_RDID                  0xab    /* Read Identification       0   0  >=1 */
#define SST25_RDID_ALT              0x90    /* Read Identification (alt) 0   0  >=1 */
#define SST25_JEDEC_ID              0x9f    /* JEDEC ID read             0   0  >=3 */
#define SST25_EBSY                  0x70    /* Enable SO RY/BY# status   0   0   0  */
#define SST25_DBSY                  0x80    /* Disable SO RY/BY# status  0   0   0  */



void SPI_TransmitByte(uint8_t byte)
{
    SPI_Transmit(&byte,1);
}


uint8_t SPI_ReceiveByte()
{
    uint8_t byte;
    SPI_Receive(&byte,1);
    return byte;
}

void SPI_WriteByte(uint8_t byte)
{
    SetPinCS(false);
    SPI_TransmitByte(byte);
    SetPinCS(true);
}


bool SST25_IsBusy();

void SST25_WriteEnable()
{
    SPI_WriteByte(SST25_WREN);
}

void SST25_WriteDisable()
{
    SPI_WriteByte(SST25_WRDI);
}
uint8_t SST25_ReadCmdRegister_u8(uint8_t cmd)
{
    SetPinCS(false);
    SPI_TransmitByte(cmd);
    uint8_t ret = SPI_ReceiveByte();
    SetPinCS(true);
    return ret;
}
uint16_t SST25_ReadCmdRegister_u16(uint8_t cmd)
{
    uint16_t ret;
    SetPinCS(false);
    SPI_TransmitByte(cmd);
     ret = SPI_ReceiveByte()<<8;
     ret = SPI_ReceiveByte();
    SetPinCS(true);
    return ret;
}
void SST25_WriteCmdRegister(uint8_t dst, uint8_t data)
{
    SetPinCS(false);
    SPI_TransmitByte(dst);
    SPI_TransmitByte(data);
    SetPinCS(true);
   
}

void SST25_ChipErase()
{
    SST25_WriteEnable();
    SPI_WriteByte(SST25_CHIPERASE);
     SST25_WriteDisable();
}
void SST25_Erase4K(uint32_t dst)
{
    SST25_WriteEnable();
    SetPinCS(false);
    SPI_TransmitByte(SST25_ERASE4K);
    SPI_TransmitByte(((dst&0xFFFFFF)>>16));
    SPI_TransmitByte(((dst&0xFFFF)>>8));
    SPI_TransmitByte((dst&0xFF));
    SetPinCS(true); 
    SST25_WriteDisable();
      while(SST25_IsBusy());
}
void SST25_Erase32K(uint32_t dst)
{
    SST25_WriteEnable();
        SetPinCS(false);
    SPI_TransmitByte(SST25_ERASE32K);
    SPI_TransmitByte(((dst&0xFFFFFF)>>16));
    SPI_TransmitByte(((dst&0xFFFF)>>8));
    SPI_TransmitByte((dst&0xFF));
    SetPinCS(true); 
    SST25_WriteDisable();
      while(SST25_IsBusy());
}
void SST25_Erase64K(uint32_t dst)
{
    SST25_WriteEnable();
        SetPinCS(false);
    SPI_TransmitByte(SST25_ERASE64K);
    SPI_TransmitByte(((dst&0xFFFFFF)>>16));
    SPI_TransmitByte(((dst&0xFFFF)>>8));
    SPI_TransmitByte((dst&0xFF));
    SetPinCS(true);
    SST25_WriteDisable();
      while(SST25_IsBusy());
}
uint8_t SST25_ReadStatusReg()
{
    return SST25_ReadCmdRegister_u8(SST25_RDSR);
}
uint8_t SST25_ReadStatusReg1()
{
    return SST25_ReadCmdRegister_u8(SST25_RDSR1);
}

uint16_t SST25_ReadJEDEC_ID()
{
    return SST25_ReadCmdRegister_u16(SST25_RDSR);
}

void SST25_EnableWriteSR()
{
    SPI_WriteByte(SST25_EWSR);
}

void SST25_WriteSR(uint8_t SR1, uint8_t SR2)
{
    SetPinCS(false);
    SPI_TransmitByte(SST25_WRSR);
    SPI_TransmitByte(SR1);
    SPI_TransmitByte(SR2);
    SetPinCS(true);
}

uint8_t SST25_ReadID()
{
    uint8_t byte;
    SetPinCS(false);
    SPI_TransmitByte(SST25_RDID_ALT);
    SPI_TransmitByte(0x00);
    SPI_TransmitByte(0x00);
    SPI_TransmitByte(0x01);
    byte = SPI_ReceiveByte();
    SetPinCS(true); 
    return byte;
}

bool SST25_IsBusy()
{
    uint8_t sr = SST25_ReadStatusReg();
    return (sr&0x01);

}

void SST25_Read(uint32_t address,uint8_t* buffer, uint16_t len)
{
    SetPinCS(false);
    SPI_TransmitByte(SST25_READ); 		
	SPI_TransmitByte(((address & 0xFFFFFF) >> 16));	
	SPI_TransmitByte(((address & 0xFFFF) >> 8));
	SPI_TransmitByte(address & 0xFF);
	SPI_Receive(buffer, len);
    SetPinCS(true);

}
void SST25_FastRead(uint32_t address,uint8_t* buffer, uint16_t len)
{
    SetPinCS(false);
    SPI_TransmitByte(SST25_FAST_READ); 		
	SPI_TransmitByte(((address & 0xFFFFFF) >> 16));	
	SPI_TransmitByte(((address & 0xFFFF) >> 8));
	SPI_TransmitByte(address & 0xFF);
    SPI_TransmitByte(0xff);
	SPI_Receive(buffer, len);
    SetPinCS(true);

}

void SST25_WriteByte(uint32_t address, uint8_t data)
{
    SST25_WriteEnable();
    SetPinCS(false);	
	SPI_TransmitByte(SST25_BP); 		
	SPI_TransmitByte(((address & 0xFFFFFF) >> 16));	
	SPI_TransmitByte(((address & 0xFFFF) >> 8));
	SPI_TransmitByte(address & 0xFF);
	SPI_TransmitByte(data);
	SetPinCS(true);
    SST25_WriteDisable();

}

void __SST25_AAI_Write(uint32_t address, uint8_t* buffer, int len)
{
    SST25_WriteEnable();
    SetPinCS(false);
    SPI_TransmitByte(SST25_AAI);
    SPI_TransmitByte(((address & 0xFF0000) >> 16));	
	SPI_TransmitByte(((address & 0xFF00) >> 8));
	SPI_TransmitByte(address & 0xFF);
    SPI_TransmitByte(*buffer++);
    //SPI_TransmitByte(*buffer++);
    len-=1;
    SetPinCS(true);
    while(SST25_IsBusy());

    while(len)
    {
        SetPinCS(false);
        SPI_TransmitByte(SST25_AAI);
        SPI_TransmitByte(*buffer++);
      
        len-=1;
        SetPinCS(true);
        while(SST25_IsBusy());
    }
    SetPinCS(true);
    SST25_WriteDisable();

}

void SST25_WriteBytes(uint32_t address, uint8_t* buffer, uint16_t count)
{
    int len = count;
   if(len==1)
        {
            SST25_WriteByte(address, buffer[0]);
            return;
        }

    if((address&0x1)==0x1)
    {
        SST25_WriteByte(address, *buffer++);
        address++;
        len--;
    }

    uint32_t evenBytes = len&~0x1;
    if(evenBytes)
    {
        __SST25_AAI_Write(address, buffer,evenBytes);
        len-=evenBytes;
    }
    if(len)
    {
        buffer+=evenBytes;
        address+=evenBytes;
        SST25_WriteByte(address,*buffer++);
    }
}

void SST25_Init()
{
    SetPinWP(true);
    SetPinCS(true);
    SetPinHOLD(true);
    SST25_EnableWriteSR();
    SST25_WriteSR(0,0);
    SST25_WriteDisable();
    
}

bool SST25_SelfTest()
{
    uint8_t id = SST25_ReadID();
    if((id) == 0)
        return false;


    //0x218 is working xD
    uint32_t dst = 0x0000000;


    SST25_Erase4K(dst);
    while(SST25_IsBusy());
    
    uint8_t data[] ="New test data";
    uint8_t rx_buff[sizeof(data)];
    SST25_WriteBytes(dst, data, sizeof(data));

    SST25_Read(dst, rx_buff, sizeof(data));

    for(uint8_t i =0; i<sizeof(data);i++)
    {
        if(data[i]!=rx_buff[i])
            return false;
    }


    return true;
}