#ifndef LED_H
#define LED_H
#include "stm32f4xx_hal.h"

void init_led();
void led_signal(int8_t gPulses, int8_t yPulses);

#endif // LED_H

