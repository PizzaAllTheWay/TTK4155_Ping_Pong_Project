/*
 * servo_driver.h
 *
 * Created: 05/11/2024 00:59:26
 *  Author: Martynas
 */ 



#ifndef SERVO_DRIVER_H_
#define SERVO_DRIVER_H_



#include "../PWM/pwm_driver.h"



// Internal variables for the servo function
#define _SERVO_POSITION_MIN (-100)
#define _SERVO_POSITION_MAX 100
#define _SERVO_POSITION_DEADZONE_MIN (-10)
#define _SERVO_POSITION_DEADZONE_MAX 10
#define _SERVO_PULSE_MIN 900
#define _SERVO_PULSE_MAX 2100



// Functions
void servo_driver_init();

void servo_driver_set_position(int8_t position);



#endif /* SERVO_DRIVER_H_ */