/*
 * pwm_driver.c
 *
 * Created: 04/11/2024 11:43:23
 *  Author: Martynas
 */ 



#include "pwm_driver.h"



// Internal macros used inside this file disable
void _pwm_driver_disable() {
	// Disable PWM before configuring PWM registers
	// disable all 8 channels for PWM
	// Check and wait until all channels are disabled
	//
	// For more information about PWM registers, read ATSAM3X8E Data Sheet:
	// Page 970 - 1052: 38. Pulse Width Modulation (PWM)
	// Page 976 - 1002: 38.6 Functional Description
	// Page 1008: 38.7.3 PWM Disable Register
	// Page 1009: 38.7.4 PWM Status Register
	PWM->PWM_DIS =   PWM_DIS_CHID0
				   | PWM_DIS_CHID1
				   | PWM_DIS_CHID2
				   | PWM_DIS_CHID3
				   | PWM_DIS_CHID4
			       | PWM_DIS_CHID5
				   | PWM_DIS_CHID6
				   | PWM_DIS_CHID7;
	
	while (PWM->PWM_SR & ( PWM_SR_CHID0
						 | PWM_SR_CHID1
						 | PWM_SR_CHID2
						 | PWM_SR_CHID3
						 | PWM_SR_CHID4
						 | PWM_SR_CHID5
						 | PWM_SR_CHID6
						 | PWM_SR_CHID7)) {
		// Poll until CHID0-7 in PWM_SR are all 0 (indicating all channels are disabled)
	}
}

void _pwm_driver_enable() {
	// Enable PWM
	// Now that setup is done we can enable PWM signal again
	// We will enable only PWM channel 0 and 1, as that is the the only channel we are using
	// Because Pin PB12 and PB13 are connected to PWMH0 and PWMH1 (PWM Controller Channel 0 and 1)
	//
	// For more information about PWM_CPRDx and PWM_CDTYx, read ATSAM3X8E Data Sheet:
	// Page 977 - 992: 38.6.2 PWM Channel
	// Page 1007: 38.7.2 PWM Enable Register
	PWM->PWM_ENA =   PWM_ENA_CHID0
				   | PWM_ENA_CHID1;
}



