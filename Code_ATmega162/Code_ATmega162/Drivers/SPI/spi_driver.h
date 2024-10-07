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
#include <util/delay.h>

//init <3
void spi_init();

//config functions
void spi_set_mode(uint8_t mode);
void spi_set_clock_divider(uint8_t divider);

//data transfer functions
void spi_transfer(uint8_t data);
void spi_transfer_buffer(uint8_t *data_out, uint8_t *data_in, size_t length);

//chip select functions
void spi_select_device(uint8_t device);
void spi_deselect_device(uint8_t device);


//error handling function
void spi_get_status();


#endif