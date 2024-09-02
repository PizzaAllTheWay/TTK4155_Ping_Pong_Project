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

#include <avr/io.h>
#include <stdio.h>

void uart_init(uint32_t f_osc, unsigned long baud_rate);

void uart_send_byte(char msg);

void uart_send_message(char *msg);

#endif
