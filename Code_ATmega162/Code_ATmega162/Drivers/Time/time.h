/*
 * time.h
 *
 * Created: 06/11/2024 21:53:08
 *  Author: Martynas
 */ 


#ifndef TIME_H_
#define TIME_H_



// Libraries for time to work
#include <avr/io.h>
#include <avr/interrupt.h>



// Functions
void time_init();

uint32_t time_get_milliseconds();
uint32_t time_get_seconds();



#endif /* TIME_H_ */