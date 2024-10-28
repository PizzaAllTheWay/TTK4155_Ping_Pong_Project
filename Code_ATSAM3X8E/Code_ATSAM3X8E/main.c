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
//#include "Drivers/CAN/can.h"
#include "Drivers/CAN/can_improved.h"



#define CAN_ID_SENDER 0



int main(void)
{
	// Disable Watchdog Timer ----------
	WDT->WDT_MR = WDT_MR_WDDIS; // Set WDDIS bit to disable the watchdog timer
	
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
	//	TQ = (1 + BRP)/MCK
	//  TQ: Time Quantum
	//  MCK: Clock Speed
	//  BRP: Baud Rate Prescaling
	CanInit can_config = {
		.brp = 83,  // Baud Rate Prescaling (41)
		.phase1 = 6,
		.phase2 = 6,
		.propag = 3,
		.sjw = 1,
		.smp = 0
	}; // CAN initialization parameters

	//can_init(can_config, 0); // Initialize CAN with the configuration, no receive interrupt
	can_init_improved(can_config, 1); // Initialize CAN with the configuration and enable receive interrupts
	
	

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
		/*
		// Define the CAN message structure for receiving
		CanMsg can_message;
		
		// Check if a CAN message is available and read it
		if (can_rx(&can_message)) {
			// Print the received CAN message via UART
			printf("Received CAN message with ID: %d\n", can_message.id);
			printf("Data Length: %d\n", can_message.length);
			printf("Data: ");
			for (uint8_t i = 0; i < can_message.length; i++) {
				printf("%c ", can_message.byte[i]);  // Print each data byte as a character
			}
			printf("Data Raw: ");
			for (uint8_t i = 0; i < 8; i++) {
				printf("%02X ", can_message.byte[i]);  // Print each data byte as a hex
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
		*/
		
		

        // Check if a message is received in RX mailbox
		CanMsg received_msg;

		// Check RX_MAILBOX_0 for received messages
		if (can_rx_improved(&received_msg, RX_MAILBOX_0)) {
			if (received_msg.id == CAN_ID_SENDER) {
				printf("RX_MAILBOX_0 ID: %d\n", received_msg.id);

				// Print all 8 bytes of data as an array in HEX format
				printf("RX_MAILBOX_0 RAW: { ");
					for (uint8_t i = 0; i < 8; i++) {
						printf("0x%02X ", received_msg.byte[i]);
					}
				printf("}\n");

				// Print all 8 bytes of data as an array in ASCII format
				printf("RX_MAILBOX_0 ASCII: { ");
					for (uint8_t i = 0; i < 8; i++) {
						if (received_msg.byte[i] >= 32 && received_msg.byte[i] <= 126) {
							// Only print readable ASCII characters
							printf("%c ", received_msg.byte[i]);
							} else {
							// Print a dot for non-readable characters
							printf(". ");
						}
					}
				printf("}\n");

				// Additional processing or delay
				time_spinFor(msecs(1000));
			}
		}


		
		
		/*
        if (can_rx_improved(&received_msg, RX_MAILBOX_0)) {
	        printf("Received CAN message with ID: %d\n", received_msg.id);

	        // Print all 8 bytes of data as an array
	        printf("Data RAW: { ");
		        for (uint8_t i = 0; i < 8; i++) {
			        printf("0x%02X ", received_msg.byte[i]);  // Print each byte in HEX
		        }
	        printf("}\n");
			
			// Print all 8 bytes of data as an array
			printf("Data ASSCI: { ");
				for (uint8_t i = 0; i < 8; i++) {
					printf("%c ", received_msg.byte[i]);  // Print each byte in ASSCI
				}
			printf("}\n");

	        // Additional processing or delay
	        time_spinFor(msecs(500));
        }
		*/

        // Delay to prevent spamming the output
		//printf("Help O_O \n");
		//time_spinFor(msecs(1000));
		
		/*
		// Print out the full buffer in received_msg.byte
		printf("Full received_msg buffer as an array: { ");
			for (uint8_t i = 0; i < 8; i++) {
				printf("%c ", received_msg.byte[i]);
			}
		printf("}\n");
		
		for (uint8_t i = 0; i < 10; i++) {time_spinFor(msecs(1000));}
		*/
    }
}
