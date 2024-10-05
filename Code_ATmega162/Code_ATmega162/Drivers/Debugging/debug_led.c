/*
 * CFile1.c
 *
 * Created: 26/08/2024 01:29:46
 *  Author: Martynas
 */ 

#include "debug_led.h"

void debug_led_blink(void) {	
	PORTB |= (1 << PB0); // Set PB0 HIGH
	_delay_ms(200); // Pause 200 ms
	PORTB ^= (1 << PB0); // Set PB0 LOW using Toggle (^)
	_delay_ms(200); // Pause 200 ms
}

void debug_led_init(void) {
	PORTB = 0x00; // Set all PINs to LOW
	
	DDRB |= (1 << PB0); // Set PB0 to OUTPUT
	
	// Blink for debugging purposes
	for (uint8_t i = 0; i < 2; i++) {
		debug_led_blink();
	}
}


