/*
 * debug_led.c
 *
 * Created: 26/10/2024 17:37:05
 *  Author: Martynas
 */



#include "debug_led.h"



void debug_led_blink(void) {
	// Set pin 25 HIGH
	PIOC->PIO_SODR |= PIO_PC14;
	time_spinFor(msecs(100));  // Delay for 100 ms

	// Set pin 25 LOW
	PIOC->PIO_CODR |= PIO_PC14;
	time_spinFor(msecs(200));  // Delay for 200 ms
}

void debug_led_init(void) {
	// Enable the PMC for PIO Controller C (Peripheral ID 13 for PIOC on SAM3X8E)
	// The Power Management Controller (PMC) manages power for all peripheral blocks,
	// allowing us to selectively enable/disable them for power efficiency.
	// Enabling the PMC for PIOC is necessary for any operations on Port C I/O pins.

	// PMC_PCER0 is the PMC Peripheral Clock Enable Register 0.
	// Setting a bit in PMC_PCER0 enables the clock for the corresponding peripheral,
	// specified by a Peripheral ID (PID) defined in the datasheet.
	// Here, we use Peripheral ID 13 (PID13) to enable PIOC.
	
	PMC->PMC_PCER0 |= PMC_PCER0_PID13; // Enable power to PIO Port C via PMC
	
	// Enable the PIO controller for PORT D
	// PIO_PC14 => PIN49
	PIOC->PIO_PER |= PIO_PC14;  // Enable PIO control
	PIOC->PIO_OER |= PIO_PC14;  // Set PIO to Output Enabled Mode
	
	// Blink for a few times to showcase that the debugging LED works
	for (uint8_t i = 0; i < 5; i++) {
		debug_led_blink();
	}
}
