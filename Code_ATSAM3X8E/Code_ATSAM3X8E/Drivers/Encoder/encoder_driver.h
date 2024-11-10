/*
 * encoder_driver.h
 *
 * Created: 09/11/2024 23:15:44
 *  Author: Martynas
 */ 


#ifndef ENCODER_DRIVER_H_
#define ENCODER_DRIVER_H_



// Escential libraries to set up Encoder correctlty
#include "sam.h"
#include <stdint.h>



#include "../Time/time.h"



// Functions
void encoder_driver_init();

int8_t encoder_driver_get_position();



#endif /* ENCODER_DRIVER_H_ */