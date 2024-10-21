/*
 * Code_ATmega162.c
 *
 * Created: 26/08/2024 00:52:18
 * Author : Martynas
 */ 

#define F_CPU 4915200UL //4915200UL // Set the CPU clock frequency 4.9152 MHz


/*
#include "Drivers/Debugging/debug_led.h"
#include "Drivers/UART/uart_driver.h"
#include "Drivers/Controls/controls.h"
#include "Drivers/OLED/oled.h"
#include "Drivers/Menu/menu.h"
*/


//#include "Drivers/CAN/can_driver.h"
#include "Drivers/CAN/spi_driver.h"
#include <util/delay.h>



// Global Constants
#define BAUD_RATE 9600
#define MENU_MAX 4 // 5 menus
#define POKEMON_MAX 2 // 3 Pokemon



// Global Variables
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
	//uart_init(F_CPU, BAUD_RATE);

	// Interface Setup
	//controls_init();
	//menu_init();
	
	
	// Testing ----------
	//can_driver_init(MODE_LOOPBACK);
	spi_driver_init();
	
	

    // Infinite loop
    while (1) 
    {		
		/*
		// UART Testing
		char uart_message[20];
		//uart_receive_message(uart_message, 20);
		//uart_send_message(message_to_print);
		
		
		/*
		// SRAM Testing
		uart_receive_message(uart_message, 10);
		external_sram_write(EXTERNAL_SRAM_ADDRESS_START+1, uart_message, 1); // Write UART sent message to SRAM
		external_sram_read(EXTERNAL_SRAM_ADDRESS_START+1, sram_data_buffer, 1); // Read SRAM saved message from UART and save it into SRAM data buffer
		// NOTE: The last 2nd bit in external_sram_read is just to null everything, thats why we don't fill it
		sram_data_buffer[0] -= 0x01; // Manipulate data before sending it     
		uart_send_message(sram_data_buffer);
		*/
		
		// Joystick Testing
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
		
		
		/*
		// Menu Testing --------------------------------------------------
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
		
		
		
		
		
		
		
		
		
		
		
		/*
		//Testing for SPI
		
		char uart_message[20];
		char *spi_data = spi_read(); // Assuming spi_read() returns a pointer to a char array

		// Use strncpy to copy the data into uart_message
		strncpy(uart_message, spi_data, sizeof(uart_message) - 1);

		
		// Since values can be NULL 0x00, it can cause issues when sending data through
		// Thats why we check if null and send -1 instead
		for (int8_t i = 0; i < 19; i++) {
			if (uart_message[i] == 0) {
				uart_message[i] = (-1);
			}
		}



		// Ensure null termination
		uart_message[sizeof(uart_message) - 1] = '\0';

		// Now uart_message contains the data read from SPI
		uart_send_message(uart_message);
		
		
		//Testing CAN stuff
		uart_send_message("........\0");
		
		char can_message[8] = "EMPTY\0";
				
		mcp2515_write(0x1E,"HelloWor");
		//uart_send_message("CANwrite\0");
		mcp2515_request_to_send();
		//uart_send_message("CAN-sent\0");
		char *spi_data = mcp2515_read(0x1E); // Assuming spi_read() returns a pointer to a char array
		//uart_send_message("CAN-read\0");
		
		// Use strncpy to copy the data into uart_message
		strncpy(can_message, spi_data, sizeof(can_message) - 1);

				
		// Since values can be NULL 0x00, it can cause issues when sending data through
		// Thats why we check if null and send -1 instead
		for (int8_t i = 0; i < 19; i++) {
			if (can_message[i] == 0) {
				can_message[i] = (-1);
			}
		}
		
		// Ensure null termination
		can_message[sizeof(can_message) - 1] = '\0';

		// Now uart_message contains the data read from SPI
		uart_send_message(can_message);
		*/
		// CAN Testing --------------------------------------------------
		spi_driver_select();
		spi_driver_write('B');  // Send dummy data or command
		char received_data = spi_driver_read();  // Read response from MCP2515
		spi_driver_deselect();
		
		// Send a single byte 'A' with CAN ID 0x123
		//can_driver_send_message(0x00, 'A');
		//_delay_ms(10);
		
		/*
		uint16_t can_id = 0;
		char data[8] = "12345678";
		can_driver_send_message(can_id, data);
		*/
		
    }
	
	// Exit
	return(0);
}

