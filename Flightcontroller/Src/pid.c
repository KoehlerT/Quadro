#include "pid.h"
#include "pid_limits.h"

//PID Variables
float roll_level_adjust, pitch_level_adjust;
float pid_error_temp;
float pid_i_mem_roll, pid_roll_setpoint, gyro_roll_input, pid_output_roll, pid_last_roll_d_error;
float pid_i_mem_pitch, pid_pitch_setpoint, gyro_pitch_input, pid_output_pitch, pid_last_pitch_d_error;
float pid_i_mem_yaw, pid_yaw_setpoint, gyro_yaw_input, pid_output_yaw, pid_last_yaw_d_error;

uint32_t pid_pitch_setpoint_base, pid_roll_setpoint_base; 

//Vertical Acceleration variables
void vertical_acceleration_calculations();
int32_t acc_z_average_short_total, acc_z_average_long_total, acc_z_average_total;
int16_t acc_z_average_short[26], acc_z_average_long[51];

uint8_t acc_z_average_short_rotating_mem_location, acc_z_average_long_rotating_mem_location;

int32_t acc_alt_integrated;

void calculate_correction();

void calculate_pid()
{
	//65.5 = 1 deg/sec (check the datasheet of the MPU-6050 for more information).
	gyro_roll_input = (gyro_roll_input * 0.7) + (((float)gyro_roll / 65.5) * 0.3);    //Gyro pid input is deg/sec.
	gyro_pitch_input = (gyro_pitch_input * 0.7) + (((float)gyro_pitch / 65.5) * 0.3); //Gyro pid input is deg/sec.
	gyro_yaw_input = (gyro_yaw_input * 0.7) + (((float)gyro_yaw / 65.5) * 0.3);       //Gyro pid input is deg/sec.
	
	//Gyro angle calculations
	//0.0000611 = 1 / (250Hz / 65.5)
	angle_pitch += (float)gyro_pitch * 0.0000611;                                     //Calculate the traveled pitch angle and add this to the angle_pitch variable.
	angle_roll += (float)gyro_roll * 0.0000611;                                       //Calculate the traveled roll angle and add this to the angle_roll variable.
	angle_yaw += (float)gyro_yaw * 0.0000611;                                         //Calculate the traveled yaw angle and add this to the angle_yaw variable.
	if(angle_yaw < 0) angle_yaw += 360;                                              //If the compass heading becomes smaller then 0, 360 is added to keep it in the 0 till 360 degrees range.
	else if(angle_yaw >= 360) angle_yaw -= 360;
	
	//0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians and not degrees.
	angle_pitch -= angle_roll * sin((float)gyro_yaw * 0.000001066);                   //If the IMU has yawed transfer the roll angle to the pitch angel.
	angle_roll += angle_pitch * sin((float)gyro_yaw * 0.000001066);                   //If the IMU has yawed transfer the pitch angle to the roll angel.
	
	/*angle_yaw -= course_deviation(angle_yaw, actual_compass_heading) / 1200.0;        //Calculate the difference between the gyro and compass heading and make a small correction.
	if(angle_yaw < 0) angle_yaw += 360;                                              //If the compass heading becomes smaller then 0, 360 is added to keep it in the 0 till 360 degrees range.
	else if(angle_yaw >= 360) angle_yaw -= 360; */
	
	//Accelerometer angle calculations
	acc_total_vector = sqrt((acc_x * acc_x) + (acc_y * acc_y) + (acc_z * acc_z));     //Calculate the total accelerometer vector.

	if(abs(acc_y) < acc_total_vector) {	                                             //Prevent the asin function to produce a NaN.
		angle_pitch_acc = asin((float)acc_y / acc_total_vector) * 57.296;               //Calculate the pitch angle.
	}
	if (abs(acc_x) < acc_total_vector) {	                                            //Prevent the asin function to produce a NaN.
	  angle_roll_acc = asin((float)acc_x / acc_total_vector) * 57.296;                //Calculate the roll angle.
	}

	angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;                    //Correct the drift of the gyro pitch angle with the accelerometer pitch angle.
	angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;                       //Correct the drift of the gyro roll angle with the accelerometer roll angle.

	pitch_level_adjust = angle_pitch * 15;                                            //Calculate the pitch angle correction.
	roll_level_adjust = angle_roll * 15;                                              //Calculate the roll angle correction.

	vertical_acceleration_calculations();                                             //Calculate the vertical accelration.

	pid_roll_setpoint_base = channel[0];                                               //Normally channel_1 is the pid_roll_setpoint input.
	pid_pitch_setpoint_base = channel[1];
	
	//Because we added the GPS adjust values we need to make sure that the control limits are not exceded.
	if(pid_roll_setpoint_base > 2000)pid_roll_setpoint_base = 2000;
	if (pid_roll_setpoint_base < 1000)pid_roll_setpoint_base = 1000;
	if (pid_pitch_setpoint_base > 2000)pid_pitch_setpoint_base = 2000;
	if (pid_pitch_setpoint_base < 1000)pid_pitch_setpoint_base = 1000;
	
	//  calculate_pid();   
	calculate_correction();
}

