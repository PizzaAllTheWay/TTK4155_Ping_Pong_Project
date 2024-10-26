/*
 * Code_ATSAM3X8E.c
 *
 * Created: 26/10/2024 14:43:32
 * Author : Martynas
 */ 



#include "sam.h"



#include "Drivers/Debugging/debug_led.h"
//#include "Drivers/UART/uart_driver.h"



int main(void)
{
    // Initialize SAM system
    SystemInit();
	
	// Initialize Debugging
	debug_led_init();
	
	

    while (1) {
    }
}
