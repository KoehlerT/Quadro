#include "send_info.h"

#define BUFFER_LENGTH 40
uint16_t crc16(uint16_t crc, uint8_t* ptr, int length);

uint8_t transmitIndex = 0;
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
	//A Package of telemetry data is set up once. Then transmittet for a number of cycles
	if (transmitIndex >= BUFFER_LENGTH)
	{
		for (int i = 0; i < BUFFER_LENGTH - 2; i++)
			buffer[i] = 0;
		buffer[BUFFER_LENGTH - 2] = '\n';
		buffer[BUFFER_LENGTH - 1] = '\r';
		
		buffer[0x00] = mode;
		buffer[0x01] = error;
		buffer[0x02] = (int8_t)angle_roll;
		buffer[0x03] = (int8_t)angle_pitch;
		
		buffer[0x0B] = (uint8_t)battery_voltage * 10;
		
		buffer[0x14] = esc_1;
		buffer[0x15] = esc_1 >> 8;
		buffer[0x16] = esc_2;
		buffer[0x17] = esc_2 >> 8;
		buffer[0x18] = esc_3;
		buffer[0x19] = esc_3 >> 8;
		buffer[0x1A] = esc_4;
		buffer[0x1B] = esc_4 >> 8;
		for (int i = 0; i < 4; i++)
		{
			buffer[0x1C + i * 2] = channel[i];
			buffer[0x1C + i * 2 + 1] = channel[i] >> 8;
		}
		uint16_t crc = crc16(0xFFFF, buffer, 40-4);
		buffer[0x24] = crc;
		buffer[0x25] = crc >> 8;
		
		buffer[BUFFER_LENGTH - 2] = '\n';
		buffer[BUFFER_LENGTH - 1] = '\r';
		transmitIndex = 0;
	}else
	{
		//Transmit the next 4 bytes
		HAL_UART_Transmit(handle, &buffer[transmitIndex], 4, 100);
		transmitIndex += 4;
	}
}

//https://forums.anandtech.com/threads/converting-a-16-bit-crc-function-from-c-to-c-fixed-thanks.2161349/
uint16_t crc16(uint16_t crc, uint8_t* ptr, int length)
{
	auto uint16_t i;
	while (length--)
	{
		crc = crc ^ (uint16_t) *ptr++;
		for (i = 0; i < 8; i++)
		{
			if (crc & 0x0001)
				crc = (crc >> 1) ^ 0x8408;
			else
				crc >>= 1;
		}
	}
	return crc;
}