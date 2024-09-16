/*
 * adc.h
 *
 * Created: 15/09/2024 00:57:04
 *  Author: Martynas
 */ 


#ifndef ADC_H_
#define ADC_H_



#include <avr/io.h> // Basic IO pin convention for when configuring MCU to ADC pins
#include <avr/interrupt.h> // For the interrupt service routine to know when the ADC is done sampling and BUSSY pin is free
#include <util/delay.h>  // Include for timing functions



// NOTE: In order for External SRAM, ADC and OLED to function properly this initialization MUST stay here
// This initialization configures the data and register buss between them
#include "../External_SRAM/external_sram.h"



#define ADC_BUSY_PIN PD2      // BUSY pin for ADC (using PD2/INT0 for interrupt handling)
#define ADC_BASE_ADDR 0x1400  // Base address for ADC in external memory
#define ADC_NUM_CHANNELS 4   // Number of ADC channels



void adc_init();
uint8_t get_latest_adc_data(void);



#endif /* ADC_H_ */