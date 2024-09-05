/*
 * external_sram.h
 *
 * Created: 05/09/2024 17:56:34
 *  Author: Martynas
 */ 


#ifndef EXTERNAL_SRAM_H_
#define EXTERNAL_SRAM_H_

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>  // This includes definitions for MCU-specific registers for External Memory Management



// These constants define the start and end addresses of the external SRAM.
// Typically, these addresses are mapped to the external memory space in the ATmega162 microcontroller.
// The addresses should remain unchanged unless the physical wiring configuration is modified.
#define EXTERNAL_SRAM_ADDRESS_START 0x1800
#define EXTERNAL_SRAM_ADDRESS_END   0x1FFF



// NOTE: To make I/O Ports for Microcontroller ATmega162 function as external memory buss, we must initialize it first before we use any of the functionalities
void external_sram_init();

void external_sram_write(uint16_t start_address, char* data_array, uint16_t array_length);
void external_sram_read(uint16_t start_address, char* buffer_array, uint16_t buffer_size);

#endif /* EXTERNAL_SRAM_H_ */