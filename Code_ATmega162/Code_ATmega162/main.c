/*
 * Code_ATmega162.c
 *
 * Created: 26/08/2024 00:52:18
 * Author : Martynas
 */ 

#define F_CPU 4915200UL // Set the CPU clock frequency 4.9152 MHz

#include <avr/io.h>
#include <stdio.h>

//#include "Drivers/UART/uart_driver.h"
#include "Drivers/Tests/test1.h"


int main(void)
{
	test1_init();
	
    /* Replace with your application code */
    while (1) 
    {
		test1_blink();
    }
}

