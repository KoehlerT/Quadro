#include "gps.h"


int64_t latitude;
int16_t longitude;


uint8_t gps_receive_buffer[100]; //Buffer for read data


void init_gps(UART_HandleTypeDef* uart)
{
	// Disable GPGSV messages by using the ublox protocol.
	uint8_t Disable_GPGSV[11] = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x03, 0x00, 0xFD, 0x15 };
	HAL_UART_Transmit(uart, Disable_GPGSV, 11, 100);
	HAL_Delay(350);    //A small delay is added to give the GPS some time to respond @ 9600bps.
	//Set the refresh rate to 5Hz by using the ublox protocol.
	uint8_t Set_to_5Hz[14] = { 0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A };
	HAL_UART_Transmit(uart, Set_to_5Hz, 14, 100);
	HAL_Delay(350);     //A small delay is added to give the GPS some time to respond @ 9600bps.
	//Set the baud rate to 57.6kbps by using the ublox protocol.
	uint8_t Set_to_57kbps[28] = { 0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0xE1, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE2, 0xE1 };
	HAL_UART_Transmit(uart, Set_to_57kbps, 28, 100);
	HAL_Delay(200);
	
	uart->Init.BaudRate = 57600;
	HAL_UART_Init(uart);
	HAL_Delay(100);
	//HAL_UART_Receive_DMA(uart, gps_receive_buffer, 100);
	
}
void read_gps(UART_HandleTypeDef* uart)
{
	
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	
}

void process_buffer(uint8_t* start, uint8_t length)
{
	
}