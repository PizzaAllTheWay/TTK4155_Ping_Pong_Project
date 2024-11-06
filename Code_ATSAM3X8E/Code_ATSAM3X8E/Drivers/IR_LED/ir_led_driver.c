/*
 * ir_led_driver.c
 *
 * Created: 05/11/2024 02:06:59
 *  Author: Martynas
 */ 



#include "ir_led_driver.h"



void ir_led_driver_init() {
	// Initialize A0 Pin to sample IR LED
	adc_driver_init();
}



uint8_t ir_led_driver_get_status() {
	// Static variable to keep track of the last status
	static uint8_t last_status = 0;

	// Get samples
	uint16_t samples_raw[_IR_LED_SAMPLE_NUM];
	uint32_t sum = 0;
	
	for (uint16_t i = 0; i < _IR_LED_SAMPLE_NUM; i++) {
		samples_raw[i] = adc_driver_read();
		sum += samples_raw[i];
	}
	
	// Apply a simple moving average filter
	uint16_t average = sum / _IR_LED_SAMPLE_NUM;

	// Compare the average to the threshold
	if (average < _IR_LED_THRESHOLD) {
		last_status = 1;
	}
	else {
		last_status = 0;
	}
	
	return last_status;
}