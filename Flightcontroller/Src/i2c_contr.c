#include "i2c_contr.h"

uint8_t i2c_receive_is_waiting = 0;
uint8_t i2c_sending_is_waiting = 0;
uint64_t start_time;
//timeout in clock cycles
#define I2C_TIMEOUT 100000
void set_receiving_flag()
{
	start_time = DWT->CYCCNT;
	i2c_receive_is_waiting = 1;
}
void wait_to_receive()
{
	while (i2c_receive_is_waiting && DWT->CYCCNT - start_time < I2C_TIMEOUT) ;
	if (DWT->CYCCNT - start_time >= I2C_TIMEOUT)
	{
		set_error(I2C_TIMEOUT_ERR);
		i2c_receive_is_waiting = 0;
	}
}
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef* handle)
{
	i2c_receive_is_waiting = 0;
}
void set_sending_flag()
{
	start_time = DWT->CYCCNT;
	i2c_sending_is_waiting = 1;
}
void wait_to_send()
{
	while (i2c_sending_is_waiting&& DWT->CYCCNT - start_time < I2C_TIMEOUT) ;
	if (DWT->CYCCNT - start_time >= I2C_TIMEOUT)
	{
		set_error(I2C_TIMEOUT_ERR);
		i2c_sending_is_waiting = 0;
	}
}
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef* handle)
{
	i2c_sending_is_waiting = 0;
}