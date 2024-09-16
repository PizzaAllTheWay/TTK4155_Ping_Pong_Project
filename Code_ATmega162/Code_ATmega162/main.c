/*
 * Code_ATmega162.c
 *
 * Created: 26/08/2024 00:52:18
 * Author : Martynas
 */ 

#define F_CPU 4915200UL //4915200UL // Set the CPU clock frequency 4.9152 MHz
#define BAUD_RATE 9600



#include <stdio.h>

#include "Drivers/Debugging/debug_led.h"
#include "Drivers/UART/uart_driver.h"
#include "Drivers/ADC/adc.h"



int main(void)
{
	// Debugging Setup
	debug_led_init();
	uart_init(F_CPU, BAUD_RATE);
	
	// Interface Setup
	adc_init();

	
	
	// Variables used in infinite loop
	char adc_data_buffer[ADC_NUM_CHANNELS];  // ADC buffer to store the data for all channels
	char uart_message[ADC_NUM_CHANNELS + 1]; // Buffer to format the UART message, +1 for null termination
	char test[10];
	

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
		if (get_all_adc_data((uint8_t*)adc_data_buffer)) {
			// Prepare the message to send via UART (e.g., as a string or raw bytes)
			for (uint8_t i = 0; i < ADC_NUM_CHANNELS; i++) {
				uart_message[i] = adc_data_buffer[i];  // Copy ADC data into the UART message buffer
			}

			// Optionally null-terminate the message (if sending as a string)
			uart_message[1] = '\0';
			uart_message[ADC_NUM_CHANNELS] = '\0';  // Null-terminate


			// Send the data buffer through UART
			test[0] = uart_message[0];
			uart_send_message(test);  // Send the message over UART
		}
    }
	
	// Exit
	return(0);
}

