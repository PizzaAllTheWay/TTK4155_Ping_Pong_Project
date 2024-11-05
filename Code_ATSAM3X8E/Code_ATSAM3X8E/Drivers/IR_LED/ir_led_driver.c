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
	// Static variables to keep track of the last status and the last sample time
	static uint8_t last_status = 0;
	static uint64_t last_sample_time = 0;

	// Get the current time in ticks
	uint64_t current_time = time_now();

	// Calculate the sampling interval in ticks
	uint64_t sampling_interval_ticks = msecs(_IR_LED_SAMPLE_INTERVAL_MS);

	// Check if enough time has passed since the last sampling
	if ((current_time - last_sample_time) >= sampling_interval_ticks) {
		// Update the last sample time
		last_sample_time = current_time;

		// Get samples
		uint16_t samples_raw[_IR_LED_SAMPLE_NUM];
		uint32_t sum = 0;
		
		for (uint16_t i = 0; i < _IR_LED_SAMPLE_NUM; i++) {
			samples_raw[i] = adc_driver_read();
			sum += samples_raw[i];
		}
		
		// Apply a simple moving average filter
		uint16_t average = sum/_IR_LED_SAMPLE_NUM;

		// Compare the average to the threshold
		if (average < _IR_LED_THRESHOLD) {
			last_status = 1;
		} 
		else {
			last_status = 0;
		}
		
		return last_status;
	}
	
	// If not enough time has passed, return 0
	return 0;
}