#include "led.h"

uint16_t cyclecount = 0;

//Pins: PB12: Green PB13: Yellow PC13: Onboard, green

void init_led()
{
	//Turn off leds
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
}
void led_signal()
{
	cyclecount++;
	if (cyclecount % 100 == 0) //Blink onboard led
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); 
	if (cyclecount % 50 == 0)
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
	if (cyclecount % 200 == 0)
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
	if (cyclecount >= 1000) //Reset after 4 seconds...
		cyclecount = 0;
}