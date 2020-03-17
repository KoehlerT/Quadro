#ifndef PID_H
#define PID_H

#include "stm32f4xx_hal.h"
#include "imu.h"
#include "receiver.h"

//extern int16_t esc_1, esc_2, esc_3, esc_4;
extern float pid_output_pitch, pid_output_roll, pid_output_yaw;
extern void calculate_pid();

extern void reset_controllers();

#endif // !PID_H
