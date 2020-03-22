#include "led.h"

uint16_t cyclecount = 0;
int8_t yPulseCount, gPulseCount, yPulses, gPulses;

#define GPIO_YELLOW GPIOB, GPIO_PIN_12
#define GPIO_GREEN GPIOB, GPIO_PIN_13
#define GPIO_BLUE GPIOC, GPIO_PIN_13

//Pins: PB13 Green    PB12 Yellow     PC13 Onboard, blue

void init_led()
{
	//Turn off leds
	HAL_GPIO_WritePin(GPIO_GREEN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIO_YELLOW, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIO_BLUE, GPIO_PIN_RESET);
}

void setPulsesGreen(int8_t green)
{
	gPulses = green;
}
void setPulsesYellow(int8_t yellow)
{
	yPulses = yellow;
}
void led_signal()
{
	//Blink every 100 cycles, after blinks complete, wait 300 cycles
	cyclecount++;
	if (cyclecount % 100 == 0) //turn on leds, which should blink
	{
		yPulseCount++;
		if (yPulseCount <= yPulses)
			HAL_GPIO_WritePin(GPIO_YELLOW, GPIO_PIN_SET);
		if (yPulseCount > yPulses + 2) //Wait 2 intervals without blinking
			yPulseCount = 0;
		gPulseCount++;
		if (gPulseCount <= gPulses)
			HAL_GPIO_WritePin(GPIO_GREEN, GPIO_PIN_SET);
		if (gPulseCount > gPulses + 2)
			gPulseCount = 0;
		
	}
	if (cyclecount % 100 == 50) //turn off all leds
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	}
}
void indicateActivity()
{
	HAL_GPIO_TogglePin(GPIO_BLUE);
}