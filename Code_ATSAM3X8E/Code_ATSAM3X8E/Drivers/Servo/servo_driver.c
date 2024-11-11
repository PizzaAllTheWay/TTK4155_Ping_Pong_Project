/*
 * servo_driver.c
 *
 * Created: 05/11/2024 00:59:38
 *  Author: Martynas
 */ 



#include "servo_driver.h"



// Local variables
uint64_t last_update_time_servo = 0;  // Store the last update timestamp



void servo_driver_init() {
	pwm_driver_init();
	servo_driver_set_position(0);
}



void servo_driver_set_position(int8_t position) {
	// Arguments:
	//		position (Input: (-100) - 100) [Unit: Percent (%)]
	//
	// Explanation:
	// You can have values -100 to 100 %, witch will then translate to 900 to 2100 us (where 0 is 1500 us)
	// These us are our duty-rate for PWM to drive the servo that we just put right in
	
	// Because servo can be updated to fast, and it takes 20 ms for servo to activate
	// Before we do anything, we must ensure proper amount of time from previous servo time	
	if ((time_now() - last_update_time_servo) < _SERVO_UPDATE_INTERVAL) {
		return;  // Exit if the interval hasn't passed yet
	}
	last_update_time_servo = time_now(); // Update timer
	
	// Ensure position is within the valid range
	if (position < _SERVO_POSITION_MIN) position = _SERVO_POSITION_MIN;
	if (position > _SERVO_POSITION_MAX) position = _SERVO_POSITION_MAX;
	
	// If value in dead zone set Servo to middle
	if ((_SERVO_POSITION_DEADZONE_MIN < position) && (position < _SERVO_POSITION_DEADZONE_MAX)) position = 0;
	
	// Map position from percent % to pulses in micro seconds us
	uint32_t duty_cycle = _SERVO_PULSE_MIN + ((position - _SERVO_POSITION_MIN) * (_SERVO_PULSE_MAX - _SERVO_PULSE_MIN))/(_SERVO_POSITION_MAX - _SERVO_POSITION_MIN);
	
	// Now since PWM Signal from Servo pin on Motor Shield is Normally HIGH this is a problem
	// Because the Servo itself takes in signals that are Normally LOW
	// To correct for this we invert the signal by subtracting duty-cycle itself from max duty-cycle
	// max duty-cycle = 20 000 us
	duty_cycle = 20000 - duty_cycle;
	
	// Sometimes when calculating duty_cycle with position = 0, it still doesen't give duty cycle 1500 
	if (position == 0) duty_cycle = 20000 - 1500;
	
	// Generate Servo Signal
	pwm_driver_set_duty_cycle(PWM_DRIVER_SERVO, duty_cycle);
}