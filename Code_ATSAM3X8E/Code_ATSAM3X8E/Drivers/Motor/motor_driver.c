/*
 * motor_driver.c
 *
 * Created: 10/11/2024 20:09:29
 *  Author: Martynas
 */ 



#include "motor_driver.h"



// Local variables
#define _MOTOR_DIRECTION_PIN PIO_PC23
#define _MOTOR_UPDATE_INTERVAL msecs(50) // WARNING: CAN NOT BE UNDER >20 ms!!! Update Motor Driver only after this period of time (Convert from milliseconds to ticks)
#define _MOTOR_SPEED_DEADZONE_MIN (-1)
#define _MOTOR_SPEED_DEADZONE_MAX 1
#define _MOTOR_SPEED_MIN (-100)
#define _MOTOR_SPEED_MAX 100
#define _MOTOR_PULSE_MIN 0
#define _MOTOR_PULSE_MAX 20000

uint64_t last_update_time_motor = 0;  // Store the last update timestamp



// ENABLE/PWM: D20/SDA0-3 (PB12)
// PHASE/DIRECTION: D7/PWM7 (PC23)
void motor_driver_init() {
	// Initialize PHASE/DIRECTION Pin for Motor ----------
	// Enable the PMC for PIO Controller C (Peripheral ID 13 for PIOC on SAM3X8E)
	// The Power Management Controller (PMC) manages power for all peripheral blocks,
	// allowing us to selectively enable/disable them for power efficiency.
	// Enabling the PMC for PIOC is necessary for any operations on Port C I/O pins.

	// PMC_PCER0 is the PMC Peripheral Clock Enable Register 0.
	// Setting a bit in PMC_PCER0 enables the clock for the corresponding peripheral,
	// specified by a Peripheral ID (PID) defined in the datasheet.
	// Here, we use Peripheral ID 13 (PID13) to enable PIOC.
	
	PMC->PMC_PCER0 |= PMC_PCER0_PID13; // Enable power to PIO Port C via PMC
	
	// Enable the PIO controller for PORT C
	// PIO_PC23 => PIN134 (Motor PHASE/DIRECTION Pin)
	PIOC->PIO_PER |= _MOTOR_DIRECTION_PIN;  // Enable PIO control
	PIOC->PIO_OER |= _MOTOR_DIRECTION_PIN;  // Set PIO to Output Enabled Mode
	
	// Initialize PWM output pin (ENABLE) ----------
	pwm_driver_init();
	
	// Reset racket position ----------
	motor_driver_set_speed(-2); // Give a bit of speed to go back to the left side
	time_spinFor(msecs(10));  // A delay until we know for sure racket is at the left side
	motor_driver_set_speed(0); // Stop Motor
}



void motor_driver_set_speed(int8_t speed) {
	// Arguments:
	//		speed (Input: (-100) - 100) [Unit: Percent (%)]
	//
	// Explanation:
	// You can have values -100 to 100 %, witch will then translate to -2100 us to 2100 us (where 0% is 0 us)
	// These us are our duty-rate for PWM to drive the servo that we just put right in
	// "+" or "-" decides witch direction the motor speed will be at
	
	// Timer -----
	// Because Motor Driver can be updated to fast, and it takes MINIMUM 20 ms for Motor Driver to activate
	// Before we do anything, we must ensure proper amount of time from previous servo time
	if ((time_now() - last_update_time_motor) < _MOTOR_UPDATE_INTERVAL) {
		return;  // Exit if the interval hasn't passed yet
	}
	last_update_time_motor = time_now(); // Update timer
	
	// Conversion -----
	// Ensure speed is within the valid range
	if (speed < _MOTOR_SPEED_MIN) speed = _MOTOR_SPEED_MIN;
	if (speed > _MOTOR_SPEED_MAX) speed = _MOTOR_SPEED_MAX;
	
	// If value in dead zone set Servo to middle
	if ((_MOTOR_SPEED_DEADZONE_MIN < speed) && (speed < _MOTOR_SPEED_DEADZONE_MAX)) speed = 0;
	
	// Map speed from percent % to pulses in micro seconds us
	uint32_t duty_cycle = _MOTOR_PULSE_MIN + (abs(speed) * (_MOTOR_PULSE_MAX - _MOTOR_PULSE_MIN))/(_MOTOR_SPEED_MAX - _MOTOR_SPEED_MIN);
	
	// Now since PWM Signal from Motor pin on Motor Shield is Normally HIGH this is a problem
	// Because the Motor itself takes in signals that are Normally LOW
	// To correct for this we invert the signal by subtracting duty-cycle itself from max duty-cycle
	// max duty-cycle = 20 000 us
	duty_cycle = 20000 - duty_cycle;
	
	// Generate Motor Signal -----
	// Generate correct direction signal
	if (speed < 0) {
		PIOC->PIO_SODR |= _MOTOR_DIRECTION_PIN; // DIRECTION = 1
	}
	else {
		PIOC->PIO_CODR |= _MOTOR_DIRECTION_PIN; // DIRECTION = 0
	}
	
	// Generate corresponding speed signal
	pwm_driver_set_duty_cycle(PWM_DRIVER_MOTOR, duty_cycle);
}