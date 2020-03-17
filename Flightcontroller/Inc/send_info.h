#ifndef SEND_INFO_H
#define SEND_INFO_H
#include "stm32f1xx_hal.h"

#include "statemachine.h"
#include "imu.h"
#include "receiver.h"
#include "motors.h"
#include "adc.h"
#include "baro.h"

void init_info(UART_HandleTypeDef* uart);

void send_info();

#endif // SEND_INFO_H

