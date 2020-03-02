#include "motors.h"

TIM_HandleTypeDef* timer_handle;

int16_t esc_1, esc_2, esc_3, esc_4;

void init_motors(TIM_HandleTypeDef* handle)
{
	timer_handle = handle;
	
	HAL_TIM_PWM_Init(timer_handle);
	HAL_TIM_PWM_Start(timer_handle, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(timer_handle, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(timer_handle, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(timer_handle, TIM_CHANNEL_4);
}

void set_motors(int16_t throttle)
{
	esc_1 = esc_2 = esc_3 = esc_4 = throttle;
	
	
	timer_handle->Instance->CCR1 = esc_1;
	timer_handle->Instance->CCR2 = esc_2;
	timer_handle->Instance->CCR3 = esc_3;
	timer_handle->Instance->CCR4 = esc_4;
	//timer_handle->Instance->CNT = 5000; //This will reset timer 4 and the ESC pulses are directly created.
}
