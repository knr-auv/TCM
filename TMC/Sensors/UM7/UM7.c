#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "UM7.h"
#include "UM7_registers.h"
#include "Config/config.h"
#include "drivers/USART.h"
#include "time/time.h"

static quaternion_t position;
static float angular_velocity[3];
static bool newData = false;
static uint8_t rx_buffer[CONFIG_UM7_RX_BUFFER_SIZE];
static uint8_t tx_buffer[CONFIG_UM7_TX_BUFFER_SIZE];
static UM7_packet_t packet;
static USART_t uart;

bool UM7_NewData(void)
{
    bool temp = newData;
    newData = false;
    return temp;
}
float *UM7_GetAngVelPointer(void)
{
    return angular_velocity;
}
quaternion_t *UM7_GetOrientationPointer(void)
{
    return &position;
}
bool checkChecksum(uint8_t *data, uint16_t len)
{
    uint16_t sum = 0;
    for (uint16_t i = 0; i < len - 2; i++)
    {
        sum += (uint16_t)data[i];
    }
    return ((data[len - 1 - 1] << 8 | data[len - 1]) == sum) ? true : false;
}

static uint16_t calculateChecksum(uint8_t *data, uint16_t len)
{
    uint16_t sum = 0;
    for (uint16_t i = 0; i < len; i++)
    {
        sum += (uint16_t)data[i];
    }
    return sum;
}
void UM7_HandleQuaternionPacket(UM7_packet_t *packet)
{
    int16_t temp = 0;
    if ((packet->address == DREG_QUAT_AB) && packet->is_batch && packet->data_lenght >= 8)
    {
        temp = packet->data[1] | (packet->data[0] << 8);
        position.r = temp / 29789.09091f;

        temp = packet->data[3] | (packet->data[2] << 8);
        position.i = temp / 29789.09091f;

        temp = packet->data[5] | (packet->data[4] << 8);
        position.j = temp / 29789.09091f;
        temp = packet->data[7] | (packet->data[6] << 8);
        position.k = temp / 29789.09091f;
    }
    newData = true;
}

void UM7_HandleEulerAnglesPacket(UM7_packet_t *packet)
{
  
    int16_t temp = 0;
    if ((packet->address == DREG_EULER_PHI_THETA) && packet->is_batch && packet->data_lenght >= 8)
    {
        //first 8 bits are euler angles
        //than i have roll, pitch, yaw rates
        temp = packet->data[9] | (packet->data[8] << 8);
        angular_velocity[0] = temp/16.f;

        temp = packet->data[11] | (packet->data[10] << 8);
        angular_velocity[1] = temp /16.f;

        temp = packet->data[13] | (packet->data[12] << 8);
        angular_velocity[2] = temp /16.f;

    }
    newData = true;
}
void UM7_HandleGyroProcesedPacket(UM7_packet_t *packet)
{
    if ((packet->address == DREG_GYRO_PROC_X) && packet->is_batch && packet->data_lenght >= 12)
    {
        uint32_t temp = 0;
        for (uint8_t i= 0; i < 12; i += 4)
        {
            temp = packet->data[3 + i] | (packet->data[2 + i] << 8) | (packet->data[1 + i] << 16) | (packet->data[0 + i] << 24);
            memcpy(angular_velocity + (i >> 2), &temp, sizeof(uint32_t));
        }
    }
}
void UM7_HandleHealthPacket(UM7_packet_t *packet)
{
}
void UM7_RC_Complete_Callback(void)
{
    if (UM7_HandleSerialData(rx_buffer, uart.GetReceivedBytes(), &packet) == PI_GOOD)
    {
        if ((packet.address == DREG_QUAT_AB) || (packet.address == DREG_QUAT_CD))
            UM7_HandleQuaternionPacket(&packet);
        else if (packet.address == DREG_HEALTH)
            UM7_HandleHealthPacket(&packet);
        else if (packet.address == DREG_GYRO_PROC_X)
            UM7_HandleGyroProcesedPacket(&packet);
        else if (packet.address == DREG_EULER_PHI_THETA)
            UM7_HandleEulerAnglesPacket(&packet);
    }
    uart.ReceiveDMA(rx_buffer, CONFIG_UM7_RX_BUFFER_SIZE);
}
bool UM7_Init(void)
{
    USART_t* u = USART_GetUSART(CONFIG_UM7_USART);
    u->RXCompleteCallback = UM7_RC_Complete_Callback;
    uart = *u;//
    uart.ReceiveDMA(rx_buffer, CONFIG_UM7_RX_BUFFER_SIZE);
    return true;
}

