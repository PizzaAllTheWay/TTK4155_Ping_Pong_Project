/*
 * debug_led.h
 *
 * Created: 26/10/2024 17:37:12
 *  Author: Martynas
 */ 


#ifndef DEBUG_LED_H_
#define DEBUG_LED_H_



// Must have for basic functionalities
#include "sam.h"



// Timer pause function for blinking
#include "../Time/time.h"



// Functions
void debug_led_blink(void);
// NOTE: To make I/O Ports for Microcontroller ATmega162 function as blinking LED pin, we must initialize it first before we use any of the functionalities
void debug_led_init(void);



#endif /* DEBUG_LED_H_ */