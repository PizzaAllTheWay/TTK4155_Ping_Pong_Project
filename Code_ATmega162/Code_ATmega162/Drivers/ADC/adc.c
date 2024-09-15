/*
 * adc.c
 *
 * Created: 15/09/2024 00:56:48
 *  Author: Martynas
 */ 



#include "adc.h"



// Global variables for ADC data
volatile uint8_t adc_data[ADC_NUM_CHANNELS];  // Array to store data for each channel
volatile uint8_t current_channel = 0;     // Track the current ADC channel being read
volatile uint8_t data_ready = 0;   // Flag to indicate new ADC data is ready



/*********** Function to Initiate a New Conversion ***********/
void _start_new_conversion(void) {
	*((volatile uint8_t *)(ADC_BASE_ADDR)) = 0x00;  // Writing to ADC to initiate a new conversion (simulated WR pulse)
}

// Function to initialize the ADC and the clock for the ADC
void init_adc_and_clock(void) {
	/*********** Setup ADC BUSY pin as an interrupt source on PD2 (INT0) ***********/
	
	// Set PD2 as input to monitor the BUSY signal from the ADC
	DDRD &= ~(1 << PD2);  // Make PD2 an input (BUSY signal from ADC)
	
	// Configure the external interrupt INT0 on PD2 to trigger on the falling edge
	MCUCR |= (1 << ISC01);  // Set INT0 to trigger on the falling edge (ISC01 = 1, ISC00 = 0)
	GICR |= (1 << INT0);    // Enable external interrupt INT0 (on PD2)
	
	/*********** Setup the ADC control register via external memory ***********/
	
	// The ADC is assumed to be memory-mapped at a base address (e.g., 0x8000)
	// Here, we access the ADC control register to configure its mode and input settings
	// External SRAM initialization is assumed to have been done by another function
	
	// Initialize ADC control registers
	volatile uint8_t *adc_ctrl = (volatile uint8_t *) (ADC_BASE_ADDR + 0x00);  // Control register mapped to ADC_BASE_ADDR
	*adc_ctrl = 0x00;  // Default mode: single-ended input, unipolar operation, etc.
	// Depending on the ADC type and requirements, the control register settings may vary
	
	/*********** Configure Timer1 to generate the clock for the ADC on PD5 (OC1A) ***********/
	
	// Set PD5 (OC1A) as output for the clock signal to be fed to the ADC
	DDRD |= (1 << ADC_CLOCK_PIN);  // Make PD5 an output for clock signal generation

	// Timer1 will be used in CTC (Clear Timer on Compare Match) mode to generate a square wave on PD5
	// The frequency of the clock depends on the system clock (f_osc) and the timer's prescaler and compare match value
	// Timer1 CTC mode: WGM12 = 1 (CTC mode), top value in OCR1A
	TCCR1B |= (1 << WGM12);  // Configure Timer1 for CTC mode

	// Set the clock prescaler to divide the system clock by 8
	// This allows for a more precise control of the output frequency
	TCCR1B |= (1 << CS11);   // Prescaler set to clk/8

	// Set the output compare value to generate the desired clock frequency for the ADC
	// Example: To generate a 1 MHz clock signal from a 16 MHz system clock with clk/8 prescaler
	// Formula: f_clk = f_osc / (2 * prescaler * (OCR1A + 1))
	// For f_clk = 1 MHz, prescaler = 8, and f_osc = 16 MHz -> OCR1A = 1
	OCR1A = 1;  // Set compare value for 1 MHz clock output on PD5 (OC1A)

	// Enable Timer1 output on OC1A (PD5) to toggle the pin on compare match
	// This will generate a square wave at the calculated frequency
	TCCR1A |= (1 << COM1A0);  // Toggle OC1A (PD5) on compare match

	/*********** Enable Global Interrupts ***********/
	
	// Enable global interrupts to allow the INT0 interrupt to be serviced
	// The ISR for INT0 will be triggered whenever the ADC signals that it has completed a conversion (BUSY pin goes low)
	sei();  // Enable global interrupts
	
	// Begin The sampling
	// Manually start the first conversion
	_start_new_conversion();  // Start the first conversion after initialization
}

/*********** Interrupt Service Routine (ISR) for INT0 ***********/
// This ISR is triggered when the BUSY signal goes low, indicating the conversion is complete.
ISR(INT0_vect) {
	// After BUSY goes low, read all ADC channels in sequence
	for (uint8_t current_channel = 0; current_channel < ADC_NUM_CHANNELS; current_channel++) {
		// Ensure timing for RD pulses
		_delay_us(0.050);  // tBRD - BUSY high to RD delay (50ns minimum)

		// Read the data for the current channel from ADC
		adc_data[current_channel] = *((volatile uint8_t *) (ADC_BASE_ADDR));  // Store the data in the appropriate channel

		// Respect timing between consecutive RD pulses
		_delay_us(0.100);  // tRDL - RD low pulse width (100ns minimum)
		_delay_us(0.180); // tRDH - RD high pulse width (180ns minimum)
	}

	// Set the flag to indicate all channel data is ready
	data_ready = 1;
	
	// Automatically start a new conversion after reading all channels
	_start_new_conversion();  // Initiate a new conversion automatically
}

/*********** Function to Check and Get ADC Data ***********/
// This function returns 1 when all channel data is ready, allowing access to the adc_data array
uint8_t get_all_adc_data(uint8_t *channel_data) {
	if (data_ready) {
		data_ready = 0;  // Reset the flag

		// Copy the channel data to the provided array
		for (uint8_t i = 0; i < ADC_NUM_CHANNELS; i++) {
			channel_data[i] = adc_data[i];  // Copy each channel's data
		}
		
		return 1;  // Return 1 to indicate that all data is available
	}
	return 0;  // Return 0 if data is not ready yet
}