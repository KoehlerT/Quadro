#include "receiver.h"

uint16_t channel[6];
int32_t measured_time, measured_time_start;
uint8_t channel_select_counter;

void init_receiver(TIM_HandleTypeDef* htim)
{
	HAL_TIM_IC_Start_IT(htim, TIM_CHANNEL_1);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim)
{	
	if (htim->Instance == TIM2)
	{
		measured_time = htim->Instance->CCR1 - measured_time_start;
		if (measured_time < 0)measured_time += 0xFFFFFFFF;
		measured_time_start = htim->Instance->CCR1;
		
		if (measured_time > 3000)channel_select_counter = 0;
		else
		{
			channel[channel_select_counter] = (int ) (measured_time * 1.58f);//??
			channel_select_counter++;
		}
	}
}