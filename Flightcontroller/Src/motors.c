#include "motors.h"

TIM_HandleTypeDef* timer_handle;

int16_t esc_1, esc_2, esc_3, esc_4;
int16_t manual_takeoff_throttle = 1350;     //Enter the manual hover point when auto take-off detection is not desired (between 1400 and 1600).
int16_t takeoff_throttle; //difference between 1500 and hover throttle
int16_t throttle;
void init_motors(TIM_HandleTypeDef* handle)
{
	timer_handle = handle;
	
	HAL_TIM_PWM_Init(timer_handle);
	HAL_TIM_PWM_Start(timer_handle, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(timer_handle, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(timer_handle, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(timer_handle, TIM_CHANNEL_4);
	
	timer_handle->Instance->CCR1 = 1000;
	timer_handle->Instance->CCR2 = 1000;
	timer_handle->Instance->CCR3 = 1000;
	timer_handle->Instance->CCR4 = 1000;
	
	if (manual_takeoff_throttle != 0)
		takeoff_throttle = 1500 - manual_takeoff_throttle;
}

void set_motors(int16_t throttle_input)
{
	//throttle is channel[2]
	//TODO: Battery compensation
	if (state == FLIGHT)
	{
		throttle = throttle_input + takeoff_throttle;                                           //The base throttle is the receiver throttle channel + the detected take-off throttle.
		if(mode >= ALTITUDE) {//If altitude mode is active.														
			throttle = 1500 + takeoff_throttle + pid_output_altitude + manual_throttle;     //The base throttle is the receiver throttle channel + the detected take-off throttle + the PID controller output.
		}
		
		if (throttle > 1800) throttle = 1800;                                           //We need some room to keep full control at full throttle.
		esc_1 = throttle - pid_output_pitch + pid_output_roll - pid_output_yaw;
		esc_2 = throttle + pid_output_pitch + pid_output_roll + pid_output_yaw;
		esc_3 = throttle + pid_output_pitch - pid_output_roll - pid_output_yaw;
		esc_4 = throttle - pid_output_pitch - pid_output_roll + pid_output_yaw;
	
		if (esc_1 < 1100) esc_1 = 1100; //Keep the motors running
		if (esc_2 < 1100) esc_2 = 1100;
		if (esc_3 < 1100) esc_3 = 1100;
		if (esc_4 < 1100) esc_4 = 1100;
	
		if (esc_1 > 2000) esc_1 = 2000; //Limit the pulses
		if (esc_2 > 2000) esc_2 = 2000;
		if (esc_3 > 2000) esc_3 = 2000;
		if (esc_4 > 2000) esc_4 = 2000;
	}
	else
	{
		esc_1 = 1000;
		esc_2 = 1000;
		esc_3 = 1000;
		esc_4 = 1000;
	}
	if (state == TEST)
	{
		esc_1 = esc_2 = esc_3 = esc_4 = throttle;
	}
	
	timer_handle->Instance->CCR1 = esc_1;
	timer_handle->Instance->CCR2 = esc_2;
	timer_handle->Instance->CCR3 = esc_3;
	timer_handle->Instance->CCR4 = esc_4;
	timer_handle->Instance->CNT = 5000; //This will reset the timer and the ESC pulses are directly created.
}
