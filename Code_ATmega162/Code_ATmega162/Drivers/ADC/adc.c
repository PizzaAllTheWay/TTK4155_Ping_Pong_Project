/*
 * adc.c
 *
 * Created: 15/09/2024 00:56:48
 *  Author: Martynas
 */ 



#include "adc.h"

#include "../UART/uart_driver.h"

// Global variables for ADC data
volatile uint8_t adc_data[ADC_NUM_CHANNELS];  // Array to store data for each channel
volatile uint8_t current_channel = 0;     // Track the current ADC channel being read
volatile uint8_t data_ready = 0;   // Flag to indicate new ADC data is ready



// Function to Initiate a New Conversion 
void _start_new_conversion(void) {
	volatile char* sram_data = (char*) ADC_BASE_ADDR;
	sram_data[0] = 0x00; // Writing to ADC to initiate a new conversion (simulated WR pulse)
}

// Function to initialize the ADC and the clock for the ADC
void adc_init() {
	// Setup External Data buss (START) --------------------------------------------------
	// NOTE: In order for External SRAM, ADC and OLED to function properly this initialization MUST stay here
	// This initialization configures the data and register buss between them
	external_sram_init();
	// Setup External Data buss (STOP) --------------------------------------------------
	


	// Setup External Clock (START) --------------------------------------------------
	// Clear TCCR1A (Timer/Counter1) register
	// Even though we are not using this timer directly
	// This timer registers are used to configure Clock settings for our timer
	// This is why we set it to 0 on bits WGM11 and WGM10
	TCCR1A = 0x00;
	
	// Clear TCCR1B (Timer/Counter1) register
	TCCR1B = 0x00;
	
	// TCCR1B: Bit 7 is ICNCn (Input Capture Noise Canceler)
	// This bit is set to 0 as we don't use ICPn pin for changing timer output or filtering it
	
	// TCCR1B: Bit 6 is ICESn (Input Capture Edge Select)
	// This bit is set to 0 as we don't use ICPn pin for changing timer output or filtering it
	
	// TCCR1B: Bit 5 is reserved for redundancy and future Microcontrollers
	// Set to 0 as we don't use it
	
	// TCCR1B: Bit 4-3 is WGMn3:2 (Waveform Generation Mod)
	// We need to chose CTC mode
	// CTC Mode is (Clear Timer on Compare Match)
	// It is a very precise mode for generating robust precise frequencies that don't change
	// Basically ideal for ADC clock
	//
	// There are 2 basic CTC modes
	// 1. OCR1A mode: Ideal for super precise custom clock frequency that doesn't change
	// 2. ICR1 mode: Basically OCR1A, but you can adjust frequencies whilst running, making dynamic frequencies
	// 
	// We will be using 1. OCR1A mode
	// This is because we will never change clock speed once its set
	// This mode makes it more precise and robust
	// And its easier to configure :)
	//
	// For OCR1A mode we must select:
	// [WGM13, WGM12, WGM11, WGM10]
	// [    0,     1,     0,     0]
	//
	// Bit 4 WGM13 set to 0
	// Bit 3 WGM12 set to 1
	// WGM11 and WGM10 are already 0 as we reset the whole TCCR1A register
	TCCR1B &= ~(1 << WGM13); // WGM13 = 0
	TCCR1B |=  (1 << WGM12); // WGM12 = 1
	
	// TCCR1B: Bit 2-0 is CSn2:0 (Clock Select)
	// These bits select the clock source we want to use for our clock generation
	//
	// There are many different modes
	// However we have a clock that is 4.9 MHz
	// The ADC is rated for 0.5 - 5.0 MHz
	// So we will chose the clk_(I/O)/1 (No prescaling) mode
	// This mode allows us to output clock frequency at the same rate as the external crystal oscillator
	//
	// For no prescale mode we have to set pins:
	// [CS12, CS11, CS10]
	// [   0,    0,    1]
	// 
	// Bit 2 CS12 set to 0
	// Bit 1 CS11 set to 0
	// Bit 0 CS10 set to 1
	TCCR1B &= ~(1 << CS12); // CS12 = 0
	TCCR1B &= ~(1 << CS11); // CS11 = 0
	TCCR1B |=  (1 << CS10); // CS10 = 1
	
	// Set compare output mode to toggle OC1A on compare match mode (CTC)
	// Toggle OC1A (PD5) on Compare Match
	TCCR1A |= (1 << COM1A0);  

	// Reset timer counter
	// Ensure the timer starts from 0
	TCNT1 = 0;  
	
	// Write the compare value before starting the timer
	// 0 Means run as fast as possible close to 1/2 Clock speed
	// If 4.9 MHz clock => This will run at 2.4 MHz
	// However in reality it is janky and so the max we can get is 0.3 MHz
	OCR1A = 0; 
	
	// Set PD5 (OC1A) as output for the clock signal to be fed to the ADC
	// This basically disables PD5 as I/O
	// However it feeds external clock signal and inverts it to be outputted on PD5 (OC1A) Pin
	// By enabling this pin, we are outputting our generated clock signal
	DDRD |= (1 << PD5);	
	// Setup External Clock (STOP) --------------------------------------------------
	
	
	
	// ADC Setup (START) --------------------------------------------------
	// Since we are using Hard - Wired mode for ADC
	// Meaning we selected MODE and V_ss Pins on MAX156 ADC
	// This means we have 8-Channel, Single-Ended, Unipolar Conversion
	// Basically the most basic of conversion types
	// Just write some data to get ADC start sampling
	// Then read data out after a while
	//
	// If we had not set the pins in hard wire mode we would have to do something like this:
	// Initialize ADC control registers
	// volatile uint8_t *adc_ctrl = (volatile uint8_t *) (ADC_BASE_ADDR + 0x00);  // Control register mapped to ADC_BASE_ADDR
	// *adc_ctrl = 0x00;  // Default mode: single-ended input, unipolar operation, etc.
	// ADC Setup (STOP) --------------------------------------------------
	
	
	
	// Setup Sampling (START) --------------------------------------------------
	// Enable global interrupts
	sei();

	// Set PD2 as input to monitor the BUSY signal from the ADC
	DDRD &= ~(1 << PD2);  // Make PD2 an input (BUSY signal from ADC)

	// Configure the external interrupt INT0 on PD2 to trigger on the falling edge
	MCUCR |= (1 << ISC01);  // Set INT0 to trigger on the rising edge (ISC01 = 1, ISC00 = 1)
	MCUCR |= (1 << ISC00);
	GICR |= (1 << INT0);    // Enable external interrupt INT0 (on PD2)
	
	// Begin The sampling
	// Manually start the first conversion
	_start_new_conversion();  // Start the first conversion after initialization
	// Setup Sampling (STOP) --------------------------------------------------
}

// Interrupt Service Routine (ISR) for INT0 (PD2)
// This ISR is triggered when the BUSY signal goes low, indicating the conversion is complete.
ISR(INT0_vect) {	
	// After BUSY goes low, read all ADC channels in sequence
	for (uint8_t current_channel = 0; current_channel < ADC_NUM_CHANNELS; current_channel++) {
		// Read the data for the current channel from ADC
		adc_data[current_channel] = *((volatile uint8_t *) (ADC_BASE_ADDR));  // Store the data in the appropriate channel
	}

	// Set the flag to indicate all channel data is ready
	data_ready = 1;
	
	// Automatically start a new conversion after reading all channels
	_start_new_conversion();  // Initiate a new conversion automatically
}

// Basic function to Check and Get ADC Data
uint8_t get_all_adc_data(uint8_t *channel_data) {
	if (data_ready) {
		data_ready = 0;  // Reset the flag

		// Copy the channel data to the provided array
		for (uint8_t i = 0; i < ADC_NUM_CHANNELS; i++) {
			channel_data[i] = adc_data[i];  // Copy each channel's data
		}
		
		return 1; // Return 1 to indicate that all data is available
	}
	return 0; // Return 0 if data is not ready yet
}