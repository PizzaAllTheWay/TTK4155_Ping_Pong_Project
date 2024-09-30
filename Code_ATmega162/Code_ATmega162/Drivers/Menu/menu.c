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
	CURRENT_MENU_STATE = NYAN_CAT;
    while (1){
        _delay_ms(500); // Pause 300 ms
        switch (CURRENT_MENU_STATE){
            
            case PING_PONG:
				//uint8_t ABC[] = "ABCDEFG";
				//uint8_t test_text[] = "Hello World!";
				//oled_set_text(ABC, 40, 3);
				//oled_set_text(test_text, 10, 2);
				oled_set_area_pattern(0xFF, 60, 5, 100, 7);

				debug_led_blink();


                break;
            case BONGO_CAT:
				debug_led_blink();
				debug_led_blink();
				sprite_bongo_cat();

                break;
            case NYAN_CAT:
				debug_led_blink();
				debug_led_blink();
				debug_led_blink();
				nyan_cat();
                break;
            case GHOST:
                debug_led_blink();
                debug_led_blink();
                debug_led_blink();
                debug_led_blink();
                sprite_ghost();

                break;
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
                break;
			}
			controls_refresh();
			char uart_message[20];
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
				//CURRENT_MENU_STATE--;
		}   
    }
    






    return 0;
}