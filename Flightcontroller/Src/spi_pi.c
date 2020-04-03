#include "spi_pi.h"
#include "statemachine.h"
#include "imu.h"

#define SPI_BUFFERLENGTH 200

uint8_t spi_rxbuf[SPI_BUFFERLENGTH];
uint8_t spi_txbuf[SPI_BUFFERLENGTH];
void writeToBuffer(void* dest, int offDest, void* source, int offSource, int length);
//void readFromBuffer(void* dest, int offDest, void* source, int offSource, int length);

void init_spi(SPI_HandleTypeDef* handle)
{
	for (int i = 0; i < SPI_BUFFERLENGTH; i++)
		spi_txbuf[i] = i;
	HAL_SPI_TransmitReceive_DMA(handle, spi_txbuf, spi_rxbuf, SPI_BUFFERLENGTH);
}
void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef* handle)
{
	//read first halfe
	//set first halfe
	
	writeToBuffer(spi_txbuf, 0, "inf1", 0, 4); //start seq
	spi_txbuf[4] = 0x01;					//state id
	spi_txbuf[5] = mode;
	spi_txbuf[6] = state;
	writeToBuffer(spi_txbuf, 7, &faultRegister, 0, 2);
	writeToBuffer(spi_txbuf, 9, &looptime, 0, 2);
	spi_txbuf[11] = 0x02; //gyro id
	writeToBuffer(spi_txbuf, 12, gyro_axis, 0, 3 * 2);
	writeToBuffer(spi_txbuf, 18, &acc_x, 0, 2);
	writeToBuffer(spi_txbuf, 20, &acc_y, 0, 2);
	writeToBuffer(spi_txbuf, 22, &acc_z, 0, 2);
	writeToBuffer(spi_txbuf, 24, &gyro_pitch, 0, 2);
	writeToBuffer(spi_txbuf, 26, &gyro_roll, 0, 2);
	writeToBuffer(spi_txbuf, 28, &gyro_yaw, 0, 2);
	writeToBuffer(spi_txbuf, 30, &angle_pitch, 0, 4);
	writeToBuffer(spi_txbuf, 34, &angle_roll, 0, 4);
	writeToBuffer(spi_txbuf, 38, &angle_yaw, 0, 4);
	writeToBuffer(spi_txbuf, 42, &acc_total_vector, 0, 4);
	writeToBuffer(spi_txbuf, 46, &acc_total_vector_at_start, 0, 4);
	writeToBuffer(spi_txbuf, 50, &angle_roll_acc, 0, 4);
	writeToBuffer(spi_txbuf, 54, &angle_pitch_acc, 0, 4);
	writeToBuffer(spi_txbuf, 58, &temperature, 0, 2);
	writeToBuffer(spi_txbuf, 60, &gyro_roll_cal, 0, 2);
	writeToBuffer(spi_txbuf, 62, &gyro_pitch, 0, 2);
	writeToBuffer(spi_txbuf, 64, &gyro_yaw_cal, 0, 2);
	writeToBuffer(spi_txbuf, 66, &acc_roll_cal_value, 0, 2);
	writeToBuffer(spi_txbuf, 68, &acc_pitch_cal_value, 0, 2);
	spi_txbuf[70] = 0x3; //baro id*/
	
}
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* handle)
{
	//read second halfe
	//set second halfe
	
}

void writeToBuffer(void* dest, int offDest, void* source, int offSource, int length)
{
	for (int i = 0; i < length; i++)
		((uint8_t *)dest)[offDest + i] = ((uint8_t *)source)[offSource + i];
}