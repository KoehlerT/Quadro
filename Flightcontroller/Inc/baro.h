#ifndef BARO_H
#define BARO_H
#include "stm32f4xx_hal.h"
#include "statemachine.h"

void init_baro(I2C_HandleTypeDef* i2cHandle);
void read_baro();

extern float actual_pressure;
extern float altitude_meters;
extern float baro_temp;

#endif