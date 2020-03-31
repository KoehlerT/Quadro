#ifndef spi_pi_h
#define spi_pi_h
#include "stm32f4xx_hal.h"

void init_spi(SPI_HandleTypeDef* handle);
void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef* handle);
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* handle);

#endif
