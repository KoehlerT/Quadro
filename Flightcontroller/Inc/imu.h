#ifndef IMU_H
#define IMU_H

#include "stm32f1xx_hal.h"
#define IMU_address 0x68 << 1


extern int16_t gyro_axis[3];
extern int16_t temperature;

extern int8_t init_gyro(I2C_HandleTypeDef* handle);
extern int8_t read_gyro();
#endif