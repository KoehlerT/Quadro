#include "led.h"

uint16_t cyclecount = 0;
int8_t yPulse, gPulse;

//Pins: PB13 Green    PB12 Yellow     PC13 Onboard, green

void init_led()
{
	//Turn off leds
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}
void led_signal(int8_t gPulses, int8_t yPulses)
{
	cyclecount++;
	if (cyclecount % 100 == 0)
	{
		if (yPulse < yPulses)
		{
			yPulse++;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,GPIO_PIN_SET);
		}
		if (gPulse < gPulses)
		{
			gPulse++;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		}
	}
	else if (cyclecount % 100 == 50)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	}
	if (cyclecount >= 500) {
		//Reset after 4 seconds...
		yPulse = gPulse = 0;
		cyclecount = 0;
	}
}