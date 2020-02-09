#ifndef PID_H
#define PID_H

#include "stm32f1xx_hal.h"
#include "imu.h"
#include "receiver.h"

extern int16_t esc_1, esc_2, esc_3, esc_4;
extern void calculate_pid();

#endif // !PID_H
