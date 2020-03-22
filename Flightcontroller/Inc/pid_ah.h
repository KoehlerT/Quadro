#ifndef PID_AH_H
#define PID_AH_H
#include "stm32f4xx_hal.h"
#include "statemachine.h"
#include "receiver.h"
#include "baro.h"

void reset_pid_altitude();
void calculate_pid_altitude();
extern float pid_output_altitude;
extern int16_t manual_throttle;

#endif // !PID_AH_H
