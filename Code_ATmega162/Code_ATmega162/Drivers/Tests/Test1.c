/*
 * CFile1.c
 *
 * Created: 26/08/2024 01:29:46
 *  Author: Martynas
 */ 

#include "test1.h"

void test1_init(void) {
	PORTB = 0x00; // Set all PINs to LOW
	
	DDRB |= (1 << PB0); // Set PB0 to OUTPUT
}

void test1_blink(void) {
	PORTB |= (1 << PB0); // Set PB0 HIGH
	_delay_ms(500); // Pause 500 ms
	PORTB ^= (1 << PB0); // Set PB0 LOW using Toggle (^)
	_delay_ms(500); // Pause 500 ms
}


