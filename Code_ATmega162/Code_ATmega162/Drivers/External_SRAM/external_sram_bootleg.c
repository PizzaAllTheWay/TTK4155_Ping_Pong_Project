/*
 * external_sram.c
 *
 * Created: 05/09/2024 17:50:27
 *  Author: Martynas
 */ 

#include "external_sram_bootleg.h"



void external_sram_bootleg_write(uint16_t start_address, char* data_array, uint16_t array_length) {
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
	for (uint16_t i = 0; i < array_length; i++) {
		uint16_t adress = start_address + i;
		external_data_bus_write(adress, data_array[i]);
	}
}

void external_sram_bootleg_read(uint16_t start_address, char* buffer_array, uint16_t buffer_size) {
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
	for (uint16_t i = 0; i < buffer_size; i++) {
		uint16_t adress = start_address + i;
		buffer_array[i] = external_data_bus_read(adress);
	}
}