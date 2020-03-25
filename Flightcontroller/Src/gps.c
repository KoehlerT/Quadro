#include "gps.h"


int64_t latitude;
int64_t longitude;
int8_t num_sats;
int8_t fix_type;

int64_t lat_gps_actual;
int64_t lon_gps_actual;
int8_t latitude_north;
int8_t longiude_east;

uint8_t gps_receive_buffer[200]; //Buffer for read data
uint8_t gps_line_buffer[100];
int16_t gps_line_buffer_position = 0;
void process_gps_line();
void process_buffer(uint8_t* start, uint8_t length);

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
	HAL_UART_Receive_DMA(uart, gps_receive_buffer, 200);
	
}
void read_gps(UART_HandleTypeDef* uart)
{
	
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	process_buffer(&gps_receive_buffer[100], 100); //process second halve
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	process_buffer(&gps_receive_buffer[0], 100); //Process first halve
}

void process_buffer(uint8_t* start, uint8_t length)
{
	//Processing buffer works as follows:
	//start reading starting line from previous transmit
	//extract lines, read lat, long, num gps, lock tyoe
	//if not finished, push remaining line into previous transmit buffer
	
	uint16_t process_position = 0;
	//add characters to line buffer until line is finished
	while(process_position < length)
	{
		
		gps_line_buffer[gps_line_buffer_position] = start[process_position]; //transcribe processed data to line buffer
		gps_line_buffer_position++; process_position++;
		
		if(gps_line_buffer[gps_line_buffer_position-1] == '*') //if line is completed, process it and clear it for the new line
		{
			process_gps_line();
			for (int i = 0; i < gps_line_buffer_position; i++) gps_line_buffer[i] = 0;   //clear line buffer
			gps_line_buffer_position = 0;   //
		}
		
		
	}//if line is not finished during processing the new received lines, it is saved in the gps_line_buffer for the next transfer
	
}

void process_gps_line()
{
	//line structure: num, num, \r \n, $, msg, msg, msg, data... , *
	//so the indices are 4 more than joop brokking
	if (gps_line_buffer[8] == 'L' && gps_line_buffer[9] == 'L' && gps_line_buffer[11] == ',') {
		     //When there is no GPS fix or latitude/longitude information available.
      //Set some variables to 0 if no valid information is found by the GPS module. This is needed for GPS lost when flying.
		longitude = 0;
		latitude = 0;
		/*lat_gps_previous = 0;
		lon_gps_previous = 0;*/
		num_sats = 0;
	}
	if (gps_line_buffer[8] == 'G' && gps_line_buffer[9] == 'A' && (gps_line_buffer[44] == '1' || gps_line_buffer[44] == '2')) //lat long, sats
	{
		lat_gps_actual = ((int)gps_line_buffer[23] - 48) *  (long)10000000;                               //Filter the minutes for the GGA line multiplied by 10.
		lat_gps_actual += ((int)gps_line_buffer[24] - 48) * (long)1000000;                                //Filter the minutes for the GGA line multiplied by 10.
		lat_gps_actual += ((int)gps_line_buffer[26] - 48) * (long)100000;                                 //Filter the minutes for the GGA line multiplied by 10.
		lat_gps_actual += ((int)gps_line_buffer[27] - 48) * (long)10000;                                  //Filter the minutes for the GGA line multiplied by 10.
		lat_gps_actual += ((int)gps_line_buffer[28] - 48) * (long)1000;                                   //Filter the minutes for the GGA line multiplied by 10.
		lat_gps_actual += ((int)gps_line_buffer[29] - 48) * (long)100;                                    //Filter the minutes for the GGA line multiplied by 10.
		lat_gps_actual += ((int)gps_line_buffer[30] - 48) * (long)10;                                     //Filter the minutes for the GGA line multiplied by 10.
		lat_gps_actual /= (long)6;                                                                          //To convert the minutes to degrees we need to divide the minutes by 6.
		lat_gps_actual += ((int)gps_line_buffer[21] - 48) *  (long)100000000;                             //Add the degrees multiplied by 10.
		lat_gps_actual += ((int)gps_line_buffer[24] - 48) *  (long)10000000;                              //Add the degrees multiplied by 10.
		lat_gps_actual /= 10;                                                                               //Divide everything by 10.

		lon_gps_actual = ((int)gps_line_buffer[37] - 48) *  (long)10000000;                               //Filter the minutes for the GGA line multiplied by 10.
		lon_gps_actual += ((int)gps_line_buffer[38] - 48) * (long)1000000;                                //Filter the minutes for the GGA line multiplied by 10.
		lon_gps_actual += ((int)gps_line_buffer[40] - 48) * (long)100000;                                 //Filter the minutes for the GGA line multiplied by 10.
		lon_gps_actual += ((int)gps_line_buffer[41] - 48) * (long)10000;                                  //Filter the minutes for the GGA line multiplied by 10.
		lon_gps_actual += ((int)gps_line_buffer[42] - 48) * (long)1000;                                   //Filter the minutes for the GGA line multiplied by 10.
		lon_gps_actual += ((int)gps_line_buffer[43] - 48) * (long)100;                                    //Filter the minutes for the GGA line multiplied by 10.
		lon_gps_actual += ((int)gps_line_buffer[44] - 48) * (long)10;                                     //Filter the minutes for the GGA line multiplied by 10.
		lon_gps_actual /= (long)6;                                                                          //To convert the minutes to degrees we need to divide the minutes by 6.
		lon_gps_actual += ((int)gps_line_buffer[34] - 48) * (long)1000000000;                             //Add the degrees multiplied by 10.
		lon_gps_actual += ((int)gps_line_buffer[35] - 48) * (long)100000000;                              //Add the degrees multiplied by 10.
		lon_gps_actual += ((int)gps_line_buffer[36] - 48) * (long)10000000;                               //Add the degrees multiplied by 10.
		lon_gps_actual /= 10;                                                                               //Divide everything by 10.

		if(gps_line_buffer[32] == 'N')latitude_north = 1;                                                //When flying north of the equator the latitude_north variable will be set to 1.
		else latitude_north = 0;                                                                            //When flying south of the equator the latitude_north variable will be set to 0.

		if(gps_line_buffer[46] == 'E')longiude_east = 1;                                                 //When flying east of the prime meridian the longiude_east variable will be set to 1.
		else longiude_east = 0;                                                                             //When flying west of the prime meridian the longiude_east variable will be set to 0.

		num_sats = ((int)gps_line_buffer[50] - 48) * (long)10;                                    //Filter the number of satillites from the GGA line.
		num_sats += (int)gps_line_buffer[51] - 48;                                                 //Filter the number of satillites from the GGA line.
	
		latitude = lat_gps_actual;
		longitude = lon_gps_actual;
	}
	
	//If the line starts with SA and if there is a GPS fix we can scan the line for the fix type (none, 2D or 3D).
    if(gps_line_buffer[8] == 'S' && gps_line_buffer[9] == 'A')fix_type = (int)gps_line_buffer[13] - 48;
}