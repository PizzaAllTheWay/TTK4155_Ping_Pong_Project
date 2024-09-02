/*
 * uart_driver.c
 *
 * Created: 26/08/2024 01:31:12
 *  Author: Martynas
 * 
 * UART Driver for the project to send basic data back and forth for debugging purposes :) 
 */ 

/*
#include "uart_driver.h"

static int uart_putchar(char c, FILE *stream) {
	if (c == '\n') {
		uart_putchar('\r', stream); // Add carriage return for newline
	}
	while (!(UCSRA & (1 << UDRE))) {
		// Wait until the transmit buffer is empty
	}
	UDR = c;
	return 0;
}

static int uart_getchar(FILE *stream) {
	while (!(UCSRA & (1 << RXC))) {
		// Wait for data to be received
	}
	return UDR;
}

void uart_init(void) {
	// Initialize the UART (Baud rate, etc.)
	UBRRH = 0;
	UBRRL = 103; // Example for 9600 baud rate with 16 MHz clock
	UCSRB = (1 << RXEN) | (1 << TXEN); // Enable RX and TX
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0); // 8-bit data

	// Link UART to standard I/O
	static FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
	static FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

	stdout = &uart_output;
	stdin = &uart_input;
}
*/