// Setting up PWM for Motor Shield:
// DB12 (D20/SDA) (PWMH0) (Motor Control)
// PB13 (D21/SCL) (PWMH1) (Servo Control)
//
// For more information on Servo and Motor Controllers:
// Read: "TTK4155_Motor_Shield"
void pwm_driver_init() {
	// PIO Setup (START) --------------------------------------------------
	// Before Doing anything with PIO pins, we must enable write permissions to the registers
	// This will allow us to configure our PIO pins in special modes any way we like
	// Like for example setting PIO pins up for PWM mode
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
	// Before using pins for PWM purposes we must configure PIO registers properly
	// We must specify the multiplexer mode for the pins to be in PWM mode
	// Since we are specifying the operation mode for multiplexer to be in PWM, we must also disable PIO registers
	// After disabling PIO registers we double check that and wait until PIO registers are disabled
	// Note that not all pins support PWM Controller mode
	// We have a shield connected to our Controller that has already predefined pins for our use cases
	// Servo PWM (Servo Control) and Enable PWM (Motor Control)
	// This means we have to choose specific pins for our operation:
	// We will be using PB12, witch connects to PWMH0 (PWM Controller channel 0) (Motor Control)
	// We will be using PB13, witch connects to PWMH1 (PWM Controller channel 1) (Servo Control)
	// NOTE: PB12 and PB13 is PWMH meaning HIGH. It is naturally at HIGH state unless pulse is sent
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
	// Page 970 - 1052: 38. Pulse Width Modulation (PWM)
	PIOB->PIO_ABSR |= PIO_ABSR_P12; // Multiplex to peripheral function B for PIN12 (D20/SDA)
	PIOB->PIO_ABSR |= PIO_ABSR_P13; // Multiplex to peripheral function B for PIN13 (D21/SCL)
	PIOB->PIO_PDR |= PIO_PDR_P12; // Disable PIO from controlling PIN12 (D20/SDA)
	PIOB->PIO_PDR |= PIO_PDR_P13; // Disable PIO from controlling PIN13 (21/SCL)
	
	while ((PIOB->PIO_PSR & (PIO_PSR_P12 | PIO_PSR_P13)) != 0) {
		// Wait until PIO control is fully disabled for P12 and P13
		// Loop until PB12 and PB13 in PIO_PSR is 0, meaning:
		// PIO is inactive
		// BUT the peripheral is active (ie PWM peripheral for us)
	}
	// PIO Setup (STOP) --------------------------------------------------
	
	
	
	// PWM Setup (START) --------------------------------------------------
	// Before doing anything with PWM we must ensure WPSWSx and WPHWSx registers are cleared
	// This is to ensure we are able to write and read from PWM Registers
	// We write to WPCMD and WPRGx fields to disable write protections for PWM registers
	// This register has a key that is 0x50574D, by activating it we unlock the ability to write to all PWM registers
	// Then wait until all Write Protect SW and HW statuses are cleared
	//
	// bit 0 - 1: WPCMD = 0 (Disable the Write Protect SW of the register groups of which the bit WPRGx is at 1)
	// bit 2: WPRG0 = 1 (Enables WPCMD field setting to take effect on this WPRGx)
	// bit 3: WPRG1 = 1 (Enables WPCMD field setting to take effect on this WPRGx)
	// bit 4: WPRG2 = 1 (Enables WPCMD field setting to take effect on this WPRGx)
	// bit 5: WPRG3 = 1 (Enables WPCMD field setting to take effect on this WPRGx)
	// bit 6: WPRG4 = 1 (Enables WPCMD field setting to take effect on this WPRGx)
	// bit 7: WPRG5 = 1 (Enables WPCMD field setting to take effect on this WPRGx)
	// bit 8 - 31: WPKEY = 0x50574D (Idiot security to ensure the person who is writing to PWM signals has read the data sheet and understands the consequences of fucking up PWM registers, withouth this key no further PWM action can be made)
	//
	// For more information about WPSWSx, read ATSAM3X8E Data Sheet:
	// Page 970 - 1052: 38. Pulse Width Modulation (PWM)
	// Page 976 - 1002: 38.6 Functional Description
	// Page 996 - 1002: 38.6.5 PWM Controller Operations
	// Page 1001 - 1002: 38.6.5.7 Write Protect Registers
	// Page 1037 - 1038: 38.7.31 PWM Write Protect Control Register
	// Page 1039: 38.7.31 PWM Write Protect Status Register
	PWM->PWM_WPCR = (_PWM_WRITE_PROTECT_KEY << 8) // Set WPKEY
					| PWM_WPCR_WPCMD(0)           // Set WPCMD to disable SW write protect
					| PWM_WPCR_WPRG0              // Enable for group 1
					| PWM_WPCR_WPRG1              // Enable for group 1
					| PWM_WPCR_WPRG2              // Enable for group 2
					| PWM_WPCR_WPRG3              // Enable for group 3
					| PWM_WPCR_WPRG4              // Enable for group 4
					| PWM_WPCR_WPRG5;             // Enable for group 5
					
	while ((PWM->PWM_WPSR & 0xFF) != 0) {
		// Poll until WPSWS0-5 and WPHWS0-5 are cleared
		// WPSR bits 0-5 (WPSWSx) and bits 8-13 (WPHWSx) should all be 0
	}
	
	_pwm_driver_disable();
	// PWM Setup (STOP) --------------------------------------------------
	
	
	
	// PWM Clock Setup (START) --------------------------------------------------
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
	
	// Set up PWM Clock
	// First we need to give power to the PWM Clock, for that we use PMC (Power Management Controller)
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
	// For more information about MCK (Master Clock), read ATSAM3X8E Data Sheet:
	// Page 39: 9.2 APB/AHB Bridge
	// Page 519 - 525: 27. Clock Generator
	// Page 526 - 566: 28. Power Management Controller (PMC)
	// Page 526: 28.2 Embedded Characteristics
	// Page 528 - 529: 28.7 Peripheral Clock Controller
	// Page 538: 28.15 Power Management Controller (PMC) User Interface
	// Page 563: 28.15.23  PMC Peripheral Clock Enable Register 1
	// Page 566: 28.15.26 PMC Peripheral Control Register
	PMC->PMC_PCR = PMC_PCR_EN | PMC_PCR_DIV_PERIPH_DIV_MCK | (ID_PWM << PMC_PCR_PID_Pos);
	PMC->PMC_PCER1 |= 1 << (ID_PWM - 32);
	
	// Set up PWM Clock
	// PWM Controller has 2 Clocks, PWM Clock A and PWM Clock B
	// We will be using PWM Clock A
	//
	// For more information about PWM Clock, read ATSAM3X8E Data Sheet:
	// Page 970 - 1052: 38. Pulse Width Modulation (PWM)
	// Page 976 - 1002: 38.6 Functional Description
	// Page 977 - 992: 38.6.2 PWM Channel
	// Page 978 - 979: 38.6.2.2 Comparator
	// Page 996 - 1002: 38.6.5 PWM Controller Operations
	
	// We need to first setup 2 things in PWM Clock generator
	//		1. CLKA, CLKB Divide Factor
	//		2. CLKA, CLKB Source Clock Selection 
	// We want CLKA to be turned on, so Divide Factor != 0
	// We also want to have an easy way to prescale clock speed, therefore we set Source Clock Selection PREA = 0
	// Now we have a nice PWM_CLK = MCK_REAL/Divide Factor
	// We want PWM_CLK = 1 MHz (REASON: Easy value to do maths with :P)
	// Divide Factor = 84
	// PWM_CLK = 84 MHz/84 = 1 MHz
	//
	// For more information about PWM_CLK, read ATSAM3X8E Data Sheet:
	// Page 1006: 38.7.1 PWM Clock Register
	PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(_PWM_CLK_DIVA_FACTOR);
	  
	// We need to also configure 3 things for the clock setup:
	//		1. Activate clock CPRE field in PWM_CMRx register for PWM
	//		2. Waveform polarity CPOL field in PWM_CMRx register for PWM
	//		3. Waveform alignment CALG field in PWM_CMRx register for PWM
	// We have 8 PWM channels and so PWM_CMRx where x=[0-7]
	// We will be using PWM_CH_NUM[0], because Pin PB12 is connects to PWMH0 (PWM Controller channel 0) (Motor Control)
	// We will be using PWM_CH_NUM[1], because Pin PB12 is connects to PWMH1 (PWM Controller Channel 1) (Servo Control)
	//
	// We want a standard PWM, which means:
	//		bit 0 - 3: CPRE = 0x0B (0b1011: Use PWM Clock A for frequency)
	//		bit     8: CALG = 0 (The period is left aligned)
	//		bit     9: CPOL = 0 (The OCx output waveform (output from the comparator) starts at a low level)
	//		The rest are for more advanced functionality and we don't care about that so we let them be 0
	// 
	// For more information about PWM_CMRx, read ATSAM3X8E Data Sheet:
	// Page 977 - 992: 38.6.2 PWM Channel
	// Page 978 - 979: 38.6.2.2Comparator
	// Page 1044: 38.7.37 PWM Channel Mode Register
	uint8_t CPRE6 = 0x0B; // 0x0B => 0b1011 (Use PWM Clock A)
	uint8_t CPRE7 = 0x0B; // 0x0B => 0b1011 (Use PWM Clock A)
	uint8_t CALG6 = 0x00;
	uint8_t CALG7 = 0x00;
	uint8_t CPOL6 = 0x00;
	uint8_t CPOL7 = 0x00;
	PWM->PWM_CH_NUM[_PWM_CHANNEL_MOTOR].PWM_CMR = (CPRE6 & 0x0F)  // Set bits 0-3 for CPRE (PWM Clock A)
												  | (CALG6 << 8)  // Set bit 8 for CALG (left-aligned)
												  | (CPOL6 << 9); // Set bit 9 for CPOL (start low)
	PWM->PWM_CH_NUM[_PWM_CHANNEL_SERVO].PWM_CMR = (CPRE7 & 0x0F)  // Set bits 0-3 for CPRE (PWM Clock A)
												  | (CALG7 << 8)  // Set bit 8 for CALG (left-aligned)
												  | (CPOL7 << 9); // Set bit 9 for CPOL (start low)
	
	// In Addition we need to configure 2 things for the clock:
	//		1. Waveform period CPRD field in PWM_CPRDx register for PWM
	//		2. Waveform duty-cycle CDTY field in PWM_CDTYx register for PWM
	// Since we have a very simple PWM
	// We will be using PWM for Servo
	// Servo Requires 20 ms waveform period (50 kHz)
	// Servo also works only in ranges of 0.9 - 2.1 ms duty cycle
	// We will be using PWM_CH_NUM[0], because Pin PB12 is connects to PWMH0 (PWM Controller channel 0) (Motor Control)
	// We will be using PWM_CH_NUM[1], because Pin PB12 is connects to PWMH1 (PWM Controller Channel 1) (Servo Control)
	//
	// Waveform Period:
	// Since we have configured PWM_CMRx PWM to have left aligned wave form
	// And we are using PWM Clock A
	// And PWM Clock A is prescaled using DIVA = PWM_CLK_DIVA_FACTOR = 84 in PWM_CLK register
	// NOTE: in PWM_CLK we set prescale = 1
	// Our formula for Waveform period will become:
	//		waveform period = (CPRD * DIVA)/MCK_REAL
	//			MCK_REAL = MCK * prescale = MCK * 1 = MCK
	//		waveform period = (CPRD * DIVA)/MCK
	//		waveform period * MCK = CPRD * DIVA
	//		CPRD = (waveform period * MCK)/DIVA
	//			waveform period = 20 ms
	//			MCK = 84 MHz
	//			DIVA = 84
	//		CPRD = (20 ms * 84 MHz)/84
	//		CPRD = 20 * (10^3)
	//		CPRD = 20 000
	// Duty Cycle:
	// Now for phase angle, all the Clock prescaling will come in clutch so that we dont have to do a lot of coding for this to work
	// Since we have configured PWM_CMRx PWM to have left aligned wave form
	// Our formula will be:
	//		duty cycle = ((period - 1)/(fchannel_x_clock * CDTY))/period
	//		duty cycle * period = (period - 1)/(fchannel_x_clock * CDTY)
	//		fchannel_x_clock * CDTY = (period - 1)/(duty cycle * period)
	//		CDTY = ((period - 1)/(duty cycle * period))/fchannel_x_clock
	//			period = Waveform Period = 20 000 us
	//			duty cycle = 0 us (ie 0 ms)
	//			fchannel_x_clock = 1 MHz
	// We can also say:
	//		CDTY = CPRD * (1 ? duty cycle)
	//			CPRD = 20 000
	//			duty cycle = 0 us (ie 0 ms)
	//		CDTY = 20 000 * (1 - 0 us)
	//		CDTY = 20 000
	// We are going to set CDTY = 0 because we dont want any duty cycle once we start everything up
	//
	// Bit 0 - 23: CPRD = 20 000
	// Bit 0 - 23: CDTY = 0
	// 
	// For more information about PWM_CPRDx and PWM_CDTYx, read ATSAM3X8E Data Sheet:
	// Page 977 - 992: 38.6.2 PWM Channel
	// Page 978 - 979: 38.6.2.2 Comparator
	// Page 996 - 1002: 38.6.5 PWM Controller Operations
	// Page 996: 38.6.5.1 Initialization
	// Page 1046: 38.7.38 PWM Channel Duty Cycle Register
	// Page 1046: 38.7.38 PWM Channel Duty Cycle Register
	uint32_t CPRD6 = 20000;
	uint32_t CPRD7 = 20000;
	uint32_t CDTY6 = 0;
	uint32_t CDTY7 = 0;
	CPRD6 &= 0xFFFFFF; // Apply mask as only bit 0 - 23 are valid
	CPRD7 &= 0xFFFFFF; // Apply mask as only bit 0 - 23 are valid
	CDTY6 &= 0xFFFFFF; // Apply mask as only bit 0 - 23 are valid
	CDTY7 &= 0xFFFFFF; // Apply mask as only bit 0 - 23 are valid
	PWM->PWM_CH_NUM[_PWM_CHANNEL_MOTOR].PWM_CPRD = PWM_CPRD_CPRD(CPRD6);
	PWM->PWM_CH_NUM[_PWM_CHANNEL_SERVO].PWM_CPRD = PWM_CPRD_CPRD(CPRD7);
	PWM->PWM_CH_NUM[_PWM_CHANNEL_MOTOR].PWM_CDTY = PWM_CPRD_CPRD(CDTY6);
	PWM->PWM_CH_NUM[_PWM_CHANNEL_SERVO].PWM_CDTY = PWM_CPRD_CPRD(CDTY7);
	// PWM Clock Setup (STOP) --------------------------------------------------
	
	
	
	// PWM Enable (START) --------------------------------------------------
	_pwm_driver_enable();
	// PWM Enable (STOP) --------------------------------------------------
}



