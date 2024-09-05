/*
 * test1.h
 *
 * Created: 02/09/2024 12:38:05
 *  Author: Martynas
 */ 


#ifndef TEST1_H_
#define TEST1_H_

#include <avr/io.h> // This includes definitions for MCU-specific registers for LED Pin to blink :)
#include <stdio.h>
#include <util/delay.h>

void debug_led_blink(void);

// NOTE: To make I/O Ports for Microcontroller ATmega162 function as blinking LED pin, we must initialize it first before we use any of the functionalities
void debug_led_init(void);

#endif /* TEST1_H_ */