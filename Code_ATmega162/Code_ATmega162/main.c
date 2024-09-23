/*
 * Code_ATmega162.c
 *
 * Created: 26/08/2024 00:52:18
 * Author : Martynas
 */ 

#define F_CPU 4915200UL //4915200UL // Set the CPU clock frequency 4.9152 MHz
#define BAUD_RATE 9600



#include "Drivers/Debugging/debug_led.h"
#include "Drivers/UART/uart_driver.h"
#include "Drivers/Controls/controls.h"
#include "Drivers/OLED/oled.h"



#include "Drivers/Menu/sprite_ghost.h"
#include "Drivers/Menu/sprite_nyan_cat.h"
#include "Drivers/Menu/sprite_bongo_cat.h"



int main(void)
{
	// Debugging Setup
	debug_led_init();
	uart_init(F_CPU, BAUD_RATE);

	// Interface Setup
	//controls_init();
	oled_init();
	
	

    // Infinite loop
    while (1) 
    {
		//debug_led_blink();
		
		/*
		// UART Testing
		char uart_message[20];
		uart_receive_message(uart_message, 20);
		uart_send_message(message_to_print);
		*/
		
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
		
		// OLED testing
		/*
		uint8_t ABC[] = "ABCDEFG";
		uint8_t test_text[] = "Hello World!";
		oled_set_text(ABC, 40, 3);
		oled_set_text(test_text, 10, 2);
		oled_set_area_pattern(0xFF, 60, 5, 100, 7);
		*/
		
		// Nyan Cat animation <3
		/*
		for (int8_t i = 0; i < 100; i+=1) {
			oled_set_sprite(sprite_nyan_cat, 8, 8, i, 0); // 8x8 nibbles, 1 nibble = 8 pixels
		}
		oled_clear_screen();
		*/
		
		// Bongo Cat <3
		/*
		oled_set_sprite(sprite_bongo_cat_0, 8, 8, 32, 0); // 8x8 nibbles, 1 nibble = 8 pixels
		oled_set_sprite(sprite_bongo_cat_1, 8, 8, 32, 0); // 8x8 nibbles, 1 nibble = 8 pixels
		oled_set_sprite(sprite_bongo_cat_inverted_0, 8, 8, 32, 0); // 8x8 nibbles, 1 nibble = 8 pixels
		oled_set_sprite(sprite_bongo_cat_inverted_1, 8, 8, 32, 0); // 8x8 nibbles, 1 nibble = 8 
		*/
		
		// Ghost Animation Back ad Forth
		uint8_t y_start = 1;
		uint8_t y_stop = 7;
		uint8_t speed_x = 7;
		uint8_t speed_y = 1;
		uint8_t direction_x = 0;
		
		uint8_t x_start = 32;
		oled_set_sprite(sprite_ghost, 4, 4, x_start, y_start);
		
		for (uint8_t i = 0; i < 100; i++) {
			// RIGHT
			direction_x = 1; 
			oled_move_screen(
			y_start,
			y_stop,
			speed_x,
			speed_y,
			direction_x
			);
			_delay_ms(100);
			
			// LEFT
			direction_x = 0;
			oled_move_screen(
			y_start,
			y_stop,
			speed_x,
			speed_y,
			direction_x
			);
			_delay_ms(100);
		}
		oled_clear_screen();
		
    }
	
	// Exit
	return(0);
}

