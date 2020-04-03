#include "imu.h"

//#define cal_level //Uncomment for calibration of gyro level values. Hardcode them afterwards and comment
#define CheckI2C(x) if (x != HAL_OK) set_error(IMU_READ_ERR);

//Public gyroscope variables
int16_t gyro_axis[3];
int16_t acc_x, acc_y, acc_z;
int16_t gyro_pitch, gyro_roll, gyro_yaw;

//Public accelerometer variables
float angle_pitch, angle_roll, angle_yaw; 
int32_t acc_total_vector, acc_total_vector_at_start;
float angle_roll_acc, angle_pitch_acc;

int32_t gyro_roll_cal, gyro_pitch_cal, gyro_yaw_cal;
int16_t acc_pitch_cal_value, acc_roll_cal_value;

//Public temperature variables
int16_t temperature;

//Helper vars
int16_t temp_raw;
uint8_t imu_buffer[14];
//- Calibration vars
int16_t cal_int; //counts calibration readings
uint8_t level_calibration_on;


I2C_HandleTypeDef* imu_handle;

int8_t init_gyro(I2C_HandleTypeDef* handle)
{
	//HAL_I2C_EnableListen_IT(handle);
	imu_handle = handle;
	
	//Check if device is ready
	HAL_StatusTypeDef state = HAL_I2C_IsDeviceReady(imu_handle, IMU_address, 2, 10);
	if (state != HAL_OK)
	{
		set_error(I2C_NOT_READY);
		return -1; //Device not ready
	}
	
	//Set Up Mup6050
	
	/*uint8_t buff[] = { 0x6B, 0x00, 0x1B, 0x08, 0x1C, 0x10, 0x1A, 0x03 };
	HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(imu_handle, IMU_address, buff, 8, 1000);
	if (st != HAL_OK)
		return -1;
	
	return 0;*/
	//Alternative
	imu_buffer[0] =  0x6B; imu_buffer[1] = 0x00;
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(imu_handle, IMU_address, imu_buffer, 2, 1000);
	if (status != HAL_OK)
		return -1;
	
	imu_buffer[0] = 0x1B; imu_buffer[1] = 0x08;
	HAL_I2C_Master_Transmit(imu_handle, IMU_address, imu_buffer, 2, 1000);
	
	imu_buffer[0] = 0x1C; imu_buffer[1] = 0x10;
	HAL_I2C_Master_Transmit(imu_handle, IMU_address, imu_buffer, 2, 1000);
	
	imu_buffer[0] = 0x1A; imu_buffer[1] = 0x03;
	HAL_I2C_Master_Transmit(imu_handle, IMU_address, imu_buffer, 2, 1000);
	
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
#ifndef cal_level
	acc_pitch_cal_value = 140;
	acc_roll_cal_value = -138;
	return;
#else
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
		set_error(CALIBRATION_ERR);
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
#endif

}
int8_t read_gyro()
{
	for (int i = 0; i < 14; i++)imu_buffer[i] = 0; //clear buffer
	imu_buffer[0] = 0x3B;
	/*set_receiving_flag();
	CheckI2C(HAL_I2C_Mem_Read_IT(imu_handle, IMU_address, 0x3B, I2C_MEMADD_SIZE_8BIT, imu_buffer, 14));
	wait_to_receive();*/
	
	set_sending_flag();
	CheckI2C(HAL_I2C_Master_Transmit_IT(imu_handle, IMU_address, imu_buffer, 1));
	wait_to_send();
	
	set_receiving_flag(); //set flag
	HAL_StatusTypeDef status =  HAL_I2C_Master_Receive_IT(imu_handle, IMU_address, imu_buffer, 14); //receive buffer
	//HAL_StatusTypeDef status = HAL_I2C_Master_Receive(imu_handle, IMU_address, imu_buffer, 14, 500);
	if (status != HAL_OK)
	{
		uint32_t error =  HAL_I2C_GetError(imu_handle);
		set_error(IMU_READ_ERR);
		return -1;
	}
	wait_to_receive(); //wait for buffer to receive*/
	//Decode buffer
	acc_y = imu_buffer[0] << 8 | imu_buffer[1];
	acc_x = imu_buffer[2] << 8 | imu_buffer[3];
	acc_z = imu_buffer[4] << 8 | imu_buffer[5];
	
	temp_raw = (int16_t)imu_buffer[6] << 8 | (int16_t)imu_buffer[7];
	
	gyro_roll =  imu_buffer[8]  << 8 | imu_buffer[9];
	gyro_pitch = imu_buffer[10] << 8 | imu_buffer[11];
	gyro_yaw =   imu_buffer[12] << 8 | imu_buffer[13];
	
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