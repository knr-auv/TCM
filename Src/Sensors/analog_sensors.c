#include "stm32f4xx.h"
#include <string.h>
#include "time/time.h"
#include <stdbool.h>
#include "Variables/variables.h"

#define ADC1_VREF  3.3f
#define SHT3x_v 3.3f
typedef enum{
    SENSOR_CPU_TEMP,
    SENSOR_ONBOARD_HUMIDITY,
    SENSOR_ONBOARD_TEMP,
    SENSOR_COUNT
}sensors_map_e;
#define SAMPLES 16

uint16_t raw_samples[SAMPLES][SENSOR_COUNT];
uint16_t raw_data[SENSOR_COUNT];

static float _cpu_temp = 0;
static float _onboard_Humidity = 0;
static float _onboard_Temp = 0;
static bool enabled = false;



bool ANALOG_IsEnabled()
{
    return enabled;
}
void ANALOG_Init()
{
    VAR_SetSysFloat(&_cpu_temp, VAR_SENSOR_CPU_TEMP);
    VAR_SetSysFloat(&_onboard_Humidity, VAR_SENSOR_ONBOARD_HUMMIDITY);
    VAR_SetSysFloat(&_onboard_Temp, VAR_SENSOR_ONBOARD_TEMP);
}
void ANALOG_Enable()
{
   // if(enabled)
     //   return;
    ADC1->CR2 |= ADC_CR2_ADON;
    DelayMs(1);
    for(uint8_t i =0; i<SENSOR_COUNT; i++)
    {
        for(uint8_t j =0; j< SAMPLES; j++)
        raw_samples[i][j] = 0;
    }
    ADC1->CR2 &= ~(ADC_CR2_SWSTART);
    DMA2_Stream0->CR &= ~DMA_SxCR_EN;
    DMA2_Stream0->NDTR = SENSOR_COUNT;
    DMA2_Stream0->PAR = (uint32_t)&ADC1->DR;
    DMA2_Stream0->M0AR = (uint32_t)raw_data;
    DMA2_Stream0->CR |= DMA_SxCR_EN;
    enabled = true;
}

void ANALOG_Disable(){
    DMA2_Stream0->CR &= ~DMA_SxCR_EN;
    ADC1->CR2 &= ~ADC_CR2_ADON;
    enabled = false;
    DelayMs(1);
}


void ANALOG_beginConversion(void)
{
    if(enabled){
        ADC1->CR2 |= ADC_CR2_SWSTART;
        }
}
void avg_value(uint32_t *dst, sensors_map_e sensor){
    *(dst) = 0;
    for(uint8_t i =0; i<SAMPLES; i++)
    {
        *(dst) += raw_samples[i][sensor];
    }
    *(dst) /=SAMPLES;
}

float ANALOG_onboardTemp(void){
    uint32_t sensor_data;
    avg_value(&sensor_data, SENSOR_ONBOARD_TEMP);
    volatile float read_val = (sensor_data/(float)4095)*ADC1_VREF;
    return -66.875 + 218.75 * read_val/SHT3x_v;
}
float ANALOG_CPUTemp(void){
    uint32_t sensor_data;
    avg_value(&sensor_data, SENSOR_CPU_TEMP);
    return  ((ADC1_VREF*sensor_data/(float)4095 - 0.76 ) / 0.0025) + 25;
}

float ANALOG_onboardHumidity(void){
    uint32_t sensor_data;
  avg_value(&sensor_data, SENSOR_ONBOARD_HUMIDITY);
    return -12.5 + 125*(sensor_data/(float)4095)*ADC1_VREF/SHT3x_v;;
}
void ANALOG_ProcesRAW()
{
    _cpu_temp = ANALOG_CPUTemp();
    _onboard_Humidity = ANALOG_onboardHumidity();
    _onboard_Temp = ANALOG_onboardTemp();
}
void ANALOG_stopConversion(void)
{
    DMA2_Stream0->CR &= ~DMA_SxCR_EN;
    enabled = false;
}

void DMA2_Stream0_IRQHandler(void)
{
    static uint8_t i =0;
    if(DMA2->LISR & DMA_LISR_TCIF0)
    {
        memcpy(raw_samples[i], raw_data, SENSOR_COUNT*sizeof(uint16_t));
        i = (i + 1)%SAMPLES;
    DMA2->LIFCR =(DMA_LIFCR_CTCIF0);
    }
}