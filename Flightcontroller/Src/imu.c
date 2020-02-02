#include "imu.h"

int16_t gyro_axis[3];
int16_t temperature;
int16_t temp_raw;
I2C_HandleTypeDef* imu_handle;

int8_t init_gyro(I2C_HandleTypeDef* handle)
{
	imu_handle = handle;
	//Check if device is ready
	HAL_StatusTypeDef state = HAL_I2C_IsDeviceReady(handle, IMU_address, 2, 2);
	if (state != HAL_OK)
	{
		return -1; //Device not ready
	}
	
	//Set Up Mup6050
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

int8_t read_gyro()
{
	uint8_t buffer[14];
	buffer[0] = 0x3B;
	
	HAL_I2C_Master_Transmit(imu_handle, IMU_address, buffer, 1, 1000);
	
	HAL_StatusTypeDef status = HAL_I2C_Master_Receive(imu_handle, IMU_address, buffer, 14, 1000);
	if (status != HAL_OK)
	{
		return -1;
	}
	
	temp_raw = (int16_t)buffer[6] << 8 | (int16_t)buffer[7];
	temperature = (int16_t)(((float)temp_raw / 340.0f) + 36.53);
	return 0;
}