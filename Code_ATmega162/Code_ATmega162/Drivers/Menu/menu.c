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
	//menu init
	
	// Add items that correspond to the enum keys
	snprintf(MenuItemList[PING_PONG].charList, 20, "Ping Pong");
	MenuItemList[PING_PONG].ptr = NULL;  // Add data if necessary

	snprintf(MenuItemList[BONGO_CAT].charList, 20, "Bongo Cat");
	MenuItemList[BONGO_CAT].ptr = NULL;  // Add data if necessary

	snprintf(MenuItemList[NYAN_CAT].charList, 20, "Nyan Cat");
	MenuItemList[NYAN_CAT].ptr = NULL;  // Add data if necessary

	snprintf(MenuItemList[GHOST].charList, 20, "Ghost");
	MenuItemList[GHOST].ptr = NULL;  // Add data if necessary
	
	
	
	
	
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
		
		
		
		uint8_t joystick_direction = controls_get_joystick_direction();
		if (joystick_direction == JOYSTIC_UP){
			uart_send_message("STICK_DIR: UP\0");
		}
		if (joystick_direction == JOYSTIC_DOWN){
			uart_send_message("STICK_DIR: DOWN\0");
		}
		else{
			uart_send_message("stick_dir: ERR\0");
		}



		if (joystick_direction == JOYSTIC_DOWN){
			oled_clear_screen();
			oled_move_screen_stop();
			//_oled_select_page(0);
			if (CURRENT_MENU_STATE == GHOST){
				CURRENT_MENU_STATE = 0;
				_delay_ms(300); //wait 300ms to avoid multiple state changes
			}
			else {
				CURRENT_MENU_STATE++;
				_delay_ms(300); //wait 300ms to avoid multiple state changes
			}
				
		}
		else if (joystick_direction == JOYSTIC_UP){
			oled_clear_screen();
			oled_move_screen_stop();
			//_oled_select_page(0);
			if (CURRENT_MENU_STATE == PING_PONG){
				CURRENT_MENU_STATE = GHOST;
				_delay_ms(300); //wait 300ms to avoid multiple state changes
			}
			else {
				CURRENT_MENU_STATE--;
				_delay_ms(300); //wait 300ms to avoid multiple state changes
			}
		}
		else {
			// Do nothing
		}


				oled_set_text(MenuItemList[CURRENT_MENU_STATE].charList, 0, 0);


        switch (CURRENT_MENU_STATE){
            
            case PING_PONG:

				
				////uint8_t ABC[] = "ABCDEFG";
				//uint8_t test_text[20];
				//test_text[] =  "Hello World!";
				//oled_set_text("ABC", 40, 3);
				
				//oled_set_text(test_text, 10, 2);
				oled_set_area_pattern(0xFF, 60, 5, 100, 7);

				//debug_led_blink();


                break;
            case BONGO_CAT:

				//debug_led_blink();
				//debug_led_blink();
				sprite_bongo_cat();

                break;
            case NYAN_CAT:

				//debug_led_blink();
				//debug_led_blink();
				//debug_led_blink();
				nyan_cat();
                break;
            case GHOST:

                //debug_led_blink();
                //debug_led_blink();
                //debug_led_blink();
                //debug_led_blink();
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
			
			
			
			

		}
	
	   
	return 0;
}