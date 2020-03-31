#ifndef i2c_contr_h
#define i2c_contr_h
#include "stm32f4xx_hal.h"

void set_receiving_flag();
void wait_to_receive();
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef* handle);

#endif