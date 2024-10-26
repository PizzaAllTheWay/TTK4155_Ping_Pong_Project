/*
 * Code_ATSAM3X8E.c
 *
 * Created: 26/10/2024 14:43:32
 * Author : Martynas
 */ 



#define F_CPU 84000000UL // Set the CPU clock frequency 84 MHz



// Necessary Drivers for ATSAM3X8E Microcontrollers to work properly
#include "sam.h"
#include <stdio.h>
#include <stdint.h>



// Custom Drivers
#include "Drivers/Time/time.h"
#include "Drivers/Debugging/debug_led.h"
#include "Drivers/UART/uart_driver.h"
#include "Drivers/CAN/can.h"



int main(void)
{
    // Initialize SAM system ----------
    SystemInit();
	
	// Initialize Debugging ----------
	debug_led_init();
	uart_init(84000000, 9600); // Initialize UART with CPU frequency (84 MHz) and desired baud rate (9600)
	
	// Initialize CAN ----------
	// CAN Baud Rate Calculation:
	// CAN Baud Rate = F_CPU / (2 * BRP * (PROP + PHASE1 + PHASE2 + 1))
	//
	// Solving for BRP:
	//     BRP = (F_CPU / (2 * CAN Baud Rate * (PROP + PHASE1 + PHASE2 + 1))) - 1
	//
	// For F_CPU = 84 MHz, CAN Baud Rate = 1 Mbps, PROP = 6, PHASE1 = 5, PHASE2 = 1:
	//     BRP = (84000000 / (2 * 1000000 * (6 + 5 + 1 + 1))) - 1
	//     BRP = (84000000 / (2 * 1000000 * 13)) - 1
	//     BRP = 2.23, rounded to 2
	CanInit can_config = {
		.brp = 30,  // Baud rate prescaler for 1 Mbps CAN WTF!!!! Why is 2 WRONG!!!??? 39 is the best, but whyyyyy!!!!????
		.phase1 = 5,
		.phase2 = 1,
		.propag = 6,
		.sjw = 1,
		.smp = 1
	}; // CAN initialization parameters

	can_init(can_config, 0); // Initialize CAN with the configuration, no receive interrupt
	
	

	// Infinite Loop
    while (1) {
		// UART Testing ----------
		/*
		printf("Hello World!\n");
		time_spinFor(msecs(1000));  // Delay for 1 000 ms
		*/
		
		// CAN Testing ----------
		/*
		// Define the CAN message
		CanMsg can_message;
		can_message.id = 1; // CAN ID
		can_message.length = 1; // Message length 
		can_message.byte[0] = 'A'; // Data bytes to send
		can_message.byte[1] = 'B';
		can_message.byte[2] = 'C';
		can_message.byte[3] = 'D';
		can_message.byte[4] = 'E';
		can_message.byte[5] = 'F';
		can_message.byte[6] = 'G';
		can_message.byte[7] = 'H';
		
		// Send the message on the CAN bus
		can_tx(can_message);
		
		// Delay to avoid flooding the CAN bus
		time_spinFor(msecs(1000));  
		*/
		
		
		
		// Controller Data CAN Test ----------
		// Define the CAN message structure for receiving
		CanMsg can_message;
		
		// Check if a CAN message is available and read it
		if (can_rx(&can_message)) {
			// Print the received CAN message via UART
			printf("Received CAN message with ID: %d\n", can_message.id);
			printf("Data Length: %d\n", can_message.length);
			printf("Data: ");
			for (uint8_t i = 0; i < can_message.length; i++) {
				printf("%c ", can_message.byte[0]);  // Print each data byte as a character
			}
			printf("\n");

			// Blink LED to indicate a message was received
			debug_led_blink();
		} 
		else {
			// Print that no data received
			printf("No Data :(");
			printf("\n");
			
			// Blink the LED slowly if no message received
			debug_led_blink();
			time_spinFor(msecs(1000));  // Delay for 1 second
		}
		
    }
}
