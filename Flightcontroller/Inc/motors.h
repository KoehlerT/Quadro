#ifndef MOTORS_H
#define MOTORS_H

#include "stm32f1xx_hal.h"
#include "statemachine.h"
#include "pid.h"

extern int16_t esc_1, esc_2, esc_3, esc_4;

extern void init_motors(TIM_HandleTypeDef*);
extern void set_motors(int16_t);

#endif // !MOTORS_H
