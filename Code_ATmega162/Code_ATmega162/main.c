/*
 * Code_ATmega162.c
 *
 * Created: 26/08/2024 00:52:18
 * Author : Martynas
 */ 

#define F_CPU 4915200UL // Set the CPU clock frequency 4.9152 MHz



#include "Drivers/Debugging/debug_led.h"
#include "Drivers/UART/uart_driver.h"
#include "Drivers/Controls/controls.h"
#include "Drivers/OLED/oled.h"
#include "Drivers/Menu/menu.h"
#include "Drivers/CAN/can_driver.h"



// Global Constants
#define BAUD_RATE 9600
#define MENU_MAX 4 // 5 menus
#define POKEMON_MAX 2 // 3 Pokemon



// Global Variables
#define CAN_ID_NODE1 1
#define CAN_ID_NODE2 2

int8_t joystic_y = 0;
int8_t button_L = 0;
int8_t button_R = 0;

int8_t menu_nr = 0;
int8_t bongocat_state = 0;
int8_t pokemon_state = 0;



int main(void)
{
	// Debugging Setup
	debug_led_init();
	uart_init(F_CPU, BAUD_RATE);

	// Interface Setup
	controls_init();
	menu_init();
	
	// CAN Setup
	//can_driver_init(MODE_LOOPBACK); // For Testing CAN
	can_driver_init(MODE_NORMAL);
	
	

    // Infinite loop
    while (1) {		
		// UART Testing ----------
		/*
		char uart_message[20];
		uart_receive_message(uart_message, 20);
		uart_send_message("ABCDEFGH");
		_delay_ms(10);
		*/
		
		// SRAM Testing ----------
		/*
		uart_receive_message(uart_message, 10);
		external_sram_write(EXTERNAL_SRAM_ADDRESS_START+1, uart_message, 1); // Write UART sent message to SRAM
		external_sram_read(EXTERNAL_SRAM_ADDRESS_START+1, sram_data_buffer, 1); // Read SRAM saved message from UART and save it into SRAM data buffer
		// NOTE: The last 2nd bit in external_sram_read is just to null everything, thats why we don't fill it
		sram_data_buffer[0] -= 0x01; // Manipulate data before sending it     
		uart_send_message(sram_data_buffer);
		*/
		
		// Joystick Testing ----------
		/*
		char uart_message[20];
		controls_refresh();
		uart_message[0] = controls_get_joystick_y();
		uart_message[1] = controls_get_joystick_x();
		uart_message[2] = controls_get_pad_left();
		uart_message[3] = controls_get_pad_right();
		uart_message[4] = controls_get_joystick_button();
		uart_message[5] = controls_get_pad_left_button();
		uart_message[6] = controls_get_pad_right_button();
		// Since values can be NULL 0x00, it can cause issues when sending data through
		// Thats why we check if null and send -1 instead
		for (int8_t i = 0; i < 8; i++) {
			if (uart_message[i] == 0) {
				uart_message[i] = (-1);
			}
		}
		uart_message[7] = '\0';
		uart_send_message(uart_message);
		*/
		
		// Menu Testing ----------
		/* --------------------------------------------------
		// Wait for input
		while (joystic_y == 0) {
			controls_refresh();
			joystic_y = controls_get_joystick_y();
			
			// Whilst waiting for input check if any of the buttons were pressed
			button_L = controls_get_pad_left_button();
			button_R = controls_get_pad_right_button();
			
			// Check witch menu we are inn to make the correct decision
			switch (menu_nr) {
				case 0:
					// Ping Pong
					if (button_R == 1) {
						menu_pingpont_set(0);
						
						// Start the game until you exit the game by pressing the left button
						while(button_L == 0) {
							controls_refresh();
							button_L = controls_get_pad_left_button();
							
							// Game (START) --------------------------------------------------
							
							// Game (STOP) --------------------------------------------------
						}
						
						menu_pingpong();
					}
					break;
				case 1:
					// Bongo Cat
					if (button_R == 1) {
						// Invert bongo cats state
						bongocat_state = ((!bongocat_state) & 0x01);
						menu_bongocat_set(bongocat_state);
						
						// Wait until button is released
						while(button_R == 1) {
							controls_refresh();
							button_R = controls_get_pad_right_button();
						}
					}
					break;
				case 2:
					// Nyan Cat
					if (button_R == 1) {
						// Start Animating Nyan Cat
						menu_nyancat_set(1);
						
						// Wait until button is released
						while(button_R == 1) {
							controls_refresh();
							button_R = controls_get_pad_right_button();
						}
						
						//Stop Animating Nyan Cat
						menu_nyancat_set(0);
					}
					break;
				case 3:
					// Ghost
					if (button_R == 1) {
						// Animate whilst button is pressed
						while(button_R == 1) {
							menu_ghost_set(1);
							_delay_ms(30);
							menu_ghost_set(2);
							_delay_ms(30);
							
							controls_refresh();
							button_R = controls_get_pad_right_button();
						}
						
						//Stop Animating Ghost
						menu_ghost_set(0);
					}
					if (button_L == 1) {
						// Animate whilst button is pressed
						while(button_L == 1) {
							menu_ghost_set(3);
							_delay_ms(30);
							menu_ghost_set(4);
							_delay_ms(30);
							
							controls_refresh();
							button_L = controls_get_pad_left_button();
						}
						
						//Stop Animating Ghost
						menu_ghost_set(0);
					}
					break;
				case 4:
					// Pokemon
					if (button_R == 1) {
						// Switch Pokemon
						pokemon_state++;
						
						if (pokemon_state > POKEMON_MAX) {
							pokemon_state = 0;
						}
						
						menu_pokemon_set(pokemon_state);
						
						// Wait for button to be released
						while(button_R == 1) {
							controls_refresh();
							button_R = controls_get_pad_right_button();
						}
					}
					if (button_L == 1) {
						// Switch Pokemon
						pokemon_state--;
						
						if (pokemon_state < 0) {
							pokemon_state = POKEMON_MAX;
						}
						
						menu_pokemon_set(pokemon_state);
						
						// Wait for button to be released
						while(button_L == 1) {
							controls_refresh();
							button_L = controls_get_pad_left_button();
						}
					}
					break;
				default:
					break;
			}
		}
		
		// Reset states
		bongocat_state = 0;
		pokemon_state = 0;
		
		// Switch menus
		if (joystic_y < 0) {
			menu_nr++;
		}
		else if (joystic_y > 0) {
			menu_nr--;
		}
		
		// Ensure we are not out of bound with menus
		if (menu_nr < 0) {
			menu_nr = MENU_MAX;
		}
		else if (menu_nr > MENU_MAX) {
			menu_nr = 0;
		}
		
		// Draw menu
		switch (menu_nr) {
			case 0:
				menu_pingpong();
				break;
			case 1:
				menu_bongocat();
				break;
			case 2:
				menu_nyancat();
				break;
			case 3:
				menu_ghost();
				break;
			case 4:
				menu_pokemon();
				break;
			default:
				break;
		}
		
		// Wait for input to stop
		while (joystic_y != 0) {
			controls_refresh();
			joystic_y = controls_get_joystick_y();
		}*/
		
		// CAN Testing Node 1 ----------
		/*
		can_message_t can_message_send;
		can_message_t can_message_received;

		// Set the CAN message ID
		can_message_send.id = 0x0000;

		// Set the message data (8 bytes max)
		can_message_send.data[0] = 'A'; 
		can_message_send.data[1] = 'B';
		can_message_send.data[2] = 'C';
		can_message_send.data[3] = 'D';
		can_message_send.data[4] = 'E';
		can_message_send.data[5] = 'F';
		can_message_send.data[6] = 'G';
		can_message_send.data[7] = 'H';

		// Set the length of the message
		can_message_send.length = 8;

		// Send the CAN message
		can_driver_send_message(&can_message_send);
		
		// Read the CAN Message
		can_driver_read_message(&can_message_received);

		// Check if a message was received
		if (can_message_received.length > 0) {
			char uart_mesage[9];
			for (uint8_t i = 0; i < 8; i++) {
				uart_mesage[i] = can_message_received.data[i];
			}
			uart_mesage[8] = '\0';
			uart_send_message(uart_mesage);
		} 
		else {
			debug_led_blink();
		}
		*/
		
		
		
		// CAN Testing Node 2 (Node 1 ==> Node 2) ----------
		/*
		can_message_t can_message_send;

		// Set the CAN message ID
		can_message_send.id = CAN_ID_NODE1;

		// Get Joystick Inputs
		// Set the message data to joystick inputs (8 bytes max)
		// Last byte just random as it is not used
		controls_refresh();
		can_message_send.data[0] = controls_get_joystick_y(); 
		can_message_send.data[1] = controls_get_joystick_x();
		can_message_send.data[2] = controls_get_pad_left();
		can_message_send.data[3] = controls_get_pad_right();
		can_message_send.data[4] = controls_get_joystick_button();
		can_message_send.data[5] = controls_get_pad_left_button();
		can_message_send.data[6] = controls_get_pad_right_button();
		can_message_send.data[7] = 'E';

		// Set the length of the message
		can_message_send.length = 8;

		// Send the CAN message
		can_driver_send_message(&can_message_send);
		*/
		
		
		
		// CAN Testing Node 2 (Node 1 <== Node 2) ----------
		/*
		// Try reading from CAN buss
		// When reading if we got new message the interrupt will trigger, letting us know that we got a new message
		can_message_t can_message_received;
		can_driver_read_message(&can_message_received);
		
		// Check the interrupt if it got a new message from CAN buss
		uint8_t is_can_available = can_driver_message_available();
		if (is_can_available) {
			// Now that we know we have a new message pending
			// Check if the message is something we are interested in (ie from a sender ID we want to get data from)
			if ((can_message_received.id == CAN_ID_NODE2) && (can_message_received.length == 8))  {
				// Print out the message
				uart_send_message("Message: ");
				char uart_mesage[10];
				for (uint8_t i = 0; i < 8; i++) {
					uart_mesage[i] = can_message_received.data[i];
				}
				uart_mesage[8] = can_message_received.id + 0x30;
				uart_mesage[9] = '\0';
				uart_send_message(uart_mesage);
			}
		}
		*/
		
		// Servo And IR LED Test ----------
		// Try reading from CAN buss
		// When reading if we got new message the interrupt will trigger, letting us know that we got a new message
		can_message_t can_message_rx;
		can_driver_read_message(&can_message_rx);
		
		// Check the interrupt if it got a new message from CAN buss
		uint8_t is_can_available = can_driver_message_available();
		if (is_can_available) {
			// Now that we know we have a new message pending
			// Check if the message is something we are interested in (ie from a sender ID we want to get data from)
			if ((can_message_rx.id == CAN_ID_NODE2) && (can_message_rx.length == 8))  {
				// Recast the message type to the propper form
				uint8_t score = (uint8_t)can_message_rx.data[0];
				uint8_t test_data1 = (uint8_t)can_message_rx.data[1];
				uint8_t test_data2 = (uint8_t)can_message_rx.data[2];
				uint8_t test_data3 = (uint8_t)can_message_rx.data[3];
				uint8_t test_data4 = (uint8_t)can_message_rx.data[4];
				uint8_t test_data5 = (uint8_t)can_message_rx.data[5];
				uint8_t test_data6 = (uint8_t)can_message_rx.data[6];
				uint8_t test_data7 = (uint8_t)can_message_rx.data[7];

				menu_pingpont_set(score);
			}
		}
		
		can_message_t can_message_tx;

		// Set the CAN message ID
		can_message_tx.id = CAN_ID_NODE1;

		// Get Joystick Inputs
		// Set the message data to joystick inputs (8 bytes max)
		// Last byte just random as it is not used
		controls_refresh();
		can_message_tx.data[0] = controls_get_joystick_y();
		can_message_tx.data[1] = controls_get_joystick_x();
		can_message_tx.data[2] = controls_get_pad_left();
		can_message_tx.data[3] = controls_get_pad_right();
		can_message_tx.data[4] = controls_get_joystick_button();
		can_message_tx.data[5] = controls_get_pad_left_button();
		can_message_tx.data[6] = controls_get_pad_right_button();
		can_message_tx.data[7] = 'E';

		// Set the length of the message
		can_message_tx.length = 8;

		// Send the CAN message
		can_driver_send_message(&can_message_tx);
		_delay_us(1);
    }
	
	// Exit
	return(0);
}

