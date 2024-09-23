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



int main(void)
{
	// Debugging Setup
	debug_led_init();
	uart_init(F_CPU, BAUD_RATE);
	
	// Interface Setup
	//controls_init();
	oled_init();
	
	
	
	// Variables used in infinite loop
	//char adc_data_buffer[ADC_NUM_CHANNELS];  // ADC buffer to store the data for all channels
	//char uart_message[8]; // Buffer to format the UART message, +1 for null termination



    // Infinite loop
    while (1) 
    {
		debug_led_blink();
		
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
		oled_clear_page(0);
		oled_set_letter('!');
		oled_set_letter(' ');
    }
	
	// Exit
	return(0);
}

