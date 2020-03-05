#include "statemachine.h"
#include "led.h"

uint8_t mode = IDLE;
uint8_t error = NO_ERR;

uint8_t gPulses, yPulses;

void init_state()
{
	init_led();
}

void change_state()
{
	//For starting the motors: throttle low and yaw left (step 1).
	if (channel[2] < 1050 && channel[3] < 1050 && channel[3] > 900) mode = READY;
	
	//When yaw stick is back in the center position start the motors (step 2).
	if (mode == READY && channel[2] < 1050 && channel[3] > 1450)
	{
		mode = MANUAL;
		reset_controllers();
	}
	
	//Stopping the motors: throttle low and yaw right.
	if(mode == MANUAL && channel[2] < 1050 && channel[3] > 1950) {
		mode = IDLE;
	}
	if (mode == IDLE && channel[0] > 1950 && channel[1] > 1950)
	{
		mode = SETUP;
	}
	
	switch (mode)
	{
	case IDLE: gPulses = 1; break;
	case MANUAL: gPulses = 2; break;
	case SETUP: gPulses = 5; break;
	case CALIBRATE: yPulses= 100; break;
	default:
		gPulses = 0;
	}
}


void set_error(CONTR_ERR err)
{
	if (err > error)
		error = err;
	//GET LED
	switch (error)
	{
	case LOW_BATTERY: yPulses = 1; break;
	case LOOPTIME_ERR: yPulses = 2; break;
	case CAL_ERR: yPulses = 3; break;
	default:
		yPulses = 0;
	}
}

void signal_state()
{
	led_signal(gPulses, yPulses);
}