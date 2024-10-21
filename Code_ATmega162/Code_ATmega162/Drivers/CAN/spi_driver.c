/*
 * spi_driver.c
 *
 * Created: 07/10/2024 11:28:57
 *  Author: Martynas
 */ 

#include "spi_driver.h"



// SPI initialize
void spi_driver_init() {
	// Set up data direction for SPI
	DDRB |= (1 << PB4);  // SS as output
	DDRB |= (1 << PB5);  // MOSI as output
	DDRB &= ~(1 << PB6); // MISO as input
	DDRB |= (1 << PB7);  // SCK as output
	
	// Set up SPI register
	// Enable SPI
	// Set as Master
	// Set clock rate to F_CPU/16
	SPCR = (1 << SPE)|(1 << MSTR)|(1 << SPR0);
	// Clear CPOL to 0 (Clock Polarity)
	SPCR &= ~(1 << CPOL);  // Clock idle state is low (0)
	// Clear CPHA to 0 (Clock Phase)
	SPCR &= ~(1 << CPHA);  // Data is sampled on the leading edge (rising edge)
	
	// Set SS-pin high
	PORTB |= (1 << SLAVE_SELECT);
}



// Every time we want to send data, we must select which device we want to send data to
// In our case we only have one Chip we want to communicate with (mcp2515), so in theory we don't need it
// However for redundancy and future proofing if we wanted to have multiple devices, this would be necessary
void spi_driver_select() {
	PORTB &= ~(1 << SLAVE_SELECT);
}

void spi_driver_deselect() {
	PORTB |= (1 << SLAVE_SELECT);
}



// Transmitting data functions
void spi_driver_write(char data) {
	// Start transmission by writing data to SPDR
	SPDR = data;
	
	// Wait for the transmission to complete (SPIF bit set)
	while (!(SPSR & (1 << SPIF)));
}

char spi_driver_read() {
	// Start transmission by writing data to SPDR
	// We write data to SPI so that on the way back we will receive data, so the SPDR register will have shifted data from the other device and thus have the other devices data inside
	// For that we must first send our device data and shift that to the other device
	SPDR = 0x00;
	
	// Wait for the transmission to complete (SPIF bit set)
	while (!(SPSR & (1 << SPIF)));
	
	// Return received data from the slave
	return SPDR;
}