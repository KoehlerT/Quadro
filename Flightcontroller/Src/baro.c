#include "baro.h"

#define MS5611_addr 0x77 << 1
#ifdef STATEMACHINE_H
#define CheckedI2c(x) if(x != HAL_OK ) set_error(BARO_READ_ERR);
#else
#define CheckedI2c(x) if (x != HAL_OK);
#endif // STATEMACHINE_H




//Public variables
float altitude_meters, actual_pressure, baro_temp;

I2C_HandleTypeDef* handle;
int64_t pow(int16_t base, int16_t exp);

uint16_t C[7];
uint8_t barometer_counter, temperature_counter, average_temperature_mem_location;
int64_t OFF, OFF_C2, SENS, SENS_C1, P;
uint32_t raw_pressure, raw_temperature, temp, raw_temperature_rotating_memory[6], raw_average_temperature_total;
float actual_pressure_slow, actual_pressure_fast, actual_pressure_diff;
float ground_pressure, altutude_hold_pressure;
int32_t dT, dT_C5;

int32_t pressure_rotating_mem[50], pressure_total_avarage;
uint8_t pressure_rotating_mem_location;
float pressure_rotating_mem_actual;

uint8_t baro_buff[5];

void init_baro(I2C_HandleTypeDef* i2cHandle)
{
	handle = i2cHandle;
	//For calculating the pressure the 6 calibration values need to be polled from the MS5611.
	//These 2 byte values are stored in the memory location 0xA2 and up.
	baro_buff[0] = (0xA0);                                //Send the address that we want to read.
	for(int i = 1 ; i <= 6 ; i++) {
		baro_buff[0] = (0xA0 + i * 2);                                 //Send the address that we want to read.
		HAL_I2C_Master_Transmit(handle, MS5611_addr, baro_buff, 1, 100);

		HAL_I2C_Master_Receive(handle, MS5611_addr, baro_buff, 2, 100);                       //Request 2 bytes from the MS5611.
		C[i] = baro_buff[0] << 8 | baro_buff[1];                 //Add the low and high byte to the C[x] calibration variable.
	}

	OFF_C2 = C[2] * pow(2, 16);                                    //This value is pre-calculated to offload the main program loop.
	SENS_C1 = C[1] * pow(2, 15);                                   //This value is pre-calculated to offload the main program loop.

	//The MS5611 needs a few readings to stabilize.
	for(int i = 0 ; i < 100 ; i++) {							//This loop runs 100 times.
		read_baro();                                            //Read and calculate the barometer data.
		HAL_Delay(4);                                                    //The main program loop also runs 250Hz (4ms per loop).
	}
	actual_pressure = 0;                                           //Reset the pressure calculations.
	ground_pressure = 96000;
	
	reset_pid_altitude(); //reset pid altitude for enabeling manual altitude change etc
}
void read_baro()
{
	barometer_counter++;

	//Every time this function is called the barometer_counter variable is incremented. This way a specific action
	//is executed at the correct moment. This is needed because requesting data from the MS5611 takes around 9ms to complete.

	if(barometer_counter == 1) {
															//When the barometer_counter variable is 1.
		if(temperature_counter == 0) {
			//And the temperature counter is 0.
			//Get temperature data from MS-5611
			baro_buff[0] = 0x00;                                                //Send a 0 to indicate that we want to poll the requested data.                                                //End the transmission with the MS5611.
			set_sending_flag();
			HAL_I2C_Master_Transmit_IT(handle, MS5611_addr, baro_buff, 1);
			wait_to_send();
			set_receiving_flag();
			CheckedI2c(HAL_I2C_Master_Receive_IT(handle, MS5611_addr, baro_buff, 3));                          //Poll 3 data bytes from the MS5611.
			wait_to_receive();
			// Store the temperature in a 5 location rotating memory to prevent temperature spikes.
			raw_average_temperature_total -= raw_temperature_rotating_memory[average_temperature_mem_location];
			raw_temperature_rotating_memory[average_temperature_mem_location] = baro_buff[0] << 16 | baro_buff[1] << 8 | baro_buff[2];
			raw_average_temperature_total += raw_temperature_rotating_memory[average_temperature_mem_location];
			average_temperature_mem_location++;
			if (average_temperature_mem_location == 5)average_temperature_mem_location = 0;
			raw_temperature = raw_average_temperature_total / 5;                       //Calculate the avarage temperature of the last 5 measurements.
		}
		else {
			//Get pressure data from MS-5611
			baro_buff[0] = 0x00;
			set_sending_flag();
			HAL_I2C_Master_Transmit_IT(handle, MS5611_addr, baro_buff, 1);                                                        //Send a 0 to indicate that we want to poll the requested data.
			wait_to_send();
			set_receiving_flag();
			CheckedI2c(HAL_I2C_Master_Receive_IT(handle, MS5611_addr, baro_buff, 3))                                       //Poll 3 data bytes from the MS5611.
			wait_to_receive();
			raw_pressure = baro_buff[0] << 16 | baro_buff[1] << 8 | baro_buff[2];      //Shift the individual bytes in the correct position and add them to the raw_pressure variable.
		}

		temperature_counter++;                                                      //Increase the temperature_counter variable.
		if(temperature_counter == 20) {
			                                            //When the temperature counter equals 20.
		  temperature_counter = 0;                                                   //Reset the temperature_counter variable.
		  //Request temperature data
		  baro_buff[0] = 0x58;                                                     //Send a 0x58 to indicate that we want to request the temperature data.
			set_sending_flag();
			HAL_I2C_Master_Transmit_IT(handle, MS5611_addr, baro_buff, 1);                                                   //End the transmission with the MS5611.
			wait_to_send();
		}
		else {
			//If the temperature_counter variable does not equal 20.
//Request pressure data
			baro_buff[0] = 0x48;//Send a 0x48 to indicate that we want to request the pressure data.
			set_sending_flag();
			HAL_I2C_Master_Transmit_IT(handle, MS5611_addr, baro_buff, 1);                                                //End the transmission with the MS5611.
			wait_to_send();
		}
	}
	if (barometer_counter == 2) {
		                                                 //If the barometer_counter variable equals 2.
	  //Calculate pressure as explained in the datasheet of the MS-5611.
	  dT = C[5];
		dT <<= 8;
		dT *= -1;
		dT += raw_temperature;
		OFF = OFF_C2 + ((int64_t)dT * (int64_t)C[4]) / pow(2, 7);
		SENS = SENS_C1 + ((int64_t)dT * (int64_t)C[3]) / pow(2, 8);
		P = ((raw_pressure * SENS) / pow(2, 21) - OFF) / pow(2, 15);
		//To get a smoother pressure value we will use a 20 location rotating memory.
		pressure_total_avarage -= pressure_rotating_mem[pressure_rotating_mem_location];                           //Subtract the current memory position to make room for the new value.
		pressure_rotating_mem[pressure_rotating_mem_location] = P;                                                 //Calculate the new change between the actual pressure and the previous measurement.
		pressure_total_avarage += pressure_rotating_mem[pressure_rotating_mem_location];                           //Add the new value to the long term avarage value.
		pressure_rotating_mem_location++;                                                                          //Increase the rotating memory location.
		if(pressure_rotating_mem_location == 20)pressure_rotating_mem_location = 0;                               //Start at 0 when the memory location 20 is reached.
		actual_pressure_fast = (float)pressure_total_avarage / 20.0;                                               //Calculate the average pressure of the last 20 pressure readings.

		//To get better results we will use a complementary fillter that can be adjusted by the fast average.
		actual_pressure_slow = actual_pressure_slow * (float)0.985 + actual_pressure_fast * (float)0.015;
		actual_pressure_diff = actual_pressure_slow - actual_pressure_fast;                                        //Calculate the difference between the fast and the slow avarage value.
		if(actual_pressure_diff > 8)actual_pressure_diff = 8;                                                     //If the difference is larger then 8 limit the difference to 8.
		if(actual_pressure_diff < -8)actual_pressure_diff = -8;                                                   //If the difference is smaller then -8 limit the difference to -8.
		//If the difference is larger then 1 or smaller then -1 the slow average is adjuste based on the error between the fast and slow average.
		if(actual_pressure_diff > 1 || actual_pressure_diff < -1)actual_pressure_slow -= actual_pressure_diff / 6.0;
		actual_pressure = actual_pressure_slow;  //The actual_pressure is used in the program for altitude calculations.
		
		altitude_meters = ((ground_pressure - actual_pressure) * 0.0842);	
		baro_temp = (float)(2000 + (dT * C[6])) / 8388608.0f;
	}
	if (barometer_counter == 3) {
		barometer_counter = 0;                                                                                     //Set the barometer counter to 0 for the next measurements.
		calculate_pid_altitude();
	}
}

void reset_ground_pressure()
{
	ground_pressure = actual_pressure;
	altitude_meters = 0;
}

int64_t pow(int16_t base, int16_t exp)
{
	int64_t res = 1;
	for (int i = 1; i <= exp; i++)
	{
		res *= base;
	}
	return res;
}