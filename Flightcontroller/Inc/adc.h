#ifndef ADC_H
#define ADC_H
#include "stm32f4xx_hal.h"

void init_adc(ADC_HandleTypeDef * handle);
void poll_adc_value();
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle);


extern float battery_voltage;

#endif
