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
int8_t _joystic_x = 0.0;
int8_t _joystic_y = 0.0;
int8_t _pad_left = 0.0;
int8_t _pad_right = 0.0;
int8_t _joystic_button = 0;
int8_t _pad_left_button = 0;
int8_t _pad_right_button = 0;



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
	// ===================================================================================
	if (get_all_adc_data((uint8_t*)_adc_data_buffer)) {
		 //_adc_data_buffer[0] -> Joystic Y;
		 //_adc_data_buffer[1] -> Joystic X;
		uint8_t joystic_val_y = _adc_data_buffer[0];
		uint8_t joystic_val_x = _adc_data_buffer[1];
		
		// Get min and max of dead zone
		uint8_t deadzone_y_min = _joystic_y_center - JOYSTIC_DEADZONE;
		uint8_t deadzone_y_max = _joystic_y_center + JOYSTIC_DEADZONE;
		uint8_t deadzone_x_min = _joystic_x_center - JOYSTIC_DEADZONE;
		uint8_t deadzone_x_max = _joystic_x_center + JOYSTIC_DEADZONE;
		
		// Update joystick X and Y values (normalize)
		// Joystick Y
		if (joystic_val_y < deadzone_y_min) {
			_joystic_y = ((deadzone_y_min - joystic_val_y)/((float)(deadzone_y_min))) * (-100.0);
		}
		else if (joystic_val_y > deadzone_y_max) {
			_joystic_y = ((joystic_val_y - deadzone_y_max)/(255.0 - deadzone_y_max)) * 100.0;
		}
		else {
			_joystic_y = 0;
		}
		
		// Joystick X
		if (joystic_val_x < deadzone_x_min) {
			_joystic_x = ((deadzone_x_min - joystic_val_x)/((float)(deadzone_x_min))) * (-100.0);
		}
		else if (joystic_val_x > deadzone_x_max) {
			_joystic_x = ((joystic_val_x - deadzone_x_max)/(255.0 - deadzone_x_max)) * 100.0;
		}
		else {
			_joystic_x = 0;
		}

		// Update pad values
		_pad_left = (_adc_data_buffer[2]/255.0) * 100.0;
		_pad_right = (_adc_data_buffer[3]/255.0) * 100.0; 
	}
	// ===================================================================================

	// Check the joystick button (on PINB1)
	// Normaly HIGH
	if (!(PINB & (1 << PINB1))) {
		_joystic_button = 1;
	}
	else {
		_joystic_button = 0;
	}

	// Check the left pad button (on PINB2)
	// Normaly LOW
	if ((PINB & (1 << PINB2))) {
		_pad_left_button = 1;
	}
	else {
		_pad_left_button = 0;
	}

	// Check the right pad button (on PINB3)
	// Normaly LOW
	if ((PINB & (1 << PINB3))) {
		_pad_right_button = 1;
	}
	else {
		_pad_right_button = 0;
	}
}



// Function to get joystick Y value (float)
int8_t controls_get_joystick_y() {
	return _joystic_y;
}

// Function to get joystick X value (float)
int8_t controls_get_joystick_x() {
	return _joystic_x;
}

// Function to get pad left value (float)
int8_t controls_get_pad_left() {
	return _pad_left;
}

// Function to get pad right value (float)
int8_t controls_get_pad_right() {
	return _pad_right;
}

// Function to get joystick button state (uint16_t)
int8_t controls_get_joystick_button() {
	return _joystic_button;
}

// Function to get left pad button state (uint16_t)
int8_t controls_get_pad_left_button() {
	return _pad_left_button;
}

// Function to get right pad button state (uint16_t)
int8_t controls_get_pad_right_button() {
	return _pad_right_button;
}



//Function to get joystick pointing direction i.e. up, down, left, right, center as a byte
JoystickDirection controls_get_joystick_direction() {
    if (_joystic_x > JOYSTIC_DEADZONE && fabs(_joystic_y) < JOYSTIC_DEADZONE) {
        return JOYSTIC_RIGHT;
    } else if (_joystic_x < -JOYSTIC_DEADZONE && fabs(_joystic_y) < JOYSTIC_DEADZONE) {
        return JOYSTIC_LEFT;
    } else if (_joystic_y > JOYSTIC_DEADZONE && fabs(_joystic_x) < JOYSTIC_DEADZONE) {
        return JOYSTIC_UP;
    } else if (_joystic_y < -JOYSTIC_DEADZONE && fabs(_joystic_x) < JOYSTIC_DEADZONE) {
        return JOYSTIC_DOWN;
    } else {
        return JOYSTIC_CENTER;
    }


	//if (_joystic_y > JOYSTIC_DEADZONE) {
		//return JOYSTIC_UP;
	//} else if (_joystic_y < -JOYSTIC_DEADZONE) {
		//return JOYSTIC_DOWN;
	//} else {
		//return JOYSTIC_CENTER;
	//}
	
	

}
