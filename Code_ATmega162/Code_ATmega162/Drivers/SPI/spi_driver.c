/*
 * spi_driver.c
 *
 * Created: 07/10/2024 11:28:57
 *  Author: Martynas
 */ 

#include "spi_driver.h"

	//SPCR  SPI control register
	//SPSR  SPI status register
	//SPDR  SPI data register
	//SPIF  SPI interrupt flag
	//CPOL clock polarity
	//CPHA clock phase
	//PRR1/0 and SPI2X are used to specify the clock rate

void spi_init(){
	//set MOSI, SCK and SS as output in DDR
	DDR_SPI |= (1<<DD_MOSI)|(1<<DD_SCK)|(1<<SLAVE_SELECT);
	//enable SPI, set as master
	//Prescaler: Fosc/16, Enable interrupts
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}


void spi_set_mode(uint8_t mode){
	//we want mode 0, 0
	//sample rising edge
	//setup falling edge
	SPCR = (SPCR & ~((1<<CPOL)|(1<<CPHA))) | (mode & 0x03);
}

void spi_set_clock_divider(uint8_t divider){
	SPCR = (SPCR & ~((1<<SPR1)|(1<<SPR0))) | (divider & 0x03);
	SPSR = (SPSR & ~(1<<SPI2X)) | ((divider & 0x04) ? (1<<SPI2X) : 0);
}


// transmit a single byte
void spi_write(uint8_t data){
	// Start transmission 
	SPDR = data;
	// Wait for transmission complete 
	while(!(SPSR & (1<<SPIF)));
}

 //read a single byte
uint8_t spi_master_read(){
	spi_write(0x0);
	return(SPDR);
}

// transfer a single byte
uint8_t spi_transfer(uint8_t data){
	// Start transmission
	SPDR = data;
	//Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));

	//return recieved data
	return(SPDR);
}


// Select SPI device
void spi_select(){
	PORTB &= ~(1<<SLAVE_SELECT);
}

// Deselect SPI device
void spi_deselect(){
	PORTB |= ~(1<<SLAVE_SELECT);
}
