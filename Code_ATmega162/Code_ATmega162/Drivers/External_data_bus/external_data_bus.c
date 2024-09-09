/*
 * external_data_buss.c
 *
 * Created: 09/09/2024 20:33:23
 *  Author: Martynas
 */ 

#include "external_data_bus.h"



// NOTE: To make I/O Ports for Microcontroller ATmega162 function as external memory buss, we must initialize it first before we use any of the functionalities
void external_data_bus_init() {
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Since ALE on MCU is SHIT, we need to manually remake a bootleg External SRAM solution
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	// Activate ALE
	DDRE |= (1 << PE1);

	PORTE &= ~(1 << PE1);
	
	// Activate Read/Write pins
	DDRD |= (1 << PD6); // WR
	DDRD |= (1 << PD7); // RD
	
	PORTD |= (1 << PD6); // WR Always HIGH when NOT used
	PORTD |= (1 << PD7); // RD Always HIGH when NOT used
	
	// Activate PORT C as outputs functioning as A8-A11
	DDRC |= (1 << PC0); // A8
	DDRC |= (1 << PC1); // A9
	DDRC |= (1 << PC2); // A10
	DDRC |= (1 << PC3); // A11
	
	PORTC &= ~(1 << PC0); // A8
	PORTC &= ~(1 << PC1); // A9
	PORTC &= ~(1 << PC2); // A10
	PORTC &= ~(1 << PC3); // A11
}

void external_data_bus_write(uint16_t address, char data) {
	// Before doing anything just check that the address is big enough be be considered external data buss address
	if (address < EXTERNAL_DATA_BUS_ADDRESS_START) {
		return;
	}
	
	// Divide address into 2 parts as its to large for single 8 PIO
	uint8_t adress_MSB = (address >> 8) & 0x0F;  // Shift right 8 bits to get the MSB
	uint8_t adress_LSB = address & 0xFF; // Mask the lower 8 bits to get the LSB
	
	// Send out the full address
	// Note: We specify here that A0-A7 is output as it can also be an input later on
	DDRA = 0xFF;
	PORTC = adress_MSB;
	PORTA = adress_LSB;
	
	// Latch latch the address to the latch
	PORTE |= (1 << PE1); // Set latch
	PORTE &= ~(1 << PE1); // Release latch
	
	// Now we start transferring data to external SRAM
	PORTA = data; // Send data to External SRAM
	PORTD &= ~(1 << PD6); // WR LOW when writing data to external SRAM
	_delay_us(1); // Delay to get external SRAM to activate
	PORTD |= (1 << PD6); // WR HIGH after data transfer to external SRAM done
}

char external_data_bus_read(uint16_t address) {
	// Before doing anything just check that the address is big enough be be considered external data buss address
	if (address < EXTERNAL_DATA_BUS_ADDRESS_START) {
		return;
	}
	
	// Divide address into 2 parts as its to large for single 8 PIO
	uint8_t adress_MSB = (address >> 8) & 0x0F;  // Shift right 8 bits to get the MSB
	uint8_t adress_LSB = address & 0xFF; // Mask the lower 8 bits to get the LSB
	
	// Send out the full address
	// Note: We specify here that A0-A7 is output as it can also be an input later on
	DDRA = 0xFF;
	PORTC = adress_MSB;
	PORTA = adress_LSB;
	
	// Latch latch the address to the latch
	PORTE |= (1 << PE1); // Set latch
	PORTE &= ~(1 << PE1); // Release latch
	
	// Now we read data from external SRAM
	PORTA = 0x00; // Clear Port A 
	DDRA = 0x00; // Set A0-A7 as input PIO
	PORTD &= ~(1 << PD7); // RD LOW when reading data from external SRAM
	_delay_us(1); // Delay to get external SRAM to activate
	char data = PORTA; // Read External SRAM data
	PORTD |= (1 << PD7); // RD HIGH after data read from external SRAM done
	
	return data;
}


