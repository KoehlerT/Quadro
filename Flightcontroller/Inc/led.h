#ifndef LED_H
#define LED_H
#include "stm32f1xx_hal.h"

void init_led(GPIO_TypeDef* gpio);
void led_signal();

#endif // LED_H

