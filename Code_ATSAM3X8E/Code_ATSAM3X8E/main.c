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
#include "Drivers/Servo/servo_driver.h"
#include "Drivers/IR_LED/ir_led_driver.h"



// Global Constant Variables
#define CAN_ID_NODE1 1
#define CAN_ID_NODE2 2



// Global Variables
uint8_t score = 0;



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
	// 500 kbps CAN Buss
	// CAN BIT TIMINGS:
	//	 - BRP = 83
	// 	 - PHASE1 = 6
	//   - PHASE2 = 6
	//   - PROP = 3
	//   - SJW = 1
	//   - SMP = 0
	// All of these timings MUST correspond to Node1 timings as well as adhere to ATSAM3X8E CAN Controller specifications
	// EXEPT for BRP! as BRP is the Baud Rate Prescaler
	// ATSAM3X8E CAN Controller specifications:
	//		- Bit Rate must be in standard TQ units (Time Quantum), and must be between 8-25 TQ
	//		- SJW must lie in 1 TQ range
	//		- PROP must be 1-8 TQ
	//		- PHASE1 must be 1-8 TQ
	//		- IPT <= PAHSE2 <= PHASE1
	//		- IPT (Information Processing Time) is fixed at 2 TQ and starts at the sample point
	//		- IPT represents the time needed by the CAN controller logic to process and determine the bit level (whether it’s a ‘1’ or ‘0’) at each sample point
	//		- SJW can only be set up to the minimum of PHASE1 or 4 TQ - whichever is smaller defines the maximum value that SJW can take.
	//
	// bit-rate we chose for node1 must be same for node2, ie 16 TQ (ie 500 kbps in our case)
	//		SJW + PROP + PHASE1 + PHASE2 = 16 TQ
	//
	// BRP is the value that gives identical TQ (Time Quantum) to the Node1 TQ
	// Even though Node1 (16 MHz) has different Frequency to Node2 (84 MHz)
	// Since we adjust BRP so that TQ stays the same
	// Then that means data rate on the buss will stay the same
	//
	// Solving for BRP:
	//		TQ = (1 + BRP)/MCK_REAL
	//			TQ: Time Quantum
	//			MCK_REAL: REAL Clock Speed
	//			BRP: Baud Rate Prescaling
	//		TQ * MCK_REAL = 1 + BRP
	//		BRP = TQ * MCK_REAL - 1
	//		MCK_REAL = MCK * prescale
	// MCK is the system clock speed and can be prescaled just like any other microcontroller
	// In ATSAM3X8E case, the peripherals on the high speed bridge are clocked by MCK
	// HOWEVER On the low-speed bridge, where CAN controller resides, MCK_REAL can be clocked at MCK divided by 2 or 4, this is to save power.
	// This division is set by PMC_PCR register as it is the Peripheral Control Register
	// By setting PMC_PCR we are setting a prescale for MCK_REAL, thus:
	//		prescale = 1/2
	// For more information about MCK (Master Clock), read ATSAM3X8E Data Sheet:
	// Page 39: 9.2 APB/AHB Bridge
	// Page 519 - 525: 27. Clock Generator
	// Page 526 - 566: 28. Power Management Controller (PMC)
	// Page 526: 28.2 Embedded Characteristics
	// Page 528 - 529: 28.7 Peripheral Clock Controller
	// Page 538: 28.15 Power Management Controller (PMC) User Interface
	// Page 566: 28.15.26 PMC Peripheral Control Register
	//		prescale = 1/2
	//		MCK_REAL = MCK * prescale
	//		MCK_REAL = MCK/2
	//		BRP = TQ * MCK_REAL - 1
	//		BRP = (TQ * MCK)/2 - 1
	// We also have that TQ is time quantum
	//		TQ = bit-rate/n
	//			bit-rate: data speed on the CAN buss we want
	//			n: Number of time quantums we have per data package sent/received
	//		TQ = 500 kbps/16
	//		BRP = (TQ * MCK)/2 - 1
	//		BRP = ((500 kbps/16) * MCK)/2 - 1
	//			MCK (Master Clock): Just the external crystal oscillator for the ATSAM3X8E Microcontroller
	//		BRP = ((500 kbps/16) * 84 MHz)/2 - 1
	//		BRP = 83
	//
	// For more information about CAN Buss, read ATSAM3X8E Data Sheet:
	// Page 1185 - 1241: 40. Controller Area Network (CAN)
	// Page 1188 - 1199: 40.7 CAN Controller Features
	// Page 1192 - 1197: 40.7.4 CAN 2.0 Standard Features
	// Page 1192 - 1194: 40.7.4.1 CAN Bit Timing Configuration
	CanInit can_config = {
		.brp = 83,  // Baud Rate Prescaling (41)
		.phase1 = 6,
		.phase2 = 6,
		.propag = 3,
		.sjw = 1,
		.smp = 1
	}; // CAN initialization parameters
	can_init(can_config, 1); // Initialize CAN with the configuration and enable receive interrupts
	
	// Initialize Servo ----------
	servo_driver_init();
	
	// Initialize IR LED ----------
	ir_led_driver_init();
	
	

	// Infinite Loop
    while (1) {
		// UART Testing ----------
		/*
		printf("Hello World!\n");
		time_spinFor(msecs(1000));  // Delay for 1 000 ms
		*/
		
		// CAN Testing Node 2 (Node 1 <== Node 2) ----------
		/*
		// Define the CAN message
		CanMsg can_message;
		can_message.id = CAN_ID_NODE2; // CAN ID
		can_message.length = 8; // Message length 
		can_message.byte[0] = 'A'; // Data bytes to send
		can_message.byte[1] = 'A';
		can_message.byte[2] = 'A';
		can_message.byte[3] = 'A';
		can_message.byte[4] = 'A';
		can_message.byte[5] = 'F';
		can_message.byte[6] = 'G';
		can_message.byte[7] = 'H';
		
		// Send the message on the CAN bus
		can_tx(can_message);
		
		// Delay to avoid flooding the CAN bus
		time_spinFor(msecs(1000));
		*/
		
		
		// CAN Testing Node 2 (Node 1 ==> Node 2) ----------
		/*
		// Define the CAN message structure for receiving
		CanMsg can_message;

		// Check RX_MAILBOX_0 for received messages
		if (can_rx(&can_message, RX_MAILBOX_0)) {
			// Check if the received message is from the correct sender ID
			if (can_message.id == CAN_ID_NODE1) {
				// Print ID
				printf("0x%02X ", can_message.id);
				// Print all 8 bytes of data as an array in HEX format
				for (uint8_t i = 0; i < 8; i++) {
					// Since values can be NULL 0x00, it can cause issues when sending data through
					// Thats why we check if null and send -1 instead
					for (int8_t i = 0; i < 8; i++) {
						if (can_message.byte[i] == 0) {
							can_message.byte[i] = (-1);
						}
					}
					printf("0x%02X ", can_message.byte[i]);
				}
				printf("0x%02X ", 0x00); // NULL-TERMINATOR
				printf("\n");
				printf("\r");
				

				// 1 second delay so that the print on screen doesen't overflow to fast
				time_spinFor(msecs(1000));
			}
		}
		*/
		
		
		
		// Servo And IR LED Test ----------
		// Define the CAN message structure for receiving
		CanMsg can_message_rx;

		// Check RX_MAILBOX_0 for received messages
		if (can_rx(&can_message_rx, RX_MAILBOX_0)) {			
			// Check if the received message is from the correct sender ID
			if (can_message_rx.id == CAN_ID_NODE1) {
				// Typecast all the messages into the correct format
				int8_t controls_joystick_y = (int8_t)can_message_rx.byte[0];
				int8_t controls_joystick_x = (int8_t)can_message_rx.byte[1];
				int8_t controls_pad_left = (int8_t)can_message_rx.byte[2];
				int8_t controls_pad_right = (int8_t)can_message_rx.byte[3];
				int8_t controls_joystick_button = (int8_t)can_message_rx.byte[4];
				int8_t controls_pad_left_button = (int8_t)can_message_rx.byte[5];
				int8_t controls_pad_right_button = (int8_t)can_message_rx.byte[6];
				char test_data = (char)can_message_rx.byte[7];
				
				// Control Servo
				servo_driver_set_position(controls_joystick_x);
			}
		}

		if (ir_led_driver_get_status() != 0) {
			// Increment score by 1
			score += 1;
			
			printf("Hellooo!");
			printf("0x%02X ", score);
			printf("\n");
			printf("\r");
			
			// Define the CAN message
			CanMsg can_message_tx;
			can_message_tx.id = CAN_ID_NODE2; // CAN ID
			can_message_tx.length = 8; // Message length
			can_message_tx.byte[0] = score; // Data bytes to send
			can_message_tx.byte[1] = score;
			can_message_tx.byte[2] = score;
			can_message_tx.byte[3] = score;
			can_message_tx.byte[4] = score;
			can_message_tx.byte[5] = score;
			can_message_tx.byte[6] = score;
			can_message_tx.byte[7] = score;
			
			// Send the message on the CAN bus
			can_tx(can_message_tx);
			
			// Delay to avoid flooding the CAN bus
			time_spinFor(msecs(1000));
		}
    }
}
