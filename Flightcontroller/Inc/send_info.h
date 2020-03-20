#ifndef SEND_INFO_H
#define SEND_INFO_H
#include "stm32f4xx_hal.h"

#include "statemachine.h"
#include "imu.h"
#include "receiver.h"
#include "motors.h"
//#include "adc.h"
#include "baro.h"

void init_info(UART_HandleTypeDef*);
void USART1_IRQHandler(void);
void send_info(UART_HandleTypeDef*);

#endif // SEND_INFO_H

