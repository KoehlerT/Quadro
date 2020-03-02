#include "statemachine.h"

uint8_t mode = IDLE;

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
}