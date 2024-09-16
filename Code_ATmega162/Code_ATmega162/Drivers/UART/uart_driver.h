/*
 * uart_driver.h
 *
 * Created: 26/08/2024 01:39:58
 *  Author: Martynas
 *
 * UART Driver for the project to send basic data back and forth for debugging purposes :) 
 */ 

#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_

#include <avr/io.h> // This includes definitions for MCU-specific registers for UART Communication
#include <stdio.h>

#include <util/delay.h>



// NOTE: To make I/O Ports for Microcontroller ATmega162 function as UART, we must initialize it first before we use any of the functionalities
void uart_init(uint32_t f_osc, unsigned long baud_rate);

void uart_send_byte(char msg);

void uart_send_message(char *msg);

void uart_receive_message(char *buffer, uint8_t max_length);

#endif
