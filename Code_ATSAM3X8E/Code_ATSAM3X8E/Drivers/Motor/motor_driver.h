/*
 * motor_driver.h
 *
 * Created: 10/11/2024 20:09:20
 *  Author: Martynas
 */ 


#ifndef MOTOR_DRIVER_H_
#define MOTOR_DRIVER_H_



// Libraries that make Motor Driver work
#include "sam.h"
#include <stdio.h>
#include <stdlib.h>
#include "../Time/time.h"
#include "../PWM/pwm_driver.h"



// Functions
void motor_driver_init();

void motor_driver_set_speed(int8_t speed);



#endif /* MOTOR_DRIVER_H_ */