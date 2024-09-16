/*
 * controls.h
 *
 * Created: 16/09/2024 20:25:04
 *  Author: Martynas
 */ 


#ifndef CONTROLS_H_
#define CONTROLS_H_



// Basic IO pin convention for when configuring MCU to Input pins for buttons
#include <avr/io.h> 



// ADC Is the one that takes in the more complex movement data
#include "../ADC/adc.h"



// Variables used inside controls setup
#define JOYSTIC_CALIBRATE_SAMPLES 100 // How many samples to calibrate the joystick to its center
#define JOYSTIC_DEADZONE 30 // +-30 is Dead zone for where joystick returns 0 instead of its true value



void controls_init();

void controls_refresh();

float controls_get_joystick_x();
float controls_get_joystick_y();
float controls_get_pad_left();
float controls_get_pad_right();
uint16_t controls_get_joystick_button();
uint16_t controls_get_pad_left_button();
uint16_t controls_get_pad_right_button();



#endif /* CONTROLS_H_ */