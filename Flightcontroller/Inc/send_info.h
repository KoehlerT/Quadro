#ifndef SEND_INFO_H
#define SEND_INFO_H
#include "stm32f4xx_hal.h"

#include "statemachine.h"
#include "imu.h"
#include "receiver.h"
#include "motors.h"
#include "adc.h"
#include "baro.h"

void init_info(UART_HandleTypeDef*);
void send_info(UART_HandleTypeDef*);

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);

#endif // SEND_INFO_H

