#ifndef GPS_H
#define GPS_H
#include "stm32f4xx_hal.h"

void init_gps(UART_HandleTypeDef* uart);
void read_gps(UART_HandleTypeDef* uart);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);

extern int64_t latitude;
extern int16_t longitude;
#endif // !GPS_H
