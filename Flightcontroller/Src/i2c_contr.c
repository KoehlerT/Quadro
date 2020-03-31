#include "i2c_contr.h"

uint8_t i2c_is_waiting = 0;

void set_receiving_flag()
{
	i2c_is_waiting = 1;
}
void wait_to_receive()
{
	while (i2c_is_waiting) ;
}
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef* handle)
{
	i2c_is_waiting = 0;
}