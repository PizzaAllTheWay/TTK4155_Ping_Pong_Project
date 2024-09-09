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
#include "Drivers/External_SRAM/external_sram_bootleg.h"
#include "Drivers/External_data_bus/external_data_bus.h"



int main(void)
{
	// Debugging Setup
	debug_led_init();
	uart_init(F_CPU, BAUD_RATE);
	
	// Setup External Data buss
	// NOTE: In order for External SRAM, ADC and OLED to function properly this initialization MUST stay here 
	// This initialization configures the data and register buss between them
	external_data_bus_init();
	
	
	
	// Variables used in infinite loop
	char uart_message[6] = "HELLO!";
	char sram_data_buffer[7];
	sram_data_buffer[6] = '\0'; // WARNING because of internal RAM being funky, if the last buffer value is not NULL TERMINATED, we will have big issues down the line
	
	

    // Infinite loop
    while (1) 
    {
		//debug_led_blink();
		
		// SRAM Shenanigans
		external_sram_bootleg_write(EXTERNAL_SRAM_ADDRESS_START, uart_message, 6);
		external_sram_bootleg_read(EXTERNAL_SRAM_ADDRESS_START, sram_data_buffer, 6); // The last 7th bit is just to null everything
		
		uart_send_message("Testing: ");
		uart_send_message(uart_message);
		uart_send_message(sram_data_buffer);
		
		_delay_ms(1);
		
		
		
		// Test external SRAM
		/*
		uart_receive_message(uart_message, 1);
		external_sram_write(EXTERNAL_SRAM_ADDRESS_START, uart_message, 1); // Write UART sent message to SRAM
		external_sram_read(EXTERNAL_SRAM_ADDRESS_START, sram_data_buffer, 1); // Read SRAM saved message from UART and save it into SRAM data buffer
		sram_data_buffer[0] -= 0x01; // Manipulate data before sending it
		uart_send_message(sram_data_buffer);
		*/
		
		/*
		// UART Testing
		char uart_message[20];
		uart_receive_message(uart_message, 20);
		uart_send_message(message_to_print);
		*/
    }
	
	// Exit
	return(0);
}

