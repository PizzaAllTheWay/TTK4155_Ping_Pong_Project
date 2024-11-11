/*
 * time.c
 *
 * Created: 06/11/2024 21:52:58
 *  Author: Martynas
 */ 



#include "time.h"



//NOTE: A unsigned long holds values from 0 to 4,294,967,295 (2^32 - 1). It will roll over to 0 after reaching its maximum value.
volatile unsigned long timer2_ticks = 0;  // Global variable for milliseconds



void time_init(unsigned long f_cpu) {
	// Calculate compare match value for 1 ms
	// Used later in the Oscilator calculations
	// We have prescaled with 8
	// 
	// For more information on Extended Timers, read ATmega162 Datasheet:
	// Page 106 - 127: 16-bit Timer/Counter (Timer/Counter1 and Timer/Counter3)
	// Page 128 - 137: 16-bit Timer/Counter Register Description
	unsigned long ctc_match_overflow = ((f_cpu / 1000) / 8);
	
	// Set Timer3 to CTC mode
	TCCR3A = (1 << WGM31);   // Configure Timer3 in CTC mode (Clear Timer on Compare Match)
	TCCR3B = (1 << CS31);    // Set prescaler to 8 (CS32 = 0, CS31 = 1, CS30 = 0)

	// Set compare match value
	OCR3AH = (ctc_match_overflow >> 8) & 0xFF;
	OCR3AL = ctc_match_overflow & 0xFF;

	// Enable Timer3 compare match interrupt
	ETIMSK |= (1 << OCIE3A);

	sei();  // Enable global interrupts
}

// Timer3 Compare Match ISR
ISR(TIMER3_COMPA_vect) {
	timer2_ticks++;
}

// Function to get the current milliseconds
unsigned long time_millis(void) {
	unsigned long millis_return;

	// Atomic block to prevent interrupt issues
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		// Convert ticks to milliseconds
		// NOTE: Calibration Number for Timer3 (ie number 7) was found experimentally :P
		millis_return = timer2_ticks * 7;
	}

	return millis_return;
}