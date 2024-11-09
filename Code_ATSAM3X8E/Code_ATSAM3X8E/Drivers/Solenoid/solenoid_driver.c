/*
 * solenoid_driver.c
 *
 * Created: 09/11/2024 20:56:22
 *  Author: Martynas
 */ 



#include "solenoid_driver.h"


// PIO_PC18 => PIN45 (Solenoid Pin)
void solenoid_driver_init() {
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
	// PIO_PC18 => PIN45 (Solenoid Pin)
	PIOC->PIO_PER |= PIO_PC18;  // Enable PIO control
	PIOC->PIO_OER |= PIO_PC18;  // Set PIO to Output Enabled Mode
}



void solenoid_driver_on() {
	// Set pin 45 HIGH
	PIOC->PIO_SODR |= PIO_PC18;
}

void solenoid_driver_off() {
	// Set pin 45 LOW
	PIOC->PIO_CODR |= PIO_PC18;
}
