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
        switch (CURRENT_MENU_STATE){
            
            case PING_PONG:

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
			
			if (controls_get_joystick_direction() == JOYSTIC_UP){
				if (CURRENT_MENU_STATE >= GHOST){
				CURRENT_MENU_STATE = PING_PONG;
				_delay_ms(300); //wait 300ms to avoid multiple state changes
				}
				else {
				CURRENT_MENU_STATE++;
				_delay_ms(300); //wait 300ms to avoid multiple state changes
				}
				//CURRENT_MENU_STATE++;
				
			}
			else if (controls_get_joystick_direction() == JOYSTIC_DOWN){
				if (CURRENT_MENU_STATE == PING_PONG){
				CURRENT_MENU_STATE = GHOST;
				_delay_ms(300); //wait 300ms to avoid multiple state changes
				}
				else {
				CURRENT_MENU_STATE--;
				_delay_ms(300); //wait 300ms to avoid multiple state changes
				}
				//CURRENT_MENU_STATE--;
			else {
				// Do nothing
				_delay_ms(300); //wait 300ms 
			}
		}   
    }
    






    return 0;
}