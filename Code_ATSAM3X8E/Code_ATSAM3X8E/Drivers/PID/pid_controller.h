/*
 * pid_controller.h
 *
 * Created: 10/11/2024 23:11:49
 *  Author: Martynas
 */ 


#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_



// Libraries that make PID Controller work
#include "sam.h"
#include <stdio.h>



// Functions
void pid_controller_init(int8_t kp, int8_t ki, int8_t kd);

int8_t pid_controller_get_u(int8_t reference, int8_t measured_value);



#endif /* PID_CONTROLLER_H_ */