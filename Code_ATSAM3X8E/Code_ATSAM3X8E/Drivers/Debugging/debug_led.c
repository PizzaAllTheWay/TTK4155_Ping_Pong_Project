/*
 * debug_led.c
 *
 * Created: 26/10/2024 17:37:05
 *  Author: Martynas
 */



#include "debug_led.h"



// Internal important values used in Encoder Driver code
#define _SSC_WRITE_PROTECT_KEY 0x535343
#define _PMC_WRITE_PROTECT_KEY 0x504D43



void debug_led_blink(void) {
	// Set pin 49 HIGH
	PIOC->PIO_SODR |= PIO_PC14;
	time_spinFor(msecs(100));  // Delay for 100 ms

	// Set pin 49 LOW
	PIOC->PIO_CODR |= PIO_PC14;
	time_spinFor(msecs(200));  // Delay for 200 ms
}

void debug_led_init(void) {
	// Before Doing anything with PIO pins, we must enable write permissions to the registers
	// This will allow us to configure our PIO pins in special modes any way we like
	// Like for example setting PIO pins up for PWM mode
	// After disabling write locking we wait and check that the permissions are set as we want
	//
	// WPEN = 0 (Disables the Write Protect if WPKEY corresponds to 0x535343 (“SSC” in ASCII))
	// SSC_WRITE_PROTECT_KEY = 0x535343 (Idiot security to ensure the person who is writing to PWM signals has read the data sheet and understands the consequences of fucking up PWM registers, withouth this key no further PWM action can be made)
	//
	// For more information about write protection registers, read ATSAM3X8E Data Sheet:
	// Page 574 - 674: 30. Synchronous Serial Controller (SSC)
	// Page 616: 30.9.17 SSC Write Protect Mode Register
	// Page 617: 30.9.18 SSC Write Protect Status Register
	uint8_t WPEN = 0;
	SSC->SSC_WPMR = (_SSC_WRITE_PROTECT_KEY << 8) // Set WPKEY
	| WPEN;                       // Set WPEN to disable pin write protect
	
	while (SSC->SSC_WPSR != 0) {
		// Poll until Write Protect Violation Status bit is 0
		// Also wait until bo Write Protect Violation Source emerge
		// (ie No Write Protect Violation has occurred)
	}
	
	// Before enabling the PWM Clock we must give permission to write to PMC registers
	// For this we configure the PMC write protect register
	// After that we check and wait that we have actually disabled write protections for PMC registers
	//
	// WPEN = 0 (Disables the Write Protect if WPKEY corresponds to 0x504D43 (“SSC” in ASCII))
	// PMC_WRITE_PROTECT_KEY = 0x504D43 (Idiot security to ensure the person who is writing to PWM signals has read the data sheet and understands the consequences of fucking up PWM registers, withouth this key no further PWM action can be made)
	//
	// For more information about PMC Write Protection, read ATSAM3X8E Data Sheet:
	// Page 526 - 566: 28. Power Management Controller (PMC)
	// Page 561: 28.15.21  PMC Write Protect Mode Register
	// Page 562: 28.15.22  PMC Write Protect Status Register
	PMC->PMC_WPMR = (_PMC_WRITE_PROTECT_KEY << 8) // Set WPKEY
	| WPEN;						  // Set WPEN to disable pin write protect
	
	while (PMC->PMC_WPSR != 0) {
		// Poll until Write Protect Violation Status bit is 0
		// Also wait until field Write Protect Violation Source don't emerge longer
		// (ie No Write Protect Violation has occurred)
	}
	
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
	// PIO_PC14 => PIN49
	PIOC->PIO_PER |= PIO_PC14;  // Enable PIO control
	PIOC->PIO_OER |= PIO_PC14;  // Set PIO to Output Enabled Mode
	
	// Blink for a few times to showcase that the debugging LED works
	for (uint8_t i = 0; i < 5; i++) {
		debug_led_blink();
	}
}
