#ifndef PID_LIMITS_H
#define PID_LIMITS_H

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//PID gain and limit settings
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float pid_p_gain_roll = 1.3;                //Gain setting for the pitch and roll P-controller (default = 1.3).  bis 2.0  (fertig.)
float pid_i_gain_roll = 0.06;               //Gain setting for the pitch and roll I-controller (default = 0.04). bis 0.06 get (abgebr.)
float pid_d_gain_roll = 10.5;               //Gain setting for the pitch and roll D-controller (default = 18.0).
int pid_max_roll = 400;                     //Maximum output of the PID-controller (+/-).

#define pid_p_gain_pitch pid_p_gain_roll
#define  pid_i_gain_pitch pid_i_gain_roll
#define pid_d_gain_pitch pid_d_gain_roll
#define pid_max_pitch pid_max_roll

/*float pid_p_gain_pitch = pid_p_gain_roll;   //Gain setting for the pitch P-controller.
float pid_i_gain_pitch = pid_i_gain_roll;   //Gain setting for the pitch I-controller.
float pid_d_gain_pitch = pid_d_gain_roll;   //Gain setting for the pitch D-controller.
int pid_max_pitch = pid_max_roll;           //Maximum output of the PID-controller (+/-).
*/

float pid_p_gain_yaw = 3;                 //Gain setting for the pitch P-controller (default = 4.0).
float pid_i_gain_yaw = 0.02;                //Gain setting for the pitch I-controller (default = 0.02).
float pid_d_gain_yaw = 0.0;                 //Gain setting for the pitch D-controller (default = 0.0).
int pid_max_yaw = 400;                      //Maximum output of the PID-controller (+/-).



#endif // !PID_LIMITS_H

