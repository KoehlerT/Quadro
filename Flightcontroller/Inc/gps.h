#ifndef GPS_H
#define GPS_H
#include "stm32f4xx_hal.h"

void init_gps(UART_HandleTypeDef* uart);
void read_gps(UART_HandleTypeDef* uart);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);

extern int8_t num_sats;
extern int8_t fix_type;
extern int64_t longitude;
extern int64_t latitude;
#endif // !GPS_H