void vertical_acceleration_calculations() {
	acc_z_average_short_rotating_mem_location++;
	if (acc_z_average_short_rotating_mem_location == 25)acc_z_average_short_rotating_mem_location = 0;

	acc_z_average_short_total -= acc_z_average_short[acc_z_average_short_rotating_mem_location];
	acc_z_average_short[acc_z_average_short_rotating_mem_location] = acc_total_vector;
	acc_z_average_short_total += acc_z_average_short[acc_z_average_short_rotating_mem_location];

	if (acc_z_average_short_rotating_mem_location == 0) {
		acc_z_average_long_rotating_mem_location++;

		if (acc_z_average_long_rotating_mem_location == 50)acc_z_average_long_rotating_mem_location = 0;

		acc_z_average_long_total -= acc_z_average_long[acc_z_average_long_rotating_mem_location];
		acc_z_average_long[acc_z_average_long_rotating_mem_location] = acc_z_average_short_total / 25;
		acc_z_average_long_total += acc_z_average_long[acc_z_average_long_rotating_mem_location];
	}
	acc_z_average_total = acc_z_average_long_total / 50;


	acc_alt_integrated += acc_total_vector - acc_z_average_total;
	if (acc_total_vector - acc_z_average_total < 400 || acc_total_vector - acc_z_average_total > 400) {
		if (acc_z_average_short_total / 25 - acc_z_average_total < 500 && acc_z_average_short_total / 25 - acc_z_average_total > -500)
			if (acc_alt_integrated > 200)acc_alt_integrated -= 200;
			else if (acc_alt_integrated < -200)acc_alt_integrated += 200;
	}
}

void calculate_correction()
{
	//The PID set point in degrees per second is determined by the roll receiver input.
  //In the case of deviding by 3 the max roll rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
  pid_roll_setpoint = 0;
	//We need a little dead band of 16us for better results.
	if(pid_roll_setpoint_base > 1508)pid_roll_setpoint = pid_roll_setpoint_base - 1508;
	else if(pid_roll_setpoint_base < 1492)pid_roll_setpoint = pid_roll_setpoint_base - 1492;

	pid_roll_setpoint -= roll_level_adjust;                                           //Subtract the angle correction from the standardized receiver roll input value.
	pid_roll_setpoint /= 3.0;                                                         //Divide the setpoint for the PID roll controller by 3 to get angles in degrees.


	//The PID set point in degrees per second is determined by the pitch receiver input.
	//In the case of deviding by 3 the max pitch rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
	pid_pitch_setpoint = 0;
	//We need a little dead band of 16us for better results.
	if(pid_pitch_setpoint_base > 1508)pid_pitch_setpoint = pid_pitch_setpoint_base - 1508;
	else if(pid_pitch_setpoint_base < 1492)pid_pitch_setpoint = pid_pitch_setpoint_base - 1492;

	pid_pitch_setpoint -= pitch_level_adjust;                                         //Subtract the angle correction from the standardized receiver pitch input value.
	pid_pitch_setpoint /= 3.0;                                                        //Divide the setpoint for the PID pitch controller by 3 to get angles in degrees.

	//The PID set point in degrees per second is determined by the yaw receiver input.
	//In the case of deviding by 3 the max yaw rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
	pid_yaw_setpoint = 0;
	//We need a little dead band of 16us for better results.
	if(channel[2] > 1050) {
		 //Do not yaw when turning off the motors.
	  if(channel[3] > 1508)pid_yaw_setpoint = (channel[3] - 1508) / 3.0;
		else if(channel[3] < 1492)pid_yaw_setpoint = (channel[3] - 1492) / 3.0;
	}

	//Roll calculations
	pid_error_temp = gyro_roll_input - pid_roll_setpoint;
	pid_i_mem_roll += pid_i_gain_roll * pid_error_temp;
	if (pid_i_mem_roll > pid_max_roll)pid_i_mem_roll = pid_max_roll;
	else if (pid_i_mem_roll < pid_max_roll * -1)pid_i_mem_roll = pid_max_roll * -1;

	pid_output_roll = pid_p_gain_roll * pid_error_temp + pid_i_mem_roll + pid_d_gain_roll * (pid_error_temp - pid_last_roll_d_error);
	if (pid_output_roll > pid_max_roll)pid_output_roll = pid_max_roll;
	else if (pid_output_roll < pid_max_roll * -1)pid_output_roll = pid_max_roll * -1;

	pid_last_roll_d_error = pid_error_temp;

	//Pitch calculations
	pid_error_temp = gyro_pitch_input - pid_pitch_setpoint;
	pid_i_mem_pitch += pid_i_gain_pitch * pid_error_temp;
	if (pid_i_mem_pitch > pid_max_pitch)pid_i_mem_pitch = pid_max_pitch;
	else if (pid_i_mem_pitch < pid_max_pitch * -1)pid_i_mem_pitch = pid_max_pitch * -1;

	pid_output_pitch = pid_p_gain_pitch * pid_error_temp + pid_i_mem_pitch + pid_d_gain_pitch * (pid_error_temp - pid_last_pitch_d_error);
	if (pid_output_pitch > pid_max_pitch)pid_output_pitch = pid_max_pitch;
	else if (pid_output_pitch < pid_max_pitch * -1)pid_output_pitch = pid_max_pitch * -1;

	pid_last_pitch_d_error = pid_error_temp;

	//Yaw calculations
	pid_error_temp = gyro_yaw_input - pid_yaw_setpoint;
	pid_i_mem_yaw += pid_i_gain_yaw * pid_error_temp;
	if (pid_i_mem_yaw > pid_max_yaw)pid_i_mem_yaw = pid_max_yaw;
	else if (pid_i_mem_yaw < pid_max_yaw * -1)pid_i_mem_yaw = pid_max_yaw * -1;

	pid_output_yaw = pid_p_gain_yaw * pid_error_temp + pid_i_mem_yaw + pid_d_gain_yaw * (pid_error_temp - pid_last_yaw_d_error);
	if (pid_output_yaw > pid_max_yaw)pid_output_yaw = pid_max_yaw;
	else if (pid_output_yaw < pid_max_yaw * -1)pid_output_yaw = pid_max_yaw * -1;

	pid_last_yaw_d_error = pid_error_temp;
}