/*
 * Code_ATmega162.c
 *
 * Created: 26/08/2024 00:52:18
 * Author : Martynas
 */ 

#define F_CPU 4915200UL // Set the CPU clock frequency 4.9152 MHz
#define BAUD_RATE 9600

#include <avr/io.h>
#include <stdio.h>

#include "Drivers/UART/uart_driver.h"
#include "Drivers/Tests/test1.h"


int main(void)
{
	// Setup
	test1_init();
	uart_init(F_CPU, BAUD_RATE);
	
    // Infinite loop
    while (1) 
    {
		test1_blink();
		uart_send_message("Iril <3 \n");
    }
	
	// Exit
	return(0);
}

