/*
 * servo_driver.c
 *
 * Created: 05/11/2024 00:59:38
 *  Author: Martynas
 */ 



#include "servo_driver.h"



void servo_driver_init() {
	pwm_driver_init();
	servo_driver_set_position(0);
}



void servo_driver_set_position(int8_t position) {
	// Arguments:
	//		position (Input: (-100) - 100) [Unit: Percent (%)]
	// 
	// Explanation:
	// You can have values -100 to 100 %, witch will then translate to 900 to 2100 us (where 0 & is 1500 us)
	// These us are our duty-rate for PWM to drive the servo that we just put right in
	
	// Ensure position is within the valid range
	if (position < _SERVO_POSITION_MIN) position = _SERVO_POSITION_MIN;
	if (position > _SERVO_POSITION_MAX) position = _SERVO_POSITION_MAX;
	
	// Map position from percent % to pulses in micro seconds us
	uint32_t duty_cycle = _SERVO_PULSE_MIN + ((position - _SERVO_POSITION_MIN) * (_SERVO_PULSE_MAX - _SERVO_PULSE_MIN))/(_SERVO_POSITION_MAX - _SERVO_POSITION_MIN);
	
	// Generate Servo Signal
	pwm_driver_set_duty_cycle(PWM_DRIVER_PIN7, duty_cycle);
}