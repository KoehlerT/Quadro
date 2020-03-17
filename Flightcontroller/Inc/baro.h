#ifndef BARO_H
#define BARO_H
#include "stm32f1xx_hal.h"

void init_baro(I2C_HandleTypeDef* i2cHandle);
void read_baro();

extern float actual_pressure;
extern float altitude_meters;
extern float baro_temp;

#endif