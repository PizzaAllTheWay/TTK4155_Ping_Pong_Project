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
    // Set up Timer 1 for 1 ms intervals using CTC mode
    TCCR1B |= (1 << WGM12);    // Configure Timer 1 in Clear Timer on Compare Match (CTC) mode

    /*
     * Calculate OCR1A for a 1 ms timer interval
     * 
     * System clock frequency (F_CPU) = 4 MHz
     * Prescaler = 8
     * 
     * Timer frequency after prescaling = F_CPU / Prescaler
     *                                 = 4,000,000 Hz / 8
     *                                 = 500,000 Hz
     *
     * Now, we need to achieve a 1 ms interval for the timer, so:
     * 1 ms interval = 0.001 seconds
     *
     * Number of ticks required for 1 ms interval = Timer frequency * Desired interval
     *                                            = 500,000 Hz * 0.001 s
     *                                            = 500 ticks
     * 
     * In CTC mode, the timer counts from 0 up to OCR1A, and the interrupt is triggered when the counter reaches OCR1A.
     * Thus, to get exactly 500 ticks (for 1 ms), we set:
     *
     * OCR1A = 500 - 1 = 499
     *
     * We subtract 1 because the counter starts at 0, so it counts 500 values (0 to 499).
     */
    OCR1A = 499;               // Set OCR1A to 499 for 1 ms interval with 4 MHz clock and 8 prescaler

    TCCR1B |= (1 << CS11);     // Set prescaler to 8 (by setting CS11 bit in TCCR1B)
    
    TIMSK |= (1 << OCIE1A);    // Enable Timer 1 Compare Match A interrupt (triggers interrupt every 1 ms)
    
    sei();                     // Enable global interrupts to allow Timer 1 interrupts to occur
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
