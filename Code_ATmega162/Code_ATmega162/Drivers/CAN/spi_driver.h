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



// Global Variables
#define SLAVE_SELECT PB4



// init <3
void spi_driver_init();

void spi_driver_select();
void spi_driver_deselect();

void spi_driver_write(char data);
char spi_driver_read();



#endif