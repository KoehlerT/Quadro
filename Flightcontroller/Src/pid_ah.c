#include "pid_ah.h"

//ALTITUDE PID LIMITS
float pid_p_gain_altitude = 1.4;             //Gain setting for the altitude P-controller (default = 1.4).
float pid_i_gain_altitude = 0.2;             //Gain setting for the altitude I-controller (default = 0.2).
float pid_d_gain_altitude = 0.75;            //Gain setting for the altitude D-controller (default = 0.75).
int pid_max_altitude = 400;                  //Maximum output of the PID-controller (+/-).

//Altitude PID variables
float pid_i_mem_altitude, pid_altitude_setpoint, pid_altitude_input, pid_output_altitude, pid_last_altitude_d_error;
uint8_t parachute_rotating_mem_location;
int32_t parachute_buffer[35], parachute_throttle;
float pressure_parachute_previous;
int32_t pressure_rotating_mem[50], pressure_total_avarage;
uint8_t pressure_rotating_mem_location;
float pressure_rotating_mem_actual;
//-error
float pid_error_gain_altitude, pid_throttle_gain_altitude;
float pid_error_temp_alt;

//
uint8_t manual_altitude_change;
int16_t manual_throttle;

void reset_pid_altitude()
{
	pid_altitude_setpoint = 0;                                                        //Reset the PID altitude setpoint.
    pid_output_altitude = 0;                                                          //Reset the output of the PID controller.
    pid_i_mem_altitude = 0;                                                           //Reset the I-controller.
    manual_throttle = 0;                                                              //Set the manual_throttle variable to 0 .
    manual_altitude_change = 1;                                                       //Set the manual_altitude_change to 1.
}
void calculate_pid_altitude()
{
	//In the following part a rotating buffer is used to calculate the long term change between the various pressure measurements.
	//This total value can be used to detect the direction (up/down) and speed of the quadcopter and functions as the D-controller of the total PID-controller.
	if(manual_altitude_change == 1)pressure_parachute_previous = actual_pressure * 10;                          //During manual altitude change the up/down detection is disabled.
	parachute_throttle -= parachute_buffer[parachute_rotating_mem_location];                                     //Subtract the current memory position to make room for the new value.
	parachute_buffer[parachute_rotating_mem_location] = actual_pressure * 10 - pressure_parachute_previous;      //Calculate the new change between the actual pressure and the previous measurement.
	parachute_throttle += parachute_buffer[parachute_rotating_mem_location];                                     //Add the new value to the long term avarage value.
	pressure_parachute_previous = actual_pressure * 10;                                                          //Store the current measurement for the next loop.
	parachute_rotating_mem_location++;                                                                           //Increase the rotating memory location.
	if(parachute_rotating_mem_location == 30)parachute_rotating_mem_location = 0;                               //Start at 0 when the memory location 20 is reached.

	if(mode >= ALTITUDE && state == FLIGHT) {
		//If the quadcopter is in altitude mode and flying.
		if(pid_altitude_setpoint == 0)pid_altitude_setpoint = actual_pressure;                                    //If not yet set, set the PID altitude setpoint.
		//When the throttle stick position is increased or decreased the altitude hold function is partially disabled. The manual_altitude_change variable
		//will indicate if the altitude of the quadcopter is changed by the pilot.
		manual_altitude_change = 0;                                                       //Preset the manual_altitude_change variable to 0.
		manual_throttle = 0;                                                              //Set the manual_throttle variable to 0.
		if(channel[2] > 1600) {
			//If the throtttle is increased above 1600us (60%).
			manual_altitude_change = 1;                                                     //Set the manual_altitude_change variable to 1 to indicate that the altitude is adjusted.
			pid_altitude_setpoint = actual_pressure;                                        //Adjust the setpoint to the actual pressure value so the output of the P- and I-controller are 0.
			manual_throttle = (channel[2] - 1600) / 3;                                       //To prevent very fast changes in hight limit the function of the throttle.
		}
		if (channel[2] < 1400) {
				//If the throtttle is lowered below 1400us (40%).
			manual_altitude_change = 1;                                                     //Set the manual_altitude_change variable to 1 to indicate that the altitude is adjusted.
			pid_altitude_setpoint = actual_pressure;                                        //Adjust the setpoint to the actual pressure value so the output of the P- and I-controller are 0.
			manual_throttle = (channel[2] - 1400) / 5;                                       //To prevent very fast changes in hight limit the function of the throttle.
		}

		//Calculate the PID output of the altitude hold.
		pid_altitude_input = actual_pressure;                                             //Set the setpoint (pid_altitude_input) of the PID-controller.
		pid_error_temp_alt = pid_altitude_input - pid_altitude_setpoint;                      //Calculate the error between the setpoint and the actual pressure value.

		//To get better results the P-gain is increased when the error between the setpoint and the actual pressure value increases.
		//The variable pid_error_gain_altitude will be used to adjust the P-gain of the PID-controller.
		pid_error_gain_altitude = 0;                                                      //Set the pid_error_gain_altitude to 0.
		if(pid_error_temp_alt > 10 || pid_error_temp_alt < -10) {
			//If the error between the setpoint and the actual pressure is larger than 10 or smaller then -10.
			pid_error_gain_altitude = (abs(pid_error_temp_alt) - 10) / 20.0;                    //The positive pid_error_gain_altitude variable is calculated based based on the error.
			if(pid_error_gain_altitude > 3)pid_error_gain_altitude = 3;                    //To prevent extreme P-gains it must be limited to 3.
		}

		//In the following section the I-output is calculated. It's an accumulation of errors over time.
		//The time factor is removed as the program loop runs at 250Hz.
		pid_i_mem_altitude += (pid_i_gain_altitude / 100.0) * pid_error_temp_alt;
		if (pid_i_mem_altitude > pid_max_altitude)pid_i_mem_altitude = pid_max_altitude;
		else if (pid_i_mem_altitude < pid_max_altitude * -1)pid_i_mem_altitude = pid_max_altitude * -1;
		//In the following line the PID-output is calculated.
		//P = (pid_p_gain_altitude + pid_error_gain_altitude) * pid_error_temp_alt.
		//I = pid_i_mem_altitude += (pid_i_gain_altitude / 100.0) * pid_error_temp_alt (see above).
		//D = pid_d_gain_altitude * parachute_throttle.
		pid_output_altitude = (pid_p_gain_altitude + pid_error_gain_altitude) * pid_error_temp_alt + pid_i_mem_altitude + pid_d_gain_altitude * parachute_throttle;
		//To prevent extreme PID-output the output must be limited.
		if(pid_output_altitude > pid_max_altitude)pid_output_altitude = pid_max_altitude;
		else if(pid_output_altitude < pid_max_altitude * -1)pid_output_altitude = pid_max_altitude * -1;
	}
}