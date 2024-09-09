/*
 * external_sram_bootleg.h
 *
 * Created: 09/09/2024 20:09:10
 *  Author: Martynas
 */ 


#ifndef EXTERNAL_SRAM_BOOTLEG_H_
#define EXTERNAL_SRAM_BOOTLEG_H_



#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>  // This includes definitions for MCU-specific registers for External Memory Management
#include "../External_data_bus/external_data_bus.h"



// These constants define the start and end addresses of the external SRAM.
// Typically, these addresses are mapped to the external memory space in the ATmega162 microcontroller.
// The addresses should remain unchanged unless the physical wiring configuration is modified.
#define EXTERNAL_SRAM_ADDRESS_START 0x1800
#define EXTERNAL_SRAM_ADDRESS_END   0x1FFF



void external_sram_bootleg_write(uint16_t start_address, char* data_array, uint16_t array_length);
void external_sram_bootleg_read(uint16_t start_address, char* buffer_array, uint16_t buffer_size);



#endif /* EXTERNAL_SRAM_BOOTLEG_H_ */