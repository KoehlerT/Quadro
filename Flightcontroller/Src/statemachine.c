#include "statemachine.h"
#include "led.h"

FLIGHTMODE mode = IDLE;
QUADSTATE state = STOP;

// I2C Timeout, IMU Err, Baro Err, Comp Err, Receiver Err, Transmitter Err, Calibration Error, Looptime exceed, Battery low
uint16_t faultRegister; //first 8 bits: peripheral faults, last 8 bits other

void init_state()
{
	mode = IDLE;
	state = STOP;
	faultRegister = NO_ERR;
	setPulsesGreen(1);
	init_led();
}
void set_state(QUADSTATE st)
{
	state = st;
	
}
void change_state()
{
	
	//For starting the motors: throttle low and yaw left (step 1).
	if (channel[2] < 1050 && channel[3] < 1050 && channel[3] > 900) state = ARM;
	
	//When yaw stick is back in the center position start the motors (step 2).
	if (state == ARM && channel[2] < 1050 && channel[3] > 1450)
	{
		mode = MANUAL;
		state = FLIGHT;
		reset_controllers();
		reset_ground_pressure();
		setPulsesGreen(2);
	}
	//Channel 5: change mode. 2000us: full autonomous (SPI Control), 1666us: manual, 1500: altitude hold, 1916: gps hold
	//Switching flight modes
	if(state == FLIGHT) //Only change flight modes while flying
	{
		if (mode != AUTONOMOuS && channel[5] > 1960 && channel[5] < 2050) {mode = AUTONOMOuS; setPulsesGreen(mode + 1); }
		if (mode != MANUAL && channel[5] > 1600 && channel[5] < 1700) {mode = MANUAL; setPulsesGreen(mode + 1); }
		if (mode != ALTITUDE && channel[5] > 1450 && channel[5] < 1550) {mode = ALTITUDE; setPulsesGreen(mode + 1); reset_pid_altitude(); }
		if (mode != GPS && channel[5] > 1890 && channel[5] < 1960) {mode = GPS; setPulsesGreen(mode + 1); }
	}
	
	
	//Stopping the motors: throttle low and yaw right.
	if(state == FLIGHT && channel[2] < 1050 && channel[3] > 1950) {
		mode = IDLE;
		state = STOP;
		setPulsesGreen(1);
	}
	//Calibrating Motors, only if on ground
	if (mode == IDLE && channel[0] > 1950 && channel[1] > 1950)
	{
		if (state == STOP)
		{
			mode = IDLE;
			state = TEST;
			setPulsesGreen(5);
		}
		else if (state == TEST)
		{
			state = STOP;
			setPulsesGreen(1);
		}
	}
	
}


void set_error(CONTR_ERR err)
{
	faultRegister = faultRegister | (1 << err);
	
	//Peripheral error
	if(faultRegister & 0x00FF) //Any of the first bits are set => Peripheral fault
	{
		setPulsesYellow(5);
	}else if(faultRegister & (1 << BATTERY_LOW))
		setPulsesYellow(4);
	else if(faultRegister & (1 << LOOPTIME_EXCEED))
		setPulsesYellow(3);
}

void signal_state()
{
	led_signal();
}