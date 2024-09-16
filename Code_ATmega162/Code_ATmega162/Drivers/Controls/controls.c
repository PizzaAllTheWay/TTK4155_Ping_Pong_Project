/*
 * controls.c
 *
 * Created: 16/09/2024 20:24:47
 *  Author: Martynas
 */ 



#include "controls.h"

#include "../UART/uart_driver.h"
#include <util/delay.h>

// Global variables for inside use
char _adc_data_buffer[ADC_NUM_CHANNELS];  // ADC buffer to store the data for all channels
uint8_t _joystic_x_center = 0;
uint8_t _joystic_y_center = 0;
float _joystic_x = 0.0;
float _joystic_y = 0.0;
float _pad_left = 0.0;
float _pad_right = 0.0;
uint16_t _joystic_button = 0;
uint16_t _pad_left_button = 0;
uint16_t _pad_right_button = 0;



void controls_init() {
	// Initialize ADC
	adc_init();
	
	// Set PINB1 and PINB2 as input pins for buttons A, B and C
	DDRB &= ~(1 << PINB1);  // Button A (Joystick Button)
	DDRB &= ~(1 << PINB2);  // Button B (Left Pad Button)
	DDRB &= ~(1 << PINB3);  // Button C (Right Pad Button)
	
	// Auto calibrate joystick on startup (START) --------------------------------------------------
	uint16_t joystic_x_calibration_buffer = 0;
	uint16_t joystic_y_calibration_buffer = 0;
	
	// Get 100 samples of joystick start position
	for (uint8_t i = 0; i < JOYSTIC_CALIBRATE_SAMPLES; i++) {
		// Wait for ADC to give us values
		while(!get_all_adc_data((uint8_t*)_adc_data_buffer));
		
		// Add data
		joystic_x_calibration_buffer += _adc_data_buffer[1];
		joystic_y_calibration_buffer += _adc_data_buffer[0];
		
		// This delay should not be necessary, however without it the calibration fails
		// SO we have it here O_O
		_delay_ms(10);
	}
	
	// Now that we have all the data
	// We take the average to find center of joystick
	_joystic_x_center = joystic_x_calibration_buffer/JOYSTIC_CALIBRATE_SAMPLES;
	_joystic_y_center = joystic_y_calibration_buffer/JOYSTIC_CALIBRATE_SAMPLES;	
	// Auto calibrate joystick on startup (STOP) --------------------------------------------------
}


void controls_refresh() {
	// Check ADC for joystick and pad values
	/*
	// ===================================================================================
	if (get_all_adc_data((uint8_t*)_adc_data_buffer)) {
		MUST NORMALIZE around the new center
		remember to includef deadzone
		also this is not perect, like center is 160 not 127
		
		// Update joystick X and Y values (normalize)
		if (_adc_data_buffer[1] < (_joystic_x_center - JOYSTIC_DEADZONE) {
			_joystic_x = (float)(_adc_data_buffer[1] - _joystic_x_center);
		}
		_joystic_x = (float)(_adc_data_buffer[1] - _joystic_x_center);  // Subtract center for offset
		_joystic_y = (float)(_adc_data_buffer[0] - _joystic_y_center);  // Subtract center for offset

		// Update pad values if they are also connected to the ADC (if needed)
		_pad_left = (float)_adc_data_buffer[2];  // Example if pad left is on channel 2
		_pad_right = (float)_adc_data_buffer[3]; // Example if pad right is on channel 3
	}
	// ===================================================================================
	*/

	// Check the joystick button (on PINB1)
	_joystic_button = !(PINB & (1 << PINB1));  // Active-low, so check if it's pressed

	// Check the left pad button (on PINB2)
	_pad_left_button = !(PINB & (1 << PINB2));  // Active-low, so check if it's pressed

	// Check the right pad button (on PINB3)
	_pad_right_button = !(PINB & (1 << PINB3));  // Active-low, so check if it's pressed
}


// Function to get joystick X value (float)
float controls_get_joystick_x() {
	return _joystic_x;
}

// Function to get joystick Y value (float)
float controls_get_joystick_y() {
	return _joystic_y;
}

// Function to get pad left value (float)
float controls_get_pad_left() {
	return _pad_left;
}

// Function to get pad right value (float)
float controls_get_pad_right() {
	return _pad_right;
}

// Function to get joystick button state (uint16_t)
uint16_t controls_get_joystick_button() {
	return _joystic_button;
}

// Function to get left pad button state (uint16_t)
uint16_t controls_get_pad_left_button() {
	return _pad_left_button;
}

// Function to get right pad button state (uint16_t)
uint16_t controls_get_pad_right_button() {
	return _pad_right_button;
}


