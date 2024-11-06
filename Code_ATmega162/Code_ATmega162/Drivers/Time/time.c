/*
 * time.c
 *
 * Created: 06/11/2024 21:52:58
 *  Author: Martynas
 */ 



#include "time.h"



// Variables used locally in the file
volatile uint32_t milliseconds = 0;
volatile uint32_t seconds = 0;



void time_init() {
	// Set up Timer 1 for 1 ms intervals
	TCCR1B |= (1 << WGM12);    // Configure Timer 1 in CTC mode
	OCR1A = 15;                // Set value for 1 ms interval (4 MHz clock, 256 prescaler)
	TCCR1B |= (1 << CS12);     // Set prescaler to 256
	TIMSK |= (1 << OCIE1A);    // Enable Timer 1 Compare Match A interrupt
	sei();                     // Enable global interrupts
}



// Timer 1 ISR (called every 1 ms)
ISR(TIMER1_COMPA_vect) {
	milliseconds++;            // Increment milliseconds counter
	
	if (milliseconds % 1000 == 0) {
		seconds++;             // Increment seconds counter every 1000 ms
	}
}



// Function to get the elapsed time in milliseconds
uint32_t time_get_milliseconds() {
	uint32_t ms;
	cli();                     // Disable interrupts to get a consistent value
	ms = milliseconds;
	sei();                     // Re-enable interrupts
	return ms;
}

// Function to get the elapsed time in seconds
uint32_t time_get_seconds() {
	uint32_t sec;
	cli();                     // Disable interrupts to get a consistent value
	sec = seconds;
	sei();                     // Re-enable interrupts
	return sec;
}
