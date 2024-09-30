/*
 * menu.c
 *
 * Created: 23/09/2024 18:23:01
 *  Author: Martynas
 */ 
#include "menu.h"
//#include "../Debugging/debug_led.h"
#include "../Controls/controls.h"

int menu_loop (){
    // Infinite loop
	//CURRENT_MENU_STATE = NYAN_CAT; // having this here defeats the debugging?
    while (1){
        //_delay_ms(500); // Pause 300 ms
		controls_refresh();
					
		// Joystick Testing
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

        switch (CURRENT_MENU_STATE){
            
            case PING_PONG:
				//uint8_t ABC[] = "ABCDEFG";
				//uint8_t test_text[] = "Hello World!";
				//oled_set_text(ABC, 40, 3);
				//oled_set_text(test_text, 10, 2);
				oled_set_area_pattern(0xFF, 60, 5, 100, 7);

				debug_led_blink();


                continue;
            case BONGO_CAT:
				debug_led_blink();
				debug_led_blink();
				sprite_bongo_cat();

                continue;
            case NYAN_CAT:
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				//nyan_cat();
                continue;
            case GHOST:
                debug_led_blink();
                debug_led_blink();
                debug_led_blink();
                debug_led_blink();
                sprite_ghost();

                continue;
            default:
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();				
                continue;
			}
			
			
			
			//char uart_message[20];
			// Debugging: Print the joystick direction
			uint8_t joystick_direction = controls_get_joystick_direction();
			//uart_message[0]= "A";
			//uart_message[1]= "B";
			//uart_message[2]= "C";
			//uart_message[3]= joystick_direction + 50;
			
			//uart_message[4] = '\0';
			//uart_send_message(uart_message);			
			
			if (controls_get_joystick_y() < 50){
				uart_send_message("UP");
			}
			if (joystick_direction == JOYSTIC_DOWN){
				uart_send_message("DOWN");
			}
			else{
				uart_send_message("ERR");
			}

			if (controls_get_joystick_y() > 50){
			debug_led_blink();
			debug_led_blink();
				if (CURRENT_MENU_STATE == GHOST){
				CURRENT_MENU_STATE = 0;
				_delay_ms(3000); //wait 300ms to avoid multiple state changes
				}
				else {
				CURRENT_MENU_STATE++;
				_delay_ms(3000); //wait 300ms to avoid multiple state changes
				}
				//CURRENT_MENU_STATE++;
				
			}
			else if (controls_get_joystick_direction() == JOYSTIC_DOWN){
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				if (CURRENT_MENU_STATE == PING_PONG){
					CURRENT_MENU_STATE = GHOST;
					_delay_ms(3000); //wait 300ms to avoid multiple state changes
					}
					else {
					CURRENT_MENU_STATE--;
					_delay_ms(3000); //wait 300ms to avoid multiple state changes
				}
			}
			//CURRENT_MENU_STATE--;
			else {
				// Do nothing
				_delay_ms(300); //wait 300ms 
			}
		}
	
	   
	return 0;
}