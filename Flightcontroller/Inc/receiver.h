#ifndef RECEIVER_H
#define RECEIVER_H
#include "stm32f4xx_hal.h"

extern void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim);
extern void init_receiver(TIM_HandleTypeDef* htim);
extern uint16_t channel[6];

#endif // !RECEIVER_H
