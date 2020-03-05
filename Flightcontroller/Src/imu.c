#include "imu.h"

//Public gyroscope variables
int16_t gyro_axis[3];
int16_t acc_x, acc_y, acc_z;
int16_t gyro_pitch, gyro_roll, gyro_yaw;

//Public accelerometer variables
float angle_pitch, angle_roll, angle_yaw; 
int32_t acc_total_vector, acc_total_vector_at_start;
float angle_roll_acc, angle_pitch_acc;


//Public temperature variables
int16_t temperature;

//Helper vars
int16_t temp_raw;
//- Calibration vars
int16_t cal_int; //counts calibration readings
uint8_t level_calibration_on;
int32_t gyro_roll_cal, gyro_pitch_cal, gyro_yaw_cal;
int16_t acc_pitch_cal_value, acc_roll_cal_value;

I2C_HandleTypeDef* imu_handle;

int8_t init_gyro(I2C_HandleTypeDef* handle)
{
	imu_handle = handle;
	//Check if device is ready
	HAL_StatusTypeDef state = HAL_I2C_IsDeviceReady(handle, IMU_address, 2, 10);
	if (state != HAL_OK)
	{
		cal_int = 0;
		//return -1; //Device not ready
	}
	
	//Set Up Mup6050
	
	/*uint8_t buff[] = { 0x6B, 0x00, 0x1B, 0x08, 0x1C, 0x10, 0x1A, 0x03 };
	HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(imu_handle, IMU_address, buff, 8, 1000);
	if (st != HAL_OK)
		return -1;
	
	return 0;*/
	//Alternative
	uint8_t buffer[] = { 0x6B, 0x00 };
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(imu_handle, IMU_address, buffer, 2, 1000);
	if (status != HAL_OK)
		return -1;
	
	buffer[0] = 0x1B; buffer[1] = 0x08;
	HAL_I2C_Master_Transmit(imu_handle, IMU_address, buffer, sizeof(buffer), 1000);
	
	buffer[0] = 0x1C; buffer[1] = 0x10;
	HAL_I2C_Master_Transmit(imu_handle, IMU_address, buffer, sizeof(buffer), 1000);
	
	buffer[0] = 0x1A; buffer[1] = 0x03;
	HAL_I2C_Master_Transmit(imu_handle, IMU_address, buffer, sizeof(buffer), 1000);
	
	return 0;
}

void calibrate_gyro()
{
	gyro_roll_cal = gyro_pitch_cal = gyro_yaw_cal = 0;
	for (cal_int = 0; cal_int < 2000; cal_int++)
	{
		if (cal_int % 25 == 0) HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);  //Blink
		read_gyro();
		gyro_roll_cal += gyro_roll;
		gyro_pitch_cal += gyro_pitch;
		gyro_yaw_cal += gyro_yaw;
		HAL_Delay(4);
	}
	gyro_roll_cal /= 2000;
	gyro_pitch_cal /= 2000;
	gyro_yaw_cal /= 2000;
}

void calibrate_level()
{
	acc_pitch_cal_value = 160;
	acc_roll_cal_value = -81;
	return;
	
	level_calibration_on = 1;
	acc_pitch_cal_value = acc_roll_cal_value = 0;
	int err = 0;
	for (err = 0; err < 64; err++)
	{
		read_gyro();
		acc_pitch_cal_value += acc_y;
		acc_roll_cal_value += acc_x;
		
		if (acc_y > 500 || acc_y < -500) err = 80; //Check if values are out of range err = 80: break + flag
		if (acc_x > 500 || acc_x < -500) err = 80;
		HAL_Delay(4);
	}
	acc_pitch_cal_value /= 64;
	acc_roll_cal_value /= 64;
	
	//if (err < 80) //Write calibration data to eeprom
	if(err >= 80)
	{
		set_error(CAL_ERR);
	}
	
	level_calibration_on = 0;
	read_gyro();
	acc_total_vector = (int32_t) sqrtf((float)((acc_x * acc_x) + (acc_y*acc_y) + (acc_z*acc_z)));
	
	if (abs(acc_y) < acc_total_vector)
	{
		angle_pitch_acc = asin((float)acc_y / acc_total_vector) * 57.296;
	}
	if (abs(acc_x) < acc_total_vector) {//Prevent the asin function to produce a NaN.	                                             
		angle_roll_acc = asin((float)acc_x / acc_total_vector) * 57.296; //Calculate the roll angle.
	}
	angle_pitch = angle_pitch_acc;//Set the gyro pitch angle equal to the accelerometer pitch angle when the quadcopter is started.
	angle_roll = angle_roll_acc;
	DWT->CYCCNT = 0; //Reset clock
}

int8_t read_gyro()
{
	uint8_t buffer[14];
	buffer[0] = 0x3B;
	
	HAL_I2C_Master_Transmit(imu_handle, IMU_address, buffer, 1, 100);
	
	HAL_StatusTypeDef status = HAL_I2C_Master_Receive(imu_handle, IMU_address, buffer, 14, 5000);
	if (status != HAL_OK)
	{
		uint32_t error =  HAL_I2C_GetError(imu_handle);
		return -1;
	}
	acc_y = buffer[0] << 8 | buffer[1];
	acc_x = buffer[2] << 8 | buffer[3];
	acc_z = buffer[4] << 8 | buffer[5];
	
	temp_raw = (int16_t)buffer[6] << 8 | (int16_t)buffer[7];
	
	gyro_roll =  buffer[8]  << 8 | buffer[9];
	gyro_pitch = buffer[10] << 8 | buffer[11];
	gyro_yaw =   buffer[12] << 8 | buffer[13];
	
	gyro_pitch *= -1;
	gyro_yaw *= -1;
	
	if (level_calibration_on == 0)
	{
		acc_y -= acc_pitch_cal_value;
		acc_x -= acc_roll_cal_value;
	}
	if (cal_int >= 2000)
	{
		gyro_roll -= gyro_roll_cal;
		gyro_pitch -= gyro_pitch_cal;
		gyro_yaw -= gyro_yaw_cal;
	}
	
	temperature = (int16_t)(((float)temp_raw / 340.0f) + 36.53);
	return 0;
}