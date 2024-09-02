/*
 * uart_driver.c
 *
 * Created: 26/08/2024 01:31:12
 *  Author: Martynas
 * 
 * UART Driver for the project to send basic data back and forth for debugging purposes :) 
 */ 


#include "uart_driver.h"

void uart_init(uint32_t f_osc, unsigned long baud_rate) {
	// USART Control and Status Register
	UCSR0A = 0x00; // Clear register
	
	// UCSRB is responsible for Enable/Disable Transceiving and Receiving of data
	// It also Enables/Disables interrupts 
	// It also Enables/Disables data frame size
	UCSR0B = 0x00; // Clear register
	UCSR0B |= (1 << RXEN0); // Enable UART to receive data
	UCSR0B |= (1 << TXEN0); // Enable UART to transmit data
	
	// USCRC is responsible for configuring the frame format
	// Basically number of data bits, parity mode and stop bits
	// It also sets the mode of operation synchronous/asynchronous
	// Synchronous are for clock that are synchronized, ours are not
	// We will be using asynchronous mode
	UCSR0C = 0x00; // Clear register
	
	// Select register UCSRC, ensuring we are writing to UCSRC register and not UBRRH register
	// This is because UCSRC and UBRRH can share same memory address because of memory constraints, thats the short of it
	// Luckily we just use the regular mode where we use UCSRC register for all data manipulation instead of UBRRH
	// UBRRH (Used to specify baud rate, clock frequencies and addresses, from what we understand)
	// UCSR0C |= (1 << URSEL0); 
	
	// UCSZ0 and UCSZ1 and UCSZ2 specifies what kind of data we want
	// We want data in 8-bit format as we are not crazy <3
	// UCSZ0: Enabled (1)
	// UCSZ1: Enabled (1)
	// UCSZ2: Disabled (0) (Otherwise we enter 9-bit mode X-X)
	// NOTE: We don't Disabled UCSZ2 as we disabled it when clearing the register :)
	UCSR0C |= (1 << UCSZ00);
	UCSR0C |= (1 << UCSZ01);
	
	// NOTE: To make system Synchronous we would need to set UMSEL to HIG
	// However we want to stay in Asynchronous mode, thats why we keep it Disabled :)
	
	// Now we must set baud rate, for this we need to set 2 registers, UBRRL and UBRRH
	// We use 2 registers because we have decided to communicate in NORMAL Synchronously mode
	// These registers need to be configured with appropriate baud rate
	// This baud rate is then calculated with:
	// UBRR = (f_osc/(16 * baud_rate)) - 1
	// After calculation we set UBRRL with the 8 least significant bits (LSB)
	// UBRRH is set with 8 MSB
	uint16_t UBRR0_value = (f_osc/(16 * baud_rate)) - 1;
	
	UBRR0L = UBRR0_value; // LSB
	UBRR0H = (UBRR0_value >> 8); // MSB
}

void uart_send_byte(char msg) {
	// UCSR0A has a status bit that indicates if the UART line is ready to Transmit data
	// We check if this bit is ready when it UDRE0 is 1
	// If the bit UDRE0 is 0, then we wait
	while (!(UCSR0A & (1 << UDRE0)));
	
	// Set message to the register
	// The register will then send this data
	UDR0 = msg;
}

void uart_send_message(char *msg) {
	// Loop until the null terminator is encountered
	while (*msg) {  
		uart_send_byte(*msg);  // Send the current character
		msg++;  // Move to the next character in the string
	}
}

