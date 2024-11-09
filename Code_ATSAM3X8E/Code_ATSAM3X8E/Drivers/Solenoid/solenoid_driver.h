/*
 * solenoid_driver.h
 *
 * Created: 09/11/2024 20:56:11
 *  Author: Martynas
 */ 


#ifndef SOLENOID_DRIVER_H_
#define SOLENOID_DRIVER_H_



// Must have for basic functionalities
#include "sam.h"



// Functions
void solenoid_driver_init();

void solenoid_driver_on();
void solenoid_driver_off();



#endif /* SOLENOID_DRIVER_H_ */