packet_info_e UM7_HandleSerialData(uint8_t *data, uint16_t len, UM7_packet_t *packet)
{
    if (!checkChecksum(data, len))
        return PI_BAD;
    uint8_t PT = data[3];
    packet->address = data[4];
    packet->has_data = (PT & (1 << PT_HAS_DATA));
    packet->is_batch = (PT & (1 << PT_IS_BATCH));
    if (packet->is_batch & packet->has_data)
    {
        packet->data_lenght = 4 * ((PT & (1 << PT_BL0 | 1 << PT_BL1 | 1 << PT_BL2 | 1 << PT_BL3)) >> 2);
    }
    else if (packet->has_data)
    {
        packet->data_lenght = 4;
    }
    for (uint8_t i = 0; i < packet->data_lenght; i++)
    {
        packet->data[i] = data[5 + i];
    }

    return PI_GOOD;
}

uint16_t createMessage(uint8_t *tx_buffer, uint8_t address, bool has_data, bool is_batch, uint8_t *data, uint8_t data_len)
{
    tx_buffer[0] = 's';
    tx_buffer[1] = 'n';
    tx_buffer[2] = 'p';

    tx_buffer[3] = ((uint8_t)0 | ((uint8_t)is_batch << PT_IS_BATCH) | ((uint8_t)has_data << PT_HAS_DATA));
    tx_buffer[4] = address;
    for (uint8_t i = 0; i < data_len; i++)
    {
        tx_buffer[5 + i] = data[i]; //
    }
    uint16_t sum = calculateChecksum(tx_buffer, 5 + data_len);
    tx_buffer[5 + data_len] = (uint16_t)(sum >> 8);
    tx_buffer[5 + data_len + 1] = (uint8_t)sum;

    return 5 + data_len + 2;
}
void UM7_HEALTH_RATE(uint8_t health)
{
    uint8_t data[4];
    data[0] = 0;
    data[1] = 0;
    data[2] = health >> 8;
    data[3] = 255;
    uint16_t len = createMessage(tx_buffer, CREG_COM_RATES6, true, false, data, 4);
    uart.TransmitDMA(tx_buffer, len);
}
void UM7_ZERO_GYROS(void)
{

    uint16_t len = createMessage(tx_buffer, ZERO_GYROS, false, false, 0, 0);
    uart.TransmitDMA(tx_buffer, len);
}
void UM7_MISC(bool pps, bool zg, bool mag, bool quatMode)
{
    uint8_t data[4];
    data[0] = (mag | quatMode << 1 | zg << 2);
    data[1] = pps;
    data[2] = data[3] = 0;
    uint16_t len = createMessage(tx_buffer, CREG_MISC_SETTINGS, true, false, data, 4);
    uart.TransmitDMA(tx_buffer, len);
}
void UM7_NMEA_PACKETS(uint8_t health, uint8_t pose, uint8_t att, uint8_t sensor)
{
    uint8_t data[4];
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    uint16_t len = createMessage(tx_buffer, CREG_COM_RATES7, true, false, data, 4);
    uart.TransmitDMA(tx_buffer, len);
}
void UM7_FACTORY_RESET(void)
{
    uint16_t len = createMessage(tx_buffer, RESET_TO_FACTORY, false, false, 0, 0);
    uart.TransmitDMA(tx_buffer, len);
}
void UM7_RAW_Rates(uint8_t acc, uint8_t gyro, uint8_t mag)
{
    uint8_t data[4];
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    uint16_t len = createMessage(tx_buffer, CREG_COM_RATES2, true, false, data, 4);
    uart.TransmitDMA(tx_buffer, len);

    data[0] = 0;
    data[1] = mag;
    data[2] = gyro;
    data[3] = acc;
    while (!uart.CheckTxEnd())
    {
    };
    len = createMessage(tx_buffer, CREG_COM_RATES1, true, false, data, 4);
    uart.TransmitDMA(tx_buffer, len);
}
void UM7_PROCCESED_Rates(uint8_t acc, uint8_t gyro, uint8_t mag)
{
    uint8_t data[4];
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    uint16_t len = createMessage(tx_buffer, CREG_COM_RATES4, true, false, data, 4);
    uart.TransmitDMA(tx_buffer, len);

    data[0] = 0;
    data[1] = mag;
    data[2] = gyro;
    data[3] = acc;
    while (!uart.CheckTxEnd())
    {
    };
    len = createMessage(tx_buffer, CREG_COM_RATES3, true, false, data, 4);
    uart.TransmitDMA(tx_buffer, len);
}
void UM7_QUAT_EUL_POS_VEL_Rates(uint8_t quat, uint8_t eul, uint8_t pos, uint8_t vel)
{
    uint8_t data[4];
    data[0] = vel;
    data[1] = pos;
    data[2] = eul;
    data[3] = quat;
    uint16_t len = createMessage(tx_buffer, CREG_COM_RATES5, true, false, data, 4);
    uart.TransmitDMA(tx_buffer, len);
    DelayMs(100);
}