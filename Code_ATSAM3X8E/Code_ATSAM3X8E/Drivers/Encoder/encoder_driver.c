/*
 * encoder_driver.c
 *
 * Created: 09/11/2024 23:15:36
 *  Author: Martynas
 */ 



#include "encoder_driver.h"



// Internal important values used in Encoder Driver code
#define _SSC_WRITE_PROTECT_KEY 0x535343
#define _PMC_WRITE_PROTECT_KEY 0x504D43

#define _ENCODER_A_PIN PIO_PC25  // Define pin for Channel A (PC25) (D4)
#define _ENCODER_B_PIN PIO_PA29  // Define pin for Channel B (PA29) (D5)

#define _ENCODER_STEP_MAX 1385 // Max step size for the motor box enclosure (Measured experimentally +-20)



// Private Global variables
volatile int32_t encoder_steps = 0;  // Counter to keep track of encoder steps



// Encoder Channel A: Interrupt service routine
void PIOC_Handler(void) {
	// Check if interrupt is from Encoder Channel A
	if ((PIOC->PIO_ISR & _ENCODER_A_PIN) == _ENCODER_A_PIN) {
		// Read the states of encoder channels A and B
		uint8_t encoder_a_state = (PIOC->PIO_PDSR & _ENCODER_A_PIN) ? 1 : 0; // Read current state of Channel A
		uint8_t encoder_b_state = (PIOA->PIO_PDSR & _ENCODER_B_PIN) ? 1 : 0; // Read current state of Channel B

		// Compare the states of A and B
		// If neither condition is met, it will ignore
		if (encoder_a_state == 1 &&  encoder_b_state == 0) {
			encoder_steps++;
		}
		else if (encoder_a_state == 0 &&  encoder_b_state == 1) {
			encoder_steps--;
		}

		// Clear pending interrupt
		NVIC_ClearPendingIRQ(PIOC_IRQn);
	}
}

// Encoder Channel B: Interrupt service routine
void PIOA_Handler(void) {
	// Check if interrupt is from Encoder Channel B
	if ((PIOA->PIO_ISR & _ENCODER_B_PIN) == _ENCODER_B_PIN) {
		// Read the states of encoder channels A and B
		uint8_t encoder_a_state = (PIOC->PIO_PDSR & _ENCODER_A_PIN) ? 1 : 0; // Read current state of Channel A
		uint8_t encoder_b_state = (PIOA->PIO_PDSR & _ENCODER_B_PIN) ? 1 : 0; // Read current state of Channel B

		// Compare the states of A and B
		// If neither condition is met, it will ignore
		if (encoder_a_state == 1 &&  encoder_b_state == 0) {
			encoder_steps++;
		}
		else if (encoder_a_state == 0 &&  encoder_b_state == 1) {
			encoder_steps--;
		}

		// Clear pending interrupt
		NVIC_ClearPendingIRQ(PIOA_IRQn);
	}
}



// Encoder driver initialization
void encoder_driver_init(void) {
	// Enable PIOA and PIOC clocks
	// This is done through PMC to give the clocks power
	// For the PIO clocks to power we need their IDs:
	// PIOA = 11
	// PIOC = 13
	//
	// For more information about PMC, read ATSAM3X8E Data Sheet:
	// Page 38 - 46: 9. Peripherals
	// Page 38 - 39: 9.1 Peripheral Identifiers
	// Page 526 - 573: 28. Power Management Controller (PMC)
	// Page 542: 28.15.4  PMC Peripheral Clock Enable Register 0
	PMC->PMC_PCER0 |= PMC_PCER0_PID13 | PMC_PCER0_PID11;
	
	// Configure Encoder Pins
	// PA29 (ENC_A_PIN) as input
	// PC25 (ENC_B_PIN) as input
	PIOA->PIO_PER |= _ENCODER_A_PIN;    // Enable control of PIO
	PIOA->PIO_ODR |= _ENCODER_A_PIN;    // Configure as input
	
	PIOC->PIO_PER |= _ENCODER_B_PIN;    // Enable control of PIO
	PIOC->PIO_ODR |= _ENCODER_B_PIN;    // Configure as input
	
	// Configure Interrupts 
	// We want to configure interrupts as rising edge on both Encoder channels
	//
	// For more information about Interrupts, read ATSAM3X8E Data Sheet:
	// Page 618 - 675: 31. Parallel Input/Output Controller (PIO)
	// Page 625 - 626: 31.5.10 Input Edge/Level Interrupt
	// Page 627: 31.5.10.4 Falling/Rising Edge or Low/High Level Detection Configuration.
	// Page 631 - 632: 31.7 Parallel Input/Output Controller (PIO) User Interface
	// Page 664: 31.7.32 Additional Interrupt Modes Enable Register
	// Page 671: 31.7.39 Rising Edge/High Level Select Register
	PIOC->PIO_AIMER |= _ENCODER_A_PIN;     // Additional mode for Channel A
	PIOC->PIO_REHLSR |= _ENCODER_A_PIN;    // Rising edge detection for Channel A
	
	PIOA->PIO_AIMER |= _ENCODER_B_PIN;     // Additional mode for Channel B
	PIOA->PIO_REHLSR |= _ENCODER_B_PIN;    // Rising edge detection for Channel B
	
	// Enable Interrupts
	//
	// For more information about Interrupts, read ATSAM3X8E Data Sheet:
	// Page 618 - 675: 31. Parallel Input/Output Controller (PIO)
	// Page 646: 31.7.14 PIO Controller Interrupt Enable Register
	PIOC->PIO_IER = _ENCODER_A_PIN;    // Enable interrupt on Encoder Pin A
	PIOA->PIO_IER = _ENCODER_B_PIN;    // Enable interrupt on Encoder Pin B
	
	// Enable PIOA and PIOC interrupts in the NVIC
	// 
	// For more information about Nested Vectored Interrupt Controller (NVIC), read ATSAM3X8E Data Sheet:
	// Page 47 - 213: 10. ARM Cortex® M3 Processor
	// Page 152: 10.19 About the Cortex-M3 peripherals
	// Page 152 - 164: 10.20 Nested Vectored Interrupt Controller
	NVIC_EnableIRQ(PIOC_IRQn); // Enable interrupt handling on Encoder Pin A
	NVIC_EnableIRQ(PIOA_IRQn); // Enable interrupt handling on Encoder Pin B	
}



int8_t encoder_driver_get_position() {
	// Calculate the position as a percentage of _ENCODER_STEP_MAX
	int8_t encoder_position_percent = (int8_t)((encoder_steps * 100) / _ENCODER_STEP_MAX);

	return encoder_position_percent;
}