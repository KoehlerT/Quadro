#include "adc.h"


float battery_voltage;
uint32_t value;
//ADC_HandleTypeDef* handle;
void init_adc(ADC_HandleTypeDef * handle)
{
	//handle = h;
	handle->Init.ContinuousConvMode = DISABLE;
	HAL_ADC_Start_IT(handle);
	battery_voltage = 0;
	
	/*HAL_StatusTypeDef state = HAL_ADC_PollForConversion(handle, 1000);
	value = HAL_ADC_GetValue(handle);
	battery_voltage = value * 0.008864f; //initial battery voltage value*/
}
void poll_adc_value(ADC_HandleTypeDef * handle)
{
	HAL_ADC_Start_IT(handle);
	
	/*HAL_StatusTypeDef state = HAL_ADC_PollForConversion(handle, 1000);
	if (state == HAL_OK)
	{
		value = HAL_ADC_GetValue(handle);
		//Calculate battery voltage: voltage at pin =  value * 3.3V / 4095
		//Battery voltage is divided by 10k and 1k resistor. Battery voltage = Vpin * 11/1; Vbat = val * 0.008864f
		//A filter is used for more stable values
		
		battery_voltage = battery_voltage  * 0.92 + value * 0.000709f;
	}*/
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
	value = HAL_ADC_GetValue(AdcHandle);
	if (battery_voltage == 0)
		battery_voltage = value * 0.008864f; //Initial Battery voltage value
	else
		battery_voltage = battery_voltage  * 0.92 + value * 0.000709f; //Filtered battery voltage value
	if(battery_voltage < 10)
		set_error(BATTERY_LOW);
}

