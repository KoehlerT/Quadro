#ifndef i2c_contr_h
#define i2c_contr_h
#include "stm32f4xx_hal.h"
#include "statemachine.h"

void set_receiving_flag();
void wait_to_receive();
void set_sending_flag();
void wait_to_send();
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef* handle);
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef* handle);

#endif