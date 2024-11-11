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
	
	// For more information on this topic read data sheet for Micro controller ATmega162
	// Page 186: USART Control and Status Register A – UCSRA
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
	UCSR0C = 0x86; // Clear register to be default 1000 0110
	
	// Select register UCSRC, ensuring we are writing to UCSRC register and not UBRRH register
	// This is because UCSRC and UBRRH can share same memory address because of memory constraints, thats the short of it
	// Luckily we just use the regular mode where we use UCSRC register for all data manipulation instead of UBRRH
	// UBRRH (Used to specify baud rate, clock frequencies and addresses, from what we understand)
	// UCSR0C |= (1 << URSEL0); 
	// However This is a newer version of AVR Architecture and we don't need to specify that
	// UCSRC and UBRRH have separate memory
	
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
	// UBRR = (f_osc_REAL/(16 * baud_rate)) - 1
	//
	// f_osc_REAL = f_osc * prescale
	// f_osc_REAL is the real frequency in the system after pre-scaling
	// Read more in the ATmega162 Data sheet on
	// Page 32 - 42: System Clock and Clock Options
	// Page 41 - 42:  Clock Prescale Register – CLKPR
	// Since we have programed fuses our CKDIV8 Fuse becomes "0011"
	// This means we have set CLKPS3-0 to 0011 witch means  Clock Division Factor of 8
	// prescale = 1/8
	// So our f_osc_REAL will be:
	// f_osc_REAL = f_osc/8
	//
	// This means that UBRR will be:
	// UBRR = (f_osc_REAL/(16 * baud_rate)) - 1
	// UBRR = (f_osc/8) * (1/(16 * baud_rate)) - 1
	// UBRR = (f_osc/(128 * baud_rate)) - 1
	//
	// After calculation we set UBRRL with the 8 least significant bits (LSB)
	// UBRRH is set with 8 MSB
	//
	// For more information on this topic read data sheet for Micro controller ATmega162
	// Page 168: AVR USART vs. AVR UART – Compatibility
	// Page 169: Internal Clock Generation – The Baud Rate Generator
	// Page 172: USART Initialization
	// Page 181: Asynchronous Operational Range
	// Page 186: USART Register Description
	// Page 190: USART Baud Rate Registers – UBRRL and UBRRH
	uint16_t UBRR0_value = (f_osc/(128 * baud_rate)) - 1;
	
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
	
	// For more information on this topic read data sheet for Micro controller ATmega162
	// Page 173: Data Transmission – The USART Transmitter
	// Page 173: Sending Frames with 5 to 8 Data Bit
	UDR0 = msg;
}

void uart_send_message(char *msg) {
	// Loop until the null terminator is encountered
	while (*msg) {  
		uart_send_byte(*msg);  // Send the current character
		msg++;  // Move to the next character in the string
	}
	
	// Add additional spacing for better readability
	char extra_message[3] = "\0\n\r";
	uart_send_byte(extra_message[0]);
	uart_send_byte(extra_message[1]);
	uart_send_byte(extra_message[2]);
}

char _uart_receive_byte(void) {
	// UCSR0A has a status bit that indicates if the UART line has received data
	// We check if this bit is ready when it RXC0 is 1
	// If the bit RXC0 is 0, then we wait
	//while (!(UCSR0A & (1 << RXC0)));
	
	while (!(UCSR0A & (1 << RXC0)));

	// Get and return received data from the buffer
	
	// For more information on this topic read data sheet for Micro controller ATmega162
	// Page 175: Data Reception – The USART Receiver
	// Page 176: Receiving Frames with 5 to 8 Data Bits
	// Page 179: Flushing the Receive Buffer
	return UDR0;
}

void uart_receive_message(char *buffer, uint8_t max_length) {
	char received_char;
	uint8_t i = 0;
	
	// Loop until the buffer is full or until we receive a newline character
	while (i < max_length) {
		received_char = _uart_receive_byte();  // Receive a byte
		
		// Check for newline, which will signify the end of the message
		if (received_char == '\n' || received_char == '\t' || received_char == '\0') {
			break;
		}
		
		// Store the received character in the buffer
		buffer[i] = received_char;
		i++;
	}
	
	// Null terminate the string
	// This will ensure that when we read or manipulate data it is easier to handle it
	buffer[i] = '\0';
}

