/*
 * adc_driver.h
 *
 * Created: 05/11/2024 02:09:14
 *  Author: Martynas
 */ 


#ifndef ADC_DRIVER_H_
#define ADC_DRIVER_H_



// Libraries
#include <sam3xa.h> // Ensure ADC library inclusion for SAM ARM M3 Cortex Microcontrollers



// Local Important Variables
#define _ADC_WRITE_PROTECT_KEY 0x414443
#define _SSC_WRITE_PROTECT_KEY 0x535343
#define _PMC_WRITE_PROTECT_KEY 0x504D43
#define _TC0_WRITE_PROTECT_KEY 0x54494D



// Functions
void adc_driver_init();

uint16_t adc_driver_read();



#endif /* ADC_DRIVER_H_ */