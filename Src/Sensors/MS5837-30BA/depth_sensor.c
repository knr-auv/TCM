

#include "stm32f4xx.h"
#include "drivers/I2C2.h"
#include <stdbool.h>
#include "time/time.h"
#include "stddef.h"
#include "depth_sensor.h"
#include "IO/LED.h"
#include "tasks/tasks.h"

// variable for RX and TX purpose (you can use any other):
static uint8_t receiving_table[20];
static uint8_t transmitting_table[10];
float depth_sens_temperature = 0;
float depth_sens_pressure = 0;

// RX:
#if defined(USE_I2C2)
void DMA1_Stream2_IRQHandler(void)
{
    // if stream2 transfer is completed:
    if (DMA1->LISR & DMA_LISR_TCIF2)
    {
        DMA1->LIFCR |= DMA_LIFCR_CTCIF2;

        DMA1_Stream2->CR &= ~DMA_SxCR_EN;
        // generate STOP conditions:
        I2C2->CR1 |= I2C_CR1_STOP;
    }
}
#endif

bool depth_sens_checkFun(timeUs_t time, timeDelta_t delta_time)
{
    /*  This function is the main function of reading new data from depth sensor.
        It is triggered according to the set frequency (TASK_READ_DEPTH_SENSOR_HZ).
        Next, after CONVERSION_TIME raw pressure is read and temp conversion started.
        Similarly after CONVERSION_TIME raw temperature is read.
        At the end new values of temperature and pressure are calculated and updated.
        Also at the end .checkFun is set to NULL to disabling this function
        (it will be enable in .taskFun according to the scheduler).
    */
    static timeUs_t time_flag;
    enum state
    {
        PRESSURE_CONVERSION,
        TEMP_CONVERSION,
        UPDATE_MEASUREMENTS
    };

    static enum state current_state = PRESSURE_CONVERSION;

    switch (current_state)
    {
    case PRESSURE_CONVERSION:
        // after initialization in .taskFun wait for end of conversion:
        if (delta_time > CONVERSION_TIME)
        {
            // read pressure after conversion:
            I2C2_Write(SENSOR_ADDRESS, PRE_SENS_ADC_READ, NULL, 0);
            I2C2_Stop();
            I2C2_Read(SENSOR_ADDRESS, receiving_table, 3);

            // start TEMP conversion:
            I2C2_Write(SENSOR_ADDRESS, PRE_SENS_D2_1024, NULL, 0);
            I2C2_Stop();
            time_flag = micros();
            current_state = TEMP_CONVERSION;
        }
        break;
    case TEMP_CONVERSION:
        //  wait for end of conversion:
        if (time - time_flag > CONVERSION_TIME)
        {
            // read pressure after conversion:
            I2C2_Write(SENSOR_ADDRESS, PRE_SENS_ADC_READ, NULL, 0);
            I2C2_Stop();
            I2C2_Read(SENSOR_ADDRESS, &receiving_table[3], 3);
            current_state = UPDATE_MEASUREMENTS;
        }
        break;
    case UPDATE_MEASUREMENTS:

        // update variables:
        pressure_and_temp_calculation(receiving_table, &depth_sens_pressure, &depth_sens_temperature);
        current_state = PRESSURE_CONVERSION;
        // disable this function (now scheduler will decide of next measurements reading):
        tasks[TASK_READ_DEPTH_SENSOR].checkFun = NULL;
        break;
    }
    // always return false:
    return false;
}

void init_depth_sensor()
{
    pressure_sensor_reset();
}

void read_depth_sensor(timeUs_t time)
{
    // this function only starts process of reading depth sensor main part is in depth_sens_checkFun():
    // initialize pressure conversion:
    I2C2_Write(SENSOR_ADDRESS, PRE_SENS_D1_1024, NULL, 0);
    I2C2_Stop();
    // set .checkFun (check depth_sens_checkFun() for further explanation):
    tasks[TASK_READ_DEPTH_SENSOR].checkFun = depth_sens_checkFun;
}

// follow this code to get coefficients (only once, then use them as constants):
void calibrate_sensor()
{
    pressure_sensor_reset();
    pressure_sensor_PROM_read(receiving_table);
    get_coefficients(receiving_table);
}

void pressure_sensor_reset()
{
    I2C2_Write(SENSOR_ADDRESS, PRE_SENS_RESET, NULL, 0);
    I2C2_Stop();
}

void pressure_sensor_PROM_read(uint8_t *data)
{
    I2C2_Write(SENSOR_ADDRESS, PRE_SENS_PROM_READ_0, NULL, 0);
    I2C2_Stop();
    I2C2_Read(SENSOR_ADDRESS, data, 2);
    data += 2;
    I2C2_Write(SENSOR_ADDRESS, PRE_SENS_PROM_READ_1, NULL, 0);
    I2C2_Stop();
    I2C2_Read(SENSOR_ADDRESS, data, 2);
    data += 2;
    I2C2_Write(SENSOR_ADDRESS, PRE_SENS_PROM_READ_2, NULL, 0);
    I2C2_Stop();
    I2C2_Read(SENSOR_ADDRESS, data, 2);
    data += 2;
    I2C2_Write(SENSOR_ADDRESS, PRE_SENS_PROM_READ_3, NULL, 0);
    I2C2_Stop();
    I2C2_Read(SENSOR_ADDRESS, data, 2);
    data += 2;
    I2C2_Write(SENSOR_ADDRESS, PRE_SENS_PROM_READ_4, NULL, 0);
    I2C2_Stop();
    I2C2_Read(SENSOR_ADDRESS, data, 2);
    data += 2;
    I2C2_Write(SENSOR_ADDRESS, PRE_SENS_PROM_READ_5, NULL, 0);
    I2C2_Stop();
    I2C2_Read(SENSOR_ADDRESS, data, 2);
    data += 2;
    I2C2_Write(SENSOR_ADDRESS, PRE_SENS_PROM_READ_6, NULL, 0);
    I2C2_Stop();
    I2C2_Read(SENSOR_ADDRESS, data, 2);
}

