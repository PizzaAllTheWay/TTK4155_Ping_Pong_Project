/*
 * pwm_driver.h
 *
 * Created: 04/11/2024 11:43:35
 *  Author: Martynas
 */ 


#ifndef PWM_DRIVER_H_
#define PWM_DRIVER_H_



// Libraries 
#include <sam3xa.h> // Ensure PWM library inclusion for SAM ARM M3 Cortex Microcontrollers



// Global important values used in PWM Driver code
#define PWM_DRIVER_MOTOR 0 // Motor Control is connected to PWM Controller Channel 0 as a Normally HIGH output
#define PWM_DRIVER_SERVO 1 // Servo Control is connected to PWM Controller Channel 1 as a Normally HIGH output



// Functions
void pwm_driver_init();

void pwm_driver_set_period(uint8_t pwm_channel, uint32_t period);
void pwm_driver_set_duty_cycle(uint8_t pwm_channel, uint32_t duty_cycle);



#endif /* PWM_DRIVER_H_ */