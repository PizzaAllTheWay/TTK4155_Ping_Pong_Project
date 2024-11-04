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



// Important global values used in PWM Driver code
#define SSC_WRITE_PROTECT_KEY 0x535343
#define PWM_WRITE_PROTECT_KEY 0x50574D
#define PMC_WRITE_PROTECT_KEY 0x504D43
#define PWM_CLK_DIVA_FACTOR 84



// Functions
void pwm_driver_init();



#endif /* PWM_DRIVER_H_ */