void pressure_sensor_ADC_read(uint8_t *data)
{ // simplest version of reading data from sensor with usage of delay functions:
    // read pressure:
    I2C2_Write(SENSOR_ADDRESS, PRE_SENS_D1_1024, NULL, 0);
    I2C2_Stop();
    DelayUs(CONVERSION_TIME);
    I2C2_Write(SENSOR_ADDRESS, PRE_SENS_ADC_READ, NULL, 0);
    I2C2_Stop();
    I2C2_Read(SENSOR_ADDRESS, data, 3);

    // read temp:
    data += 3;
    I2C2_Write(SENSOR_ADDRESS, PRE_SENS_D2_1024, NULL, 0);
    I2C2_Stop();
    DelayUs(CONVERSION_TIME);
    I2C2_Write(SENSOR_ADDRESS, PRE_SENS_ADC_READ, NULL, 0);
    I2C2_Stop();
    I2C2_Read(SENSOR_ADDRESS, data, 3);
}

void pressure_and_temp_calculation(uint8_t *raw_data, float *pressure, float *temperature)
{
    /* implemented according to page 10, 11, 12:
     *	https://www.te.com/commerce/DocumentDelivery/DDEController?Action=showdoc&DocId=Data+Sheet%7FMS5837-30BA%7FB1%7Fpdf%7FEnglish%7FENG_DS_MS5837-30BA_B1.pdf%7FCAT-BLPS0017
     */

    uint32_t D1;
    uint32_t D2;
    int64_t dT;
    int64_t TEMP;
    int64_t OFF;
    int64_t SENS;
    int64_t P;
    int64_t Ti;
    int64_t OFFi;
    int64_t SENSi;

    // create raw data from read bytes:
    D1 = *raw_data << 16 | *(raw_data + 1) << 8 | *(raw_data + 2);
    raw_data += 3;
    D2 = *raw_data << 16 | *(raw_data + 1) << 8 | *(raw_data + 2);

    // first order temperature compensation:
    dT = D2 - C5_T * 256;
    TEMP = (2000 + dT * C6_TEMPSENS / 8388608);
    OFF = C2_OFF * 65536 + C4_TCO * dT / 128;
    SENS = C1_SENS * 32768 + C3_TCS * dT / 256;
    P = (D1 * SENS / 2097152 - OFF) / 8192;

    // second order of compensation:
    if (TEMP / 100 < 20)
    {
        Ti = 3 * dT * dT / 8589934592;
        OFFi = 3 * (TEMP - 2000) * (TEMP - 2000) / 2;
        SENSi = 5 * (TEMP - 2000) * (TEMP - 2000) / 8;
        if (TEMP / 100 < -15)
        {
            OFFi = OFFi + 7 * (TEMP + 1500) * (TEMP + 1500);
            SENSi = SENSi + 4 * (TEMP + 1500) * (TEMP + 1500);
        }
    }
    else
    {
        Ti = 2 * dT * dT / 137438953472.;
        OFFi = (TEMP - 2000) * (TEMP - 2000) / 16;
        SENSi = 0;
    }
    *temperature = (TEMP - Ti) / 100.f;                                           // in [C]
    *pressure = (((D1 * (SENS - SENSi)) / 2097152 - (OFF - OFFi)) / 8192) / 10.f; // in [mbar]
}

bool get_coefficients(uint8_t *data)
{
    uint16_t n_prom[8]; // n_prom defined as 8x uint16_t (for coefficients)
    uint16_t n_rem = 0; // crc remainder
    uint16_t crc_value = 0x00;

    // create coefficients from bytes:
    for (uint8_t i = 0; i < 8; i++)
    {
        n_prom[i] = *data << 8 | *(data + 1);
        data += 2;
    }

    // CRC checking:
    crc_value = n_prom[0] >> 12;        // save read CRC value
    n_prom[0] = ((n_prom[0]) & 0x0FFF); // CRC byte is replaced by 0
    n_prom[7] = 0;                      // Subsidiary value, set to 0

    for (uint8_t cnt = 0; cnt < 16; cnt++)
    { // operation is performed on bytes
        // choose LSB or MSB
        if (cnt % 2 == 1)
            n_rem ^= (uint8_t)((n_prom[cnt >> 1]) & 0x00FF);
        else
            n_rem ^= (uint8_t)(n_prom[cnt >> 1] >> 8);
        for (uint8_t n_bit = 8; n_bit > 0; n_bit--)
        {
            if (n_rem & (0x8000))
            {
                n_rem = (n_rem << 1) ^ 0x3000;
            }
            else
            {
                n_rem = (n_rem << 1);
            }
        }
    }
    n_rem = ((n_rem >> 12) & 0x000F); // final 4-bit remainder is CRC code
    if (crc_value == (n_rem ^ 0x00))
    {
        return true;
    }
    else
    {
        return false;
    }
}