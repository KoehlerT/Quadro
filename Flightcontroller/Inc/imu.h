#ifndef IMU_H
#define IMU_H

#include "stm32f4xx_hal.h"
#include "statemachine.h"
#include "i2c_contr.h"
#define IMU_address 0x68 << 1


extern int16_t gyro_axis[3];
extern int16_t acc_x, acc_y, acc_z;
extern int16_t gyro_pitch, gyro_roll, gyro_yaw;
extern float angle_pitch, angle_roll, angle_yaw; 
extern int32_t acc_total_vector, acc_total_vector_at_start;
extern float angle_roll_acc, angle_pitch_acc;

extern int32_t gyro_roll_cal, gyro_pitch_cal, gyro_yaw_cal;
extern int16_t acc_pitch_cal_value, acc_roll_cal_value;

extern int16_t temperature;

int8_t init_gyro(I2C_HandleTypeDef* handle);
int8_t read_gyro();
void calibrate_gyro();
void calibrate_level();

#endif