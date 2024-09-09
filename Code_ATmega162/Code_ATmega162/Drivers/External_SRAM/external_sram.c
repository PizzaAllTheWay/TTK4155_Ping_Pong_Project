/*
 * external_sram.c
 *
 * Created: 05/09/2024 17:50:27
 *  Author: Martynas
 */ 

#include "external_sram.h"



void external_sram_init() {
	// The ATmega162 is configured to use PORT A (PA0 - PA7) as the data bus for reading and writing bytes to external memory.
	// The ALE (Address Latch Enable) signal on pin PE1 is used to differentiate between address and data phases.

	// PA0 - PA7: Serve as the data bus, handling both reading and writing operations to/from external SRAM.
	// PE1 (ALE): The Address Latch Enable signal (ALE) on PE1 latches the lower part of the address from PORT A.
	// When ALE is high, PORT A sends the lower byte of the address; when ALE is low, PORT A transmits data.

	// PC0 - PC3: These pins on PORT C hold the higher nibble (upper 4 bits) of the address, allowing the ATmega162
	// to address larger memory spaces. Together, PORT C and PORT A enable addressing up to 64 KB of external memory.

	// PD6 (WR): Write strobe pin.
	// When writing to external memory, the WR pin on PD6 goes low to signal the start of a write operation.
	// It remains low for the duration of the write cycle and returns high once the data has been written to memory.

	// PD7 (RD): Read strobe pin.
	// The RD pin on PD7 goes low to signal the start of a read operation from external memory.
	// It stays low while reading data from memory and goes high once the data has been latched into the microcontroller.

	// For more information on this topic read data sheet for Micro controller ATmega162
	// Page  18: SRAM Data Memory
	// Page  25: I/O Memory
	// Page  26: External Memory Interface
	// Page  26: Using the External Memory Interface
	// Page  27: Address Latch Requirements
	// Page  30: XMEM Register Description
	// Page  63: I/O-Ports
	// Page  70: Alternate Functions of Port A
	// Page  75: Alternate Functions of Port C
	// Page  81: Alternate Functions of Port E
	// Page 304: Register Summary
	
	// Enable the External Memory Interface (XMEM)
	// Clear the MCUCR register
	MCUCR = 0x00;
	
	// Set the SRE bit in MCUCR to enable external SRAM
	MCUCR |= (1 << SRE);  // SRE (bit 7) in MCUCR enables the external memory interface
	
	// Clear the EMCUCR register
	EMCUCR = 0x00;
	
	// Configure wait states for the external memory interface
	// SRW11 and SRW10 control the wait states for the upper sector of the external memory
	// SRW01 and SRW00 control the wait states for the lower sector
	// Here, we configure the memory to operate without wait states:
	MCUCR &= ~((1 << SRW10) | (1 << SRW11)); // No wait state for the upper sector
	EMCUCR &= ~((1 << SRW00) | (1 << SRW01)); // No wait state for the lower sector
	
	// Additional configuration: Set up the sector size if needed.
	// You can divide the memory into two sectors with different wait state settings if required.
	// Leave this at the default (one sector) for now.
	
	// Enable the Bus Keeper by setting the XMBK bit in SFIOR
	// This ensures that the data bus (AD7-0) retains its last valid logic state when no device (SRAM, ADC, OLED) is actively driving the bus.
	// It prevents the bus from floating, reducing noise and power consumption while maintaining bus stability.
	// Since all external devices share the same bus and have tri-state (floating) outputs when not selected, enabling the Bus Keeper ensures the bus is not left in a floating state.
	SFIOR |= (1 << XMBK);  // Enable the Bus Keeper for the external data bus (AD7-0)
	
	SFIOR |= (1 << XMM2);
	// The external SRAM is now configured and ready to use.
}

void external_sram_write(uint16_t start_address, char* data_array, uint16_t array_length) {
	// For more information on this topic read data sheet for Micro controller ATmega162
	// Page  32: Using all Locations of External Memory Smaller than 64 KB
	
	// Pointer to the external SRAM
	volatile char* sram_data = (char*) EXTERNAL_SRAM_ADDRESS_START;
	
	// Make sure the start address is within the valid SRAM range (0x1800-0x1FFF)
	if (start_address < EXTERNAL_SRAM_ADDRESS_START) {
		return;
	}
	if (start_address > EXTERNAL_SRAM_ADDRESS_END) {
		return;
	}

	// If the current address exceeds the SRAM end address, stop writing (i.e over 0x1FFF)
	if (array_length > (EXTERNAL_SRAM_ADDRESS_END - start_address)) {
		return;
	}
	
	// Write to the external memory
	for (uint16_t current_address = 0; current_address < array_length; current_address++) {
		sram_data[current_address] = data_array[current_address];
		
		_delay_ms(1); // A small delay because the SRAM data buss is EXTREMELY noisy X-X
	}
}

void external_sram_read(uint16_t start_address, char* buffer_array, uint16_t buffer_size) {
	// For more information on this topic read data sheet for Micro controller ATmega162
	// Page  32: Using all Locations of External Memory Smaller than 64 KB
	
	// Pointer to the external SRAM, starting at the SRAM base address
	volatile char* sram_data = (char*) EXTERNAL_SRAM_ADDRESS_START;

	// Make sure the start address is within the valid SRAM range (0x1800-0x1FFF)
	if (start_address < EXTERNAL_SRAM_ADDRESS_START) {
		return;
	}
	if (start_address > EXTERNAL_SRAM_ADDRESS_END) {
		return;
	}
	
	// If the current address exceeds the SRAM end address, stop writing (i.e over 0x1FFF)
	if (buffer_size > (EXTERNAL_SRAM_ADDRESS_END - start_address)) {
		return;
	}

	// Read from the external memory
	for (uint16_t current_address = 0; current_address < buffer_size; current_address++) {
		buffer_array[current_address] = sram_data[current_address];
		
		_delay_ms(1); // A small delay because the SRAM data buss is EXTREMELY noisy X-X
	}
}