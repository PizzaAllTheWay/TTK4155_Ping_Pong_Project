/*
 * ir_led_driver.h
 *
 * Created: 05/11/2024 02:07:12
 *  Author: Martynas
 */ 


#ifndef IR_LED_DRIVER_H_
#define IR_LED_DRIVER_H_



// Libraries
#include <stdint.h>
#include "../ADC/adc_driver.h"
#include "../Time/time.h"



// Local Variables used in this code
#define _IR_LED_SAMPLE_NUM 64 // Number of samples each time we read status of IR LED
#define _IR_LED_THRESHOLD 2500 // Threshold value to compare against



// Functions
void ir_led_driver_init();

uint8_t ir_led_driver_get_status();



#endif /* IR_LED_DRIVER_H_ */