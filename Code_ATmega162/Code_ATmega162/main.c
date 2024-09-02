/*
 * Code_ATmega162.c
 *
 * Created: 26/08/2024 00:52:18
 * Author : Martynas
 */ 

#define F_CPU 4915200UL // Set the CPU clock frequency 4.9152 MHz
#define BAUD_RATE 9600



#include <stdio.h>

#include "Drivers/Debugging/debug_led.h"
#include "Drivers/UART/uart_driver.h"



int main(void)
{
	// Setup
	debug_led_init();
	uart_init(F_CPU, BAUD_RATE);
	
	// UART Testing
	char uart_message[20];
	uart_receive_message(uart_message, 20);
	
	// Build a message
	char message_to_print[50] = "Iril <3 Said: ";
	int base_length = strlen(message_to_print);
	for (int i = 0; i < 20; i++) {
		message_to_print[base_length + i] = uart_message[i];
	}

    // Infinite loop
    while (1) 
    {
		debug_led_blink();
		uart_send_message(message_to_print);
    }
	
	// Exit
	return(0);
}

