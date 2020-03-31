#include "spi_pi.h"

#define SPI_BUFFERLENGTH 100

uint8_t spi_rxbuf[SPI_BUFFERLENGTH];
uint8_t spi_txbuf[SPI_BUFFERLENGTH];

void init_spi(SPI_HandleTypeDef* handle)
{
	for (int i = 0; i < SPI_BUFFERLENGTH; i++)
		spi_txbuf[i] = i;
	HAL_SPI_TransmitReceive_DMA(handle, spi_txbuf, spi_rxbuf, SPI_BUFFERLENGTH);
}
void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef* handle)
{
	
}
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* handle)
{
	
}