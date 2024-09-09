/*
 * external_data_buss.h
 *
 * Created: 09/09/2024 20:33:14
 *  Author: Martynas
 */ 


#ifndef EXTERNAL_DATA_BUS_H_
#define EXTERNAL_DATA_BUS_H_



#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>  // This includes definitions for MCU-specific registers for External Memory Management



#define EXTERNAL_DATA_BUS_ADDRESS_START 0x1000

// NOTE: To make I/O Ports for Microcontroller ATmega162 function as external memory buss, we must initialize it first before we use any of the functionalities
void external_data_bus_init();

void external_data_bus_write(uint16_t address, char data);
char external_data_bus_read(uint16_t address);


#endif /* EXTERNAL_DATA_BUS_H_ */