/*
 * adc_driver.c
 *
 * Created: 05/11/2024 02:09:05
 *  Author: Martynas
 */ 



#include "adc_driver.h"



void adc_driver_init() {
	// PIO Setup (START) --------------------------------------------------
	// Before Doing anything with PIO pins, we must enable write permissions to the registers
	// This will allow us to configure our PIO pins in special modes any way we like
	// Like for example setting PIO pins up for Timer mode
	// After disabling write locking we wait and check that the permissions are set as we want
	//
	// WPEN = 0 (Disables the Write Protect if WPKEY corresponds to 0x535343 (“SSC” in ASCII))
	// SSC_WRITE_PROTECT_KEY = 0x535343 (Idiot security to ensure the person who is writing to PWM signals has read the data sheet and understands the consequences of fucking up PWM registers, withouth this key no further PWM action can be made)
	//
	// For more information about write protection registers, read ATSAM3X8E Data Sheet:
	// Page 574 - 674: 30. Synchronous Serial Controller (SSC)
	// Page 616: 30.9.17 SSC Write Protect Mode Register
	// Page 617: 30.9.18 SSC Write Protect Status Register
	uint8_t WPEN = 0;
	SSC->SSC_WPMR = (_SSC_WRITE_PROTECT_KEY << 8) // Set WPKEY
	| WPEN;                       // Set WPEN to disable pin write protect
	
	while (SSC->SSC_WPSR != 0) {
		// Poll until Write Protect Violation Status bit is 0
		// Also wait until bo Write Protect Violation Source emerge
		// (ie No Write Protect Violation has occurred)
	}
	
	// PIO Setup
	// Before using pins for ADC purposes we must configure PIO registers 
	// After disabling PIO registers we double check that and wait until PIO registers are disabled
	// We must specify pin as Input
	// Note that not all pins support ADC Controller mode
	// We will be using PA16, witch connects to AD7 (ADC Controller Channel 7)
	// NOTE: Pin PA16 is Analogue PIN 0 on Arduino DUE (A0)
	//
	// For more information about PWM and PIO registers, read ATSAM3X8E Data Sheet:
	// Page 43: 9.3.3 PIO Controller C Multiplexing
	// Page 618 - 675: 31. Parallel Input/Output Controller (PIO)
	// Page 622: 31.5.2 I/O Line or Peripheral Function Selection
	// Page 622: 31.5.3 Peripheral A or B Selection
	// Page 623: 31.5.4 Output Control
	// Page 634: 31.7.2 PIO Controller PIO Disable Register
	// Page 635: 31.7.3 PIO Controller PIO Status Register
	// Page 656: 31.7.24 PIO Peripheral AB Select Register
	PIOA->PIO_PDR = PIO_PDR_P16;  // Disable PIO control for PA16 (A0)
	PIOA->PIO_ODR = PIO_ODR_P16;  // Ensure the pin is set as input (A0)
	
	while ((PIOA->PIO_PSR & PIO_PSR_P16) != 0) {
		// Wait until PIO control is fully disabled for P23 and P23
		// Loop until PA16 in PIO_PSR is 0, meaning:
		// PIO is inactive
		// BUT the peripheral is active (ie PWM peripheral for us)
	}
	// PIO Setup (STOP) --------------------------------------------------
	
	
	
	// ADC Setup (START) --------------------------------------------------
	// Before ADC can be configured we must disable write protections
	// This will allow us to configure our ADC registers in special modes any way we like
	// After disabling write locking we wait and check that the permissions are set as we want
	//
	// WPEN = 0 (Disables the Write Protect if WPKEY corresponds to  0x414443 (“SSC” in ASCII))
	// ADC_WRITE_PROTECT_KEY =  0x414443 (Idiot security to ensure the person who is writing to ADC have read the data sheet and understands the consequences of fucking up ADC registers, withouth this key no further ADC action can be made)
	//
	// For more information about write protection registers, read ATSAM3X8E Data Sheet:
	// Page 1317 - 1354: 43. Analog-to-Digital Converter (ADC)
	// Page 1353: 43.7.20 ADC Write Protect Mode Register
	// Page 1354: 43.7.21 ADC Write Protect Status Register
	ADC->ADC_WPMR = (_ADC_WRITE_PROTECT_KEY << 8) // Set WPKEY
					| WPEN;                       // Set WPEN to disable pin write protect
	
	while (ADC->ADC_WPSR != 0) {
		// Poll until Write Protect Violation Status bit is 0
		// Also wait until bo Write Protect Violation Source emerge
		// (ie No Write Protect Violation has occurred)
	}
	
	// Reset ADC to ensure it is in the known state
	//
	// For more information about ADC RESET register, read ATSAM3X8E Data Sheet:
	// Page 1317 - 1354: 43. Analog-to-Digital Converter (ADC)
	// Page 1332: 43.7.1 ADC Control Register
	ADC->ADC_CR = ADC_CR_SWRST;  // Reset ADC
	
	// Set ADC Mode
	// We want the most basic of ADC functionality
	// Therefore we will set everything to as default as possible to get data
	// We don't need to get data very fast
	// And so we don't really need to minmax everything here
	// We will keep 12 - bit operation mode as default set
	// We want to use hardware trigger by setting TRGEN field, as it is a reliable way to know when a new data sample has come into the buffer
	// We also need to select type of trigger that will trigger our ADC. We want to use an internal trigger mechanism to set TRGSEL field.
	// Since we want internal trigger, we can select from 3 different Timer Triggers. We will go with Timer Counter channel 0 (TIOA0) as the trigger source
	// We also want to always keep ADC active, so we never go into sleep mode
	// Prescaler Rate Selection for ADC Clock we set as low as we can in order to have as fast sampling as possible, ie ADCClock = 1 MHz
	//		ADCClock = MCK/((PRESCAL + 1) * 2)
	//		2 * ADCClock * (PRESCAL + 1) = MCK
	//		PRESCAL + 1 = MCK/(2 * ADCClock)
	//		PRESCAL = (MCK/(2 * ADCClock)) - 1
	//			MCK = 84 MHz
	//			ADCClock = 1 MHz
	//		PRESCAL = ((84 MHz)/(2 * 1 MHz)) - 1
	//		PRESCAL = 41
	// We also need to give ADC some time to start before starting sampling. Otherwise we might sample wrong or sync out of ADC
	// We give ADC generous 64 ADC Clock cycles
	// We also want a stable voltage when we are in sample mode
	// For this we need to set ADC track time
	// The longer the track time the better, however the slower the ADC will sample
	// We will set track time at a modest 16 ADC CLock cycles
	//		Tracking Time = (TRACKTIM + 1) * ADCClock periods
	//		Tracking Time/ADCClock periods = TRACKTIM + 1
	//		TRACKTIM = (Tracking Time/ADCClock periods) - 1
	//			Tracking Time = 16 ADCClock periods
	//		TRACKTIM = ((16 ADCClock periods)/ADCClock periods) - 1
	//		TRACKTIM = 15
	// We also want to include Transfer Period to let the signal setle down
	// This will again slow ADC down but we are after noise free as possible samples, and not speed
	// We will give ADC 9 ADC CLock cycles to settle the signal
	//		Transfer Period = (TRANSFER * 2 + 3) ADCClock periods
	//		Transfer Period/ADCClock periods = TRANSFER * 2 + 3
	//		(Transfer Period/ADCClock periods) - 3 = 2 * TRANSFER
	//		TRANSFER = ((Transfer Period/ADCClock periods) - 3)/2
	//			Transfer Period = 9 ADCClock periods
	//		TRANSFER = (((9 ADCClock periods)/ADCClock periods) - 3)/2
	//		TRANSFER = 3
	//
	// Bit 0: TRGEN = 1 (Hardware trigger selected by TRGSEL field is enabled)
	// Bit 1 - 3: TRGSEL = 1 (TIOA0 Output of the Timer Counter 0 Channel 0)
	// Bit 4: LOWRES = 0 (12-bit resolution)
	// Bit 5: SLEEP = 0 (Normal Mode: The ADC Core and reference voltage circuitry are kept ON between conversions)
	// Bit 8 - 15: PRESCAL = 41
	// Bit 16 - 19: STARTUP = 4 (64 periods of ADCClock)
	// Bit 24 - 27: TRACKTIM = 15
	// Bit 28 - 29: TRANSFER = 3
	// 
	//
	// For more information about ADC, read ATSAM3X8E Data Sheet:
	// Page 1317 - 1354: 43. Analog-to-Digital Converter (ADC)
	// Page 1322: 43.6.3 Conversion Resolution
	// Page 1325: 43.6.5 Conversion Triggers
	// Page 1329: 43.6.10 ADC Timings
	// Page 1333 - 1335: 43.7.2 ADC Mode Register
	uint8_t ADC_MR_LOWRES_BITS_12 = (0 << 4); // Bit 4: LOWRES = 0 (12-bit resolution)
	ADC->ADC_MR = ADC_MR_TRGEN_EN                 // Enable hardware triggers (TRGEN = 1)
				| ADC_MR_TRGSEL_ADC_TRIG1         // Trigger source TIOA0 (TRGSEL = 1)
				| ADC_MR_LOWRES_BITS_12           // 12-bit resolution (LOWRES = 0)
				| ADC_MR_SLEEP_NORMAL             // Normal mode, ADC Core stays ON (SLEEP = 0)
				| ADC_MR_PRESCAL(41)              // Prescaler setting for ADC clock (PRESCAL = 41)
				| ADC_MR_STARTUP_SUT64            // 64 ADC clock cycles for startup (STARTUP = 4)
				| ADC_MR_TRACKTIM(15)             // Tracking time: 16 ADC clock cycles (TRACKTIM = 15)
				| ADC_MR_TRANSFER(3);             // Transfer time: 9 ADC clock cycles (TRANSFER = 3)
	
	
	// Enable ADC channels
	// We will be using just 1 ADC channel
	// We will be using ADC channel 7 (AD7) as this is ADC Peripheral Cahnnel that PIN PA16 is conected to
	// Pin PA16 is Analogue PIN 0 on Arduino DUE (A0)
	//  
	// For more information about ADC, read ATSAM3X8E Data Sheet:
	// Page 1317 - 1320: 43.5.5 I/O Lines
	// Page 1338: 43.7.5 ADC Channel Enable Register 
	ADC->ADC_CHER = ADC_CHER_CH7;
	// ADC Setup (STOP) --------------------------------------------------
	
	
	
	// ADC Clock Setup (START) --------------------------------------------------
	// Before enabling the PWM Clock we must give permission to write to PMC registers
	// For this we configure the PMC write protect register
	// After that we check and wait that we have actually disabled write protections for PMC registers
	//
	// WPEN = 0 (Disables the Write Protect if WPKEY corresponds to 0x504D43 (“SSC” in ASCII))
	// PMC_WRITE_PROTECT_KEY = 0x504D43 (Idiot security to ensure the person who is writing to PWM signals has read the data sheet and understands the consequences of fucking up PWM registers, withouth this key no further PWM action can be made)
	//
	// For more information about PMC Write Protection, read ATSAM3X8E Data Sheet:
	// Page 526 - 566: 28. Power Management Controller (PMC)
	// Page 561: 28.15.21  PMC Write Protect Mode Register
	// Page 562: 28.15.22  PMC Write Protect Status Register
	PMC->PMC_WPMR = (_PMC_WRITE_PROTECT_KEY << 8) // Set WPKEY
					| WPEN;						  // Set WPEN to disable pin write protect
	
	while (PMC->PMC_WPSR != 0) {
		// Poll until Write Protect Violation Status bit is 0
		// Also wait until field Write Protect Violation Source don't emerge longer
		// (ie No Write Protect Violation has occurred)
	}
	
	// Set up ADC Clock
	// First we need to give power to the ADC Clock, for that we use PMC (Power Management Controller)
	// We need to specify in the register which peripheral clock we want to activate
	// We also set the peripheral clock prescale value from Master clock to peripheral clock
	//
	// NOTE: This will also prescale MCK (Master Clock) for PWM peripheral clock, witch is the clock PWM Controller uses
	//
	// MCK is the system clock speed and can be prescaled just like any other microcontroller
	// In ATSAM3X8E case, the peripherals on the high speed bridge are clocked by MCK
	// HOWEVER On the low-speed bridge, where CAN controller resides, MCK_REAL can be clocked at MCK divided a prescale value
	// This division is set by PMC_PCR register as it is the Peripheral Control Register
	// By setting PMC_PCR we are setting a prescale for MCK_REAL, thus:
	//		prescale = 1
	// NOTE: This is different from CAN buss clock as that clock can only be prescaled to 2 minimum, while PWM Clock can be prescaled to 1 :)
	// This allows us to have 1:1 ratio for MCK = MCK_REAL and will make further calculations easier to do
	//
	// For more information about PMC, read ATSAM3X8E Data Sheet:
	// Page 39: 9.2 APB/AHB Bridge
	// Page 519 - 525: 27. Clock Generator
	// Page 526 - 566: 28. Power Management Controller (PMC)
	// Page 526: 28.2 Embedded Characteristics
	// Page 528 - 529: 28.7 Peripheral Clock Controller
	// Page 538: 28.15 Power Management Controller (PMC) User Interface
	// Page 563: 28.15.23  PMC Peripheral Clock Enable Register 1
	// Page 566: 28.15.26 PMC Peripheral Control Register
	PMC->PMC_PCR = PMC_PCR_EN | PMC_PCR_DIV_PERIPH_DIV_MCK | (ID_ADC << PMC_PCR_PID_Pos);
	PMC->PMC_PCER1 |= 1 << (ID_ADC - 32);
	// ADC Clock Setup (STOP) --------------------------------------------------
	
	
	
	// Timer Setup (START) --------------------------------------------------
	// Before enabling the Timer, we must give permission to write to Timer registers
	// For this we configure the TC write protect register
	// After that we check and wait that we have actually disabled write protections for PMC registers
	//
	// WPEN = 0 (Disables the write protection if WPKEY corresponds to 0x54494D (“TIM” in ASCII))
	// _TC0_WRITE_PROTECT_KEY = 0x54494D (Idiot security to ensure the person who is writing to PWM signals has read the data sheet and understands the consequences of fucking up PWM registers, withouth this key no further PWM action can be made)
	//
	// For more information about Timer Write Protection, read ATSAM3X8E Data Sheet:
	// Page 5908: 36.7.20 TC Write Protection Mode Register
	TC0->TC_WPMR = (_TC0_WRITE_PROTECT_KEY << 8) // Set WPKEY
				   | WPEN;						 // Set WPEN to disable pin write protect
	
	// Enable Peripheral Clock for the Timer
	// We will be configuring Timer Counter 0
	// This is because when we set up ADC_MR register, we said that we will trigger ADC when this triggers:
	//		ADC_MR => TRGSEL = 1 (TIOA0 Output of the Timer Counter 0 Channel 0)
	// We chose Channel 0 on Timer counter 0 as this is the most convenient
	// We want to enable Peripheral Clock Enable Register 0 (PMC_PCER0)
	// This is done through PMC
	//
	// For more information about PMC , read ATSAM3X8E Data Sheet:
	// Page 39: 9.2 APB/AHB Bridge
	// Page 519 - 525: 27. Clock Generator
	// Page 526 - 566: 28. Power Management Controller (PMC)
	// Page 526: 28.2 Embedded Characteristics
	// Page 528 - 529: 28.7 Peripheral Clock Controller
	// Page 538: 28.15 Power Management Controller (PMC) User Interface
	// Page 542: 8.15.4  PMC Peripheral Clock Enable Register 0
	// Page 566: 28.15.26 PMC Peripheral Control Register
	// Page 858 - 859: 36.5 Product Dependencies
	// Page 858 - 859: 36.5.1 I/O Lines
	PMC->PMC_PCER0 |= (1 << ID_TC0); // Enable TC0 clock
	
	// Configure Timer
	// We want to configure trigger in Waveform Mode for TIOA0 output
	// We need to select waveform mode that automatically triggers on a trigger alert
	// This is very important as it is stated in the ADC_MR register that other modes will fail triggering ADC
	// We also select the clock to be prescaled. We want it as fast as possible so we chose the smallest prescale value witch is 2
	// 
	// Bit 0 - 2: TCCLKS = 0 (Clock selected: internal MCK/2 clock signal (from PMC))
	// Bit 13 - 14: WAVSEL = 2 ( UP mode with automatic trigger on RC Compare)
	// Bit 15: WAVE = 1 (Capture mode is disabled (Waveform mode is enabled))
	// Bit 16 - 17: ACPA = 1 (Set)
	// Bit 18 - 19: ACPC = 1 (Clear)
	// 
	// For more information about Timers, read ATSAM3X8E Data Sheet:
	// Page 877: 36.6.14.5 Speed Measurement
	// Page 879: 36.7 Timer Counter (TC) User Interface
	// Page 883 - 886: 36.7.3 TC Channel Mode Register: Waveform Mode
	TC0->TC_CHANNEL[0].TC_CMR =   TC_CMR_WAVE                 // Waveform mode for TIOA trigger
								| TC_CMR_TCCLKS_TIMER_CLOCK1  // MCK/2 as clock source
								| TC_CMR_WAVSEL_UP_RC         // UP mode with automatic trigger on RC compare
								| TC_CMR_ACPA_SET             // RA compare effect on TIOA: Set
								| TC_CMR_ACPC_CLEAR;          // RC compare effect on TIOA: Clear
	
	// Set RA Value
	// Set RA to define the duty cycle. For example, RA = 33,600 for 60% duty cycle
	// Calculate RA for Desired Duty Cycle
	// Desired Duty Cycle (%): 60%
	//
	// RA Calculation Formula:
	// RA = RC * (1 - Duty Cycle (%) / 100)
	// RA = 84,000 * (1 - 60 / 100) = 84,000 * 0.4 = 33,600
	//
	// For more information about TC_RC, read ATSAM3X8E Data Sheet:
	// Page 889: 36.7.6 TC Register A
	TC0->TC_CHANNEL[0].TC_RA = 33600;   // RA value for 60% duty cycle
	
	// Set RC value
	// Adjust RC to set the frequency. For example, RC = 84000 gives 500 Hz frequency
	// Calculate RC for Desired Frequency
	// Timer Clock Frequency (f_TC): MCK / 2 = 84 MHz / 2 = 42 MHz
	//
	// Desired Waveform Frequency (f_desired): 500 Hz
	//
	// RC Calculation Formula:
	// RC = f_TC / f_desired
	// RC = 42,000,000 Hz / 500 Hz = 84,000
	//
	// For more information about TC_RC, read ATSAM3X8E Data Sheet:
	// Page 891: 36.7.8 TC Register C
	TC0->TC_CHANNEL[0].TC_RC = 84000;   // RC value for desired frequency (500 Hz)

	// Start the Timer Counter for triggering
	TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	// Timer Setup (STOP) --------------------------------------------------
	
	
	
	// Enable ADC (START) --------------------------------------------------
	// Begin ADC conversion (will wait for the first trigger from TIOA0)
	ADC->ADC_CR = ADC_CR_START;
	// Enable ADC (STOP) --------------------------------------------------	
}



uint16_t adc_driver_read() {
	// Wait until conversion is complete on channel 7
	while (!(ADC->ADC_ISR & ADC_ISR_EOC7));

	// Read conversion result from Channel 7
	return ADC->ADC_CDR[7];
}