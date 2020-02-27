#include "led.h"

GPIO_TypeDef* _gpio;
uint16_t cyclecount = 0;

void init_led(GPIO_TypeDef* gpio)
{
	_gpio = gpio;
}
void led_signal()
{
	cyclecount++;
	if (cyclecount % 500 == 0) //Blink
		HAL_GPIO_TogglePin(_gpio, 13);
	//HAL_GPIO_WritePin(_gpio, 13, GPIO_PIN_SET);
	if (cyclecount >= 1000) //Reset after 4 seconds...
		cyclecount = 0;
}