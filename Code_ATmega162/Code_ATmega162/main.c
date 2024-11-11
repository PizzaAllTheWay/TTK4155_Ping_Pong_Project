/*
 * Code_ATmega162.c
 *
 * Created: 26/08/2024 00:52:18
 * Author : Martynas
 */ 

#define F_CPU 4915200UL // Set the CPU clock frequency 4.9152 MHz



// Custom Libraries
#include "Drivers/Debugging/debug_led.h"
#include "Drivers/UART/uart_driver.h"
#include "Drivers/Time/time.h"
#include "Drivers/Controls/controls.h"
#include "Drivers/OLED/oled.h"
#include "Drivers/Menu/menu.h"
#include "Drivers/CAN/can_driver.h"




#include <avr/io.h>
#include <stdio.h>
#include <time.h>



// Global Constants
#define BAUD_RATE 9600

#define GAME_SCORE_INTERVAL 1000 // 100 [ms] How long it takes for the player to earn a score

#define CAN_ID_NODE1 1 // Our Node
#define CAN_ID_NODE2 2 // Motor Box Node

#define MENU_MAX 4 // 5 menus
#define POKEMON_MAX (11 - 1) // 11 Pokemon (start index 0, so subtract by 1)



// Global Variables
uint8_t score = 0x00;
uint8_t hearts = 0xFF;

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
	time_init(F_CPU);

	// Interface Setup
	controls_init();
	menu_init();
	
	// CAN Setup
	//can_driver_init(MODE_LOOPBACK); // For Testing CAN
	can_driver_init(MODE_NORMAL);
	
	

    // Infinite loop
    while (1) {
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
					// Game (START) --------------------------------------------------
					// Ping Pong
					if (button_R == 1) {
						// Main Game Loop (START) --------------------------------------------------
						// Initialize Game Screen
						menu_pingpont_game(); // Set default screen
						menu_pingpont_set(score, hearts); // Set the score to default
						
						// Set up a timer for counting score
						unsigned long game_time = time_millis();
						
						// Start the game until you exit the game by pressing the left button
						// OR if you game_state = 1 (GAME OVER)
						uint8_t game_state = 0;
						while ((button_L == 0) && (game_state == 0)) {								
							// While no new messages are pending OR exit button not pressed, send controller data
							uint8_t can_message_available = 0;
							while ((!can_message_available) && (button_L == 0)) {
								// Get CAN message status 
								can_message_available = can_driver_message_available();
								
								// Get exit button status
								controls_refresh();
								button_L = controls_get_pad_left_button();

								// Declare CAN message type to format our message in
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
								
								// Increase players score if timer for that
								// A little treat :3 for surviving this long X)
								if ((time_millis() - game_time) > GAME_SCORE_INTERVAL) {
									// Update timer
									game_time = time_millis();
									
									// Increase score
									score++;
									
									// Update the screen with the new score
									menu_pingpont_set(score, hearts);
								}
							}
							
							// Once we get a pending message we read the message
							can_message_t can_message_rx;
							can_driver_read_message(&can_message_rx);

							// Check if the message is something we are interested in (ie from a sender ID we want to get data from and is not corrupted)
							if ((can_message_rx.id == CAN_ID_NODE2) && (can_message_rx.length == 8))  {
								// Recast the message type to the proper form
								uint8_t can_hearts = (uint8_t)can_message_rx.data[0];
								uint8_t test_data1 = (uint8_t)can_message_rx.data[1];
								uint8_t test_data2 = (uint8_t)can_message_rx.data[2];
								uint8_t test_data3 = (uint8_t)can_message_rx.data[3];
								uint8_t test_data4 = (uint8_t)can_message_rx.data[4];
								uint8_t test_data5 = (uint8_t)can_message_rx.data[5];
								uint8_t test_data6 = (uint8_t)can_message_rx.data[6];
								uint8_t test_data7 = (uint8_t)can_message_rx.data[7];
								
								// Check if the hearts we got from CAN buss is different from the hearts we stored
								// This is so that we don't update screen unnecessary
								if (hearts != can_hearts) {
									// Update hearts to the new CAN hearts
									hearts = can_hearts;
									
									// Update the screen with the new hearts
									menu_pingpont_set(score, hearts);
									
									// Check if the new hearts is not in range between 1 and 9
									// If so => GAME OVER
									if (hearts < 1 || hearts > 9) {
										game_state = 1; // GAME OVER
									}
								}
							}
							// Main Game Loop (STOP) --------------------------------------------------
						}
						
						// Reset Score ----------
						score = 0x00;
						
						// Reset Hearts ----------
						hearts = 0xFF;
						
						// Send Reset Can Message ----------
						// For the last time send CAN Message to reset motor box to its default state
						// Declare CAN message type to format our message in
						can_message_t can_message_tx;

						// Set the CAN message ID
						can_message_tx.id = CAN_ID_NODE1;

						// Send default Joystick Inputs
						// Set the message data to joystick inputs (8 bytes max)
						// Last byte just random as it is not used
						controls_refresh();
						can_message_tx.data[0] = 0;
						can_message_tx.data[1] = 0;
						can_message_tx.data[2] = 0;
						can_message_tx.data[3] = 0;
						can_message_tx.data[4] = 0;
						can_message_tx.data[5] = 0;
						can_message_tx.data[6] = 0;
						can_message_tx.data[7] = 'R'; // R => RESET HEARTS (Any other key will be ignored on NODE 2)

						// Set the length of the message
						can_message_tx.length = 8;

						// Send the CAN message
						can_driver_send_message(&can_message_tx);
						
						// If we didn't just exit the game but had a GAME OVER state, display game over screen
						if (game_state == 1) {
							// Display Game over screen
							menu_game_over();
							
							// Wait until a exit button is pressed to exit game over screen
							while (button_L == 0) {
								// Get newest controller data and check if a button was pressed
								controls_refresh();								
								button_L = controls_get_pad_left_button();
							}
						}
						
						// Go Back to the Menu ----------
						menu_pingpong();
					}
					break;
					// Game (START) --------------------------------------------------
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
		}
    }
	
	// Exit
	return(0);
}

