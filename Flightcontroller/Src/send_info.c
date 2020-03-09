#include "send_info.h"

#define BUFFER_LENGTH 20
#define CYCLE_WAIT 5

uint8_t cycleCount = 0;
UART_HandleTypeDef* handle;
uint8_t buffer[BUFFER_LENGTH];

void init_info(UART_HandleTypeDef* uart)
{
	handle = uart;
	for (int i = 0; i < BUFFER_LENGTH-2; i++)
		buffer[i] = 0;
	buffer[BUFFER_LENGTH - 2] = '\n';
	buffer[BUFFER_LENGTH - 1] = '\r';
}

void send_info()
{
	cycleCount++;
	if (cycleCount % CYCLE_WAIT != 0)
		return;
	for (int i = 0; i < BUFFER_LENGTH - 2; i++)
		buffer[i] = 0;
	
	if (cycleCount == CYCLE_WAIT * 1)
	{
		//Send package 0
		buffer[0] = 0;
		buffer[1] = mode;
		buffer[2] = error;
		buffer[3] = (int8_t )angle_roll;
		buffer[4] = (int8_t)angle_pitch;
	}
	else if (cycleCount == CYCLE_WAIT * 2)
	{
		//Send package 1
		cycleCount = 0;
		buffer[0] = 1; //ID
		buffer[1] = esc_1;
		buffer[2] = esc_1 >> 8;
		buffer[3] = esc_2;
		buffer[4] = esc_2 >> 8;
		buffer[5] = esc_3;
		buffer[6] = esc_3 >> 8;
		buffer[7] = esc_4;
		buffer[8] = esc_4 >> 8;
		for (int i = 0; i < 4; i++)
		{
			buffer[9 + i * 2] = channel[i];
			buffer[9 + i * 2 + 1] = channel[i] >> 8;
		}
		
	}
	
	HAL_UART_Transmit(handle, buffer, BUFFER_LENGTH, 100);
}