// Handy functions to manipulate PWM registers without a hazel
void pwm_driver_set_period(uint8_t pwm_channel, uint32_t period) {
	// Arguments:
	//		pwm_channel (Input: 0 - 7)		     [Unit: NONE]
	//		period      (Input: 0 - 16,777,215)  [Unit: micro seconds (us)]
	//
	// Explanation:
	// You will find all what this code does in pwm_driver_init();
	// Basically because of how we set up our PWM Clocks and PWM Mode
	// We don't have to do a lot of math as a lot of things just cancel out
	// This means that we can directly manipulate registers without first recalculating and reconverting the values
	if ((pwm_channel == _PWM_CHANNEL_MOTOR) || (pwm_channel == _PWM_CHANNEL_SERVO)) {
		_pwm_driver_disable();
		
		uint32_t CPRDx = period;
		PWM->PWM_CH_NUM[pwm_channel].PWM_CPRD = PWM_CPRD_CPRD(CPRDx);
		
		_pwm_driver_enable();
	}
}

void pwm_driver_set_duty_cycle(uint8_t pwm_channel, uint32_t duty_cycle) {
	// Arguments:
	//		pwm_channel (Input: 0 - 7)		[Unit: NONE]
	//		duty_cycle  (Input: 0 - CDTYx)  [Unit: micro seconds (us)]
	//
	// Explanation:
	// You will find all what this code does in pwm_driver_init();
	// Basically because of how we set up our PWM Clocks and PWM Mode
	// We don't have to do a lot of math as a lot of things just cancel out
	// This means that we can directly manipulate registers without first recalculating and reconverting the values
	if ((pwm_channel == _PWM_CHANNEL_MOTOR) || (pwm_channel == _PWM_CHANNEL_SERVO)) {
		_pwm_driver_disable();
		
		uint32_t CDTYx = duty_cycle;
		PWM->PWM_CH_NUM[pwm_channel].PWM_CDTY = PWM_CDTY_CDTY(CDTYx);
		
		_pwm_driver_enable();
	}
}

