#include "send_info.h"

#define BUFFER_LENGTH 40
uint16_t crc16(uint16_t crc, uint8_t* ptr, int length);

uint8_t transmitIndex = 0;
//UART_HandleTypeDef* uart_hand;
uint8_t tel_trans_buff[BUFFER_LENGTH];

void init_info(UART_HandleTypeDef* uart)
{
	
	//uart = uart;
	HAL_UART_Init(uart);
	for (int i = 0; i < BUFFER_LENGTH-2; i++)
		tel_trans_buff[i] = 0;
	tel_trans_buff[BUFFER_LENGTH - 2] = '\n';
	tel_trans_buff[BUFFER_LENGTH - 1] = '\r';
	HAL_StatusTypeDef state = HAL_UART_Transmit(uart, tel_trans_buff, 40, 1000);
	
	
}

void send_info(UART_HandleTypeDef* uart)
{
	//A Package of telemetry data is set up once. Then transmittet for a number of cycles
	if (transmitIndex >= BUFFER_LENGTH)
	{
		for (int i = 0; i < BUFFER_LENGTH - 2; i++)
			tel_trans_buff[i] = 0;
		tel_trans_buff[BUFFER_LENGTH - 2] = '\n';
		tel_trans_buff[BUFFER_LENGTH - 1] = '\r';
		
		tel_trans_buff[0x00] = mode;
		tel_trans_buff[0x01] = error;
		tel_trans_buff[0x02] = (int8_t)angle_roll;
		tel_trans_buff[0x03] = (int8_t)angle_pitch;
		tel_trans_buff[0x04] = temperature * 10;
		//buffer[0x05] empty
		uint16_t alt_send = (uint16_t)((altitude_meters * 100) + 10000);
		tel_trans_buff[0x06] = alt_send;
		tel_trans_buff[0x07] = alt_send >> 8;
		
		tel_trans_buff[0x0B] = (uint8_t)(battery_voltage * 10);
		
		tel_trans_buff[0x14] = esc_1;
		tel_trans_buff[0x15] = esc_1 >> 8;
		tel_trans_buff[0x16] = esc_2;
		tel_trans_buff[0x17] = esc_2 >> 8;
		tel_trans_buff[0x18] = esc_3;
		tel_trans_buff[0x19] = esc_3 >> 8;
		tel_trans_buff[0x1A] = esc_4;
		tel_trans_buff[0x1B] = esc_4 >> 8;
		for (int i = 0; i < 4; i++)
		{
			tel_trans_buff[0x1C + i * 2] = channel[i];
			tel_trans_buff[0x1C + i * 2 + 1] = channel[i] >> 8;
		}
		uint16_t crc = crc16(0xFFFF, tel_trans_buff, 40 - 4);
		tel_trans_buff[0x24] = crc;
		tel_trans_buff[0x25] = crc >> 8;
		
		tel_trans_buff[BUFFER_LENGTH - 2] = '\n';
		tel_trans_buff[BUFFER_LENGTH - 1] = '\r';
		transmitIndex = 0;
	}else
	{
		//Transmit the next 4 bytes
		HAL_StatusTypeDef state = HAL_UART_Transmit(uart, &tel_trans_buff[transmitIndex], 4, 100);
		if(state != HAL_OK)
			hardwareFaultRegister |= 0b00001000;
		transmitIndex += 4;
	}
}

void USART1_IRQHandler(void)
{
	return;
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