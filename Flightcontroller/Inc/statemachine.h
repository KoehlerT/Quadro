#ifndef STATEMACHINE_H
#define STATEMACHINE_H
#include "main.h"
#include "receiver.h"
#include "pid.h"

enum FLIGHTMODE;
typedef enum {
	IDLE   = 0,
	READY  = 1,
	MANUAL = 2
}FLIGHTMODE;

extern uint8_t mode;
extern void change_state();

#endif