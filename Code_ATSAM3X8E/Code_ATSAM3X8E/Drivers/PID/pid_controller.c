/*
 * pid_controller.c
 *
 * Created: 10/11/2024 23:11:40
 *  Author: Martynas
 */ 



#include "pid_controller.h"



// Local Variables ----------
// Constants
#define _PID_INTEGRAL_CLAMP_MIN (-1000)
#define _PID_INTEGRAL_CLAMP_MAX 1000
#define _PID_U_MIN (-100)
#define _PID_U_MAX 100



// PID structure for controller
typedef struct {
	int8_t kp;          // Proportional gain
	int8_t ki;          // Integral gain
	int8_t kd;          // Derivative gain
	int16_t integral;   // Integral term (accumulated error, larger range to prevent overflow)
	int8_t prev_error;  // Last error value (for derivative calculation)
} PID;

PID pid; // Create a global PID instance



// Initialize PID controller with gains
void pid_controller_init(int8_t kp, int8_t ki, int8_t kd) {
	pid.kp = kp;
	pid.ki = ki;
	pid.kd = kd;
	pid.integral = 0;
	pid.prev_error = 0;
}



// Calculate control output (u) in the range of int8_t
int8_t pid_controller_get_u(int8_t reference, int8_t measured_value) {
	// 1. Calculate the error between desired and actual values
	//    (How far off we are from the target)
	int8_t error = reference - measured_value;
	
	// 2. Update integral term by adding the current error
	//    (Keeps track of the total error over time, helping correct any consistent offset)
	pid.integral += error;
	
	// 3. Clamp integral to prevent it from becoming too large or too small
	//    (Limits how much effect the accumulated error has to avoid overreaction/hysteresis)
	if (pid.integral > _PID_INTEGRAL_CLAMP_MAX) pid.integral = _PID_INTEGRAL_CLAMP_MAX;
	if (pid.integral < _PID_INTEGRAL_CLAMP_MIN) pid.integral = _PID_INTEGRAL_CLAMP_MIN;
	
	// 4. Calculate the derivative term as the difference between current and previous error
	//    (Measures how fast the error is changing, helping to prevent overshoot)
	int8_t derivative = error - pid.prev_error;

	// 5. Save the current error as the previous error for the next cycle
	//    (Allows us to track the change in error over time)
	pid.prev_error = error;

	// 6. Calculate the control signal (u)
	//    (Combines the effects of current error, total error, and error change rate)
	int16_t u = (pid.kp * error)               // Proportional part: current error correction
			  + (pid.ki * pid.integral)        // Integral part: accumulated error correction
			  + (pid.kd * derivative);         // Derivative part: rate of error change adjustment

	// 7. Limit the control signal (u) to a set range (e.g., -100 to 100)
	//    (Ensures the output signal stays within safe bounds for the system)
	if (u > _PID_U_MAX) u = _PID_U_MAX;
	if (u < _PID_U_MIN) u = _PID_U_MIN;
	
	// Print reference, measured_value, and control signal u
	//printf("Reference: %d, Measured: %d, Control Signal (u): %d\n\r", reference, measured_value, (int8_t)u);

	// 8. Return the final control signal to be applied
	//    (This is the signal that adjusts the motor speed in our system)
	//	  (This will in turn regulate our position)
	//	  (Then we feed new position data and so on and on the closed loop control system)
	return (int8_t)u;
}
