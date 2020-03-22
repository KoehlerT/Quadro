#ifndef LED_H
#define LED_H
#include "stm32f4xx_hal.h"

void init_led();
void setPulsesGreen(int8_t green);
void setPulsesYellow(int8_t yellow);
void setPulsesBlue(int8_t blue);
void led_signal();
void indicateActivity();

#endif // LED_H

