/*
 * spi_driver.h
 *
 * Created: 07/10/2024 11:29:19
 *  Author: Martynas
 */ 


#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_

#include <avr/io.h>
#include <stdio.h>


#define DDR_SPI DDRB
#define SLAVE_SELECT PB4
#define DD_MOSI DDB5
#define DD_MISO DDB6 //not nesecary
#define DD_SCK DDB7

/*
• Request to send
• Bit modify*/

//init <3
void spi_init();

//config functions
void spi_set_mode(uint8_t mode);
void spi_set_clock_divider(uint8_t divider);

//data transfer functions
void spi_write(uint8_t data);
uint8_t spi_read();
uint8_t spi_transfer(uint8_t data);

//chip select functions
void spi_select();
void spi_deselect();


#endif