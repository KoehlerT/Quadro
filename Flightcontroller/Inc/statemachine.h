#ifndef STATEMACHINE_H
#define STATEMACHINE_H
#include "main.h"
#include "receiver.h"
#include "pid.h"
#include "pid_ah.h"
#include "baro.h"

enum FLIGHTMODE;
typedef enum {
	IDLE = 0,
	MANUAL = 1,
	ALTITUDE = 2,
	GPS = 3,
	AUTONOMOuS = 4
}FLIGHTMODE;

enum QUADSTATE;
typedef enum
{
	STOP = 0,
	ARM = 1,
	TAKEOFF = 2,
	FLIGHT = 3,
	INIT = 4,
	TEST = 5
}QUADSTATE;

typedef enum
{
	NO_ERR = 0,
	I2C_NOT_READY = 1,
	IMU_READ_ERR = 2,
	BARO_READ_ERR = 3,
	COMPASS_READ_ERR = 4,
	RECEIVER_ERR = 5,
	TRANSMIT_ERR = 6,
	I2C_TIMEOUT_ERR = 7,
	CALIBRATION_ERR = 8,
	LOOPTIME_EXCEED = 9,
	BATTERY_LOW = 10,
}CONTR_ERR;

extern FLIGHTMODE mode;
extern QUADSTATE state;
extern uint16_t faultRegister;
extern int16_t looptime;
// I2C Timeout, IMU Err, Baro Err, Comp Err, Receiver Err, Transmitter Err, Calibration Error, Looptime exceed, Battery low

void init_state();
void change_state();
void set_error(CONTR_ERR error);
void signal_state();
void set_state(QUADSTATE st);
#endif