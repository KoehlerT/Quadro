#include "i2c_scanner.h"

int8_t i2c_scanner(I2C_HandleTypeDef* spiHandle)
{
	uint8_t data, error, address, done;
	uint16_t nDevices;
	nDevices = data = 0;
	for (address = 1; address < 127; address++)
	{
		/*printf("Checking address: ");
		printf("%#02x",(int)address);*/
		//HAL_StatusTypeDef state = HAL_I2C_Master_Transmit(spiHandle, address << 1, &data, 1, 100);
		HAL_StatusTypeDef state = HAL_I2C_IsDeviceReady(spiHandle, address << 1, 2, 2);
		if (state == HAL_OK)
		{
			
			nDevices++;
		}
		if (state == HAL_ERROR)
		{
		}
		if (state == HAL_TIMEOUT)
		{
		}
		if (state == HAL_BUSY)
		{
			//DBG_WRITE(" Busy\n\r");
		}
	}
	//char msg[20];
	return nDevices;
	
}