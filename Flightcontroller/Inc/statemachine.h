#ifndef STATEMACHINE_H
#define STATEMACHINE_H
#include "main.h"
#include "receiver.h"
#include "pid.h"

enum FLIGHTMODE;
typedef enum {
	IDLE   = 0x00,
	READY  = 0x01,
	MANUAL = 0x02,
	SETUP = 0x11,
	CALIBRATE = 0x12
}FLIGHTMODE;

typedef enum
{
	NO_ERR = 0x00,
	LOW_BATTERY = 0x01,
	LOOPTIME_ERR = 0x02,
	CAL_ERR = 0x11
}CONTR_ERR;

extern uint8_t mode;
extern uint8_t error;
extern uint8_t hardwareFaultRegister; // I2C Timeout, IMU Err, Baro Err, Comp Err, Receiver Err, Transmit Err,0,0

void init_state();
void change_state();
void set_error(CONTR_ERR error);
void signal_state();

#endif