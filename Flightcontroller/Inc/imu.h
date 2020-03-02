#ifndef IMU_H
#define IMU_H

#include "stm32f1xx_hal.h"
#define IMU_address 0x68 << 1


extern int16_t gyro_axis[3];
extern int16_t acc_x, acc_y, acc_z;
extern int16_t gyro_pitch, gyro_roll, gyro_yaw;
extern float angle_pitch, angle_roll, angle_yaw; 
extern int32_t acc_total_vector, acc_total_vector_at_start;
extern float angle_roll_acc, angle_pitch_acc;

extern int16_t temperature;

extern int8_t init_gyro(I2C_HandleTypeDef* handle);
extern int8_t read_gyro();
extern void calibrate_gyro();
extern void calibrate_level();
#endif