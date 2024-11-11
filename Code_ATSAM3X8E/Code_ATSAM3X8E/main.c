/*
 * Code_ATSAM3X8E.c
 *
 * Created: 26/10/2024 14:43:32
 * Author : Martynas
 */ 



#define F_CPU 84000000UL // Set the CPU clock frequency 84 MHz



// Necessary Drivers for ATSAM3X8E Microcontrollers to work properly
#include "sam.h"
#include <stdio.h>
#include <stdint.h>



// Custom Drivers
#include "Drivers/Time/time.h"
#include "Drivers/Debugging/debug_led.h"
#include "Drivers/UART/uart_driver.h"
#include "Drivers/CAN/can.h"
#include "Drivers/Servo/servo_driver.h"
#include "Drivers/IR_LED/ir_led_driver.h"
#include "Drivers/Solenoid/solenoid_driver.h"
#include "Drivers/Motor/motor_driver.h"
#include "Drivers/Encoder/encoder_driver.h"
#include "Drivers/PID/pid_controller.h"



// Global Constant Variables
#define SOLENOID_BOUNCE_DURATION msecs(100)  // 100  [ms]
#define PID_UPDATE_INTERVAL      msecs(50)   // 50   [ms] TIPS: Should be the same as _MOTOR_UPDATE_INTERVAL
#define BALL_INTERVAL            msecs(5000) // 5000 [ms]
#define CAN_SEND_INTERVAL        msecs(1000) // 1000 [ms]
#define HEARTS_START_AMOUNT      1           // (Value MUST BE: 1 - 9) How many chances we have at the game before its GAME OVER X-X
#define HEARTS_RESET_KEY	     'R'		 // KEY value to reset heart



// Global Variables
uint8_t hearts = HEARTS_START_AMOUNT;

int8_t controls_joystick_y = 0;
int8_t controls_joystick_x = 0;
int8_t controls_pad_left = 0;
int8_t controls_pad_right = 0;
int8_t controls_joystick_button = 0;
int8_t controls_pad_left_button = 0;
int8_t controls_pad_right_button = 0;
char hearts_key = 0x00; // Responsible for reseting hearts (KEY: 'R' ie 0x52) other key values get ignored

uint8_t bounce_state = 0;

int8_t racket_speed = 0;



int main(void) {
	// Disable Watchdog Timer ----------
	WDT->WDT_MR = WDT_MR_WDDIS; // Set WDDIS bit to disable the watchdog timer
	
    // Initialize SAM system ----------
    SystemInit();
	
	// Initialize Debugging ----------
	debug_led_init();
	uart_init(84000000, 9600); // Initialize UART with CPU frequency (84 MHz) and desired baud rate (9600)
	
	// Setup Timers ----------
	// Variables to keep track of timers
	// SOLENOID:
	// For solenoid we add a timer just because we want a de-bounce effect
	// Basically timer is here to count until solenoid can retract
	//
	// PID:
	// There is no point in updating PID every clock cycle because of 3 reasons
	// PID doesn't get new reference and measured values each cycle (The processor is faster than peripheral interfaces)
	// PID controls Motors, witch itself has a strict limit and timer that doesn't allow it to update physically that fast
	// PID takes precious time to compute, no point in wasting time if it will give same result multiple times
	//
	// BALL:
	// The ping pong ball bounces, This can give false hearts subtractions
	// To mitigate this we have a small timer that starts counting after first heart got subtracted
	// Once the timer runs out only then can ping pong again have another heart subtraction if the ping pong is still there
	// 
	// CAN:
	// The CAN sending timings, as to not overwhelm the CAN buss and let us do other tasks at the same time
	// The Controls timings, as there is no point in sending data every microsecond
	uint64_t start_time_solenoid = 0;
	uint64_t start_time_pid = 0;
	uint64_t start_time_ball = 0;
	uint64_t start_time_can = 0;
	
	// Initialize CAN ----------
	// 500 kbps CAN Buss
	// CAN BIT TIMINGS:
	//	 - BRP = 83
	// 	 - PHASE1 = 6
	//   - PHASE2 = 6
	//   - PROP = 3
	//   - SJW = 1
	//   - SMP = 0
	// All of these timings MUST correspond to Node1 timings as well as adhere to ATSAM3X8E CAN Controller specifications
	// EXEPT for BRP! as BRP is the Baud Rate Prescaler
	// ATSAM3X8E CAN Controller specifications:
	//		- Bit Rate must be in standard TQ units (Time Quantum), and must be between 8-25 TQ
	//		- SJW must lie in 1 TQ range
	//		- PROP must be 1-8 TQ
	//		- PHASE1 must be 1-8 TQ
	//		- IPT <= PAHSE2 <= PHASE1
	//		- IPT (Information Processing Time) is fixed at 2 TQ and starts at the sample point
	//		- IPT represents the time needed by the CAN controller logic to process and determine the bit level (whether it’s a ‘1’ or ‘0’) at each sample point
	//		- SJW can only be set up to the minimum of PHASE1 or 4 TQ - whichever is smaller defines the maximum value that SJW can take.
	//
	// bit-rate we chose for node1 must be same for node2, ie 16 TQ (ie 500 kbps in our case)
	//		SJW + PROP + PHASE1 + PHASE2 = 16 TQ
	//
	// BRP is the value that gives identical TQ (Time Quantum) to the Node1 TQ
	// Even though Node1 (16 MHz) has different Frequency to Node2 (84 MHz)
	// Since we adjust BRP so that TQ stays the same
	// Then that means data rate on the buss will stay the same
	//
	// Solving for BRP:
	//		TQ = (1 + BRP)/MCK_REAL
	//			TQ: Time Quantum
	//			MCK_REAL: REAL Clock Speed
	//			BRP: Baud Rate Prescaling
	//		TQ * MCK_REAL = 1 + BRP
	//		BRP = TQ * MCK_REAL - 1
	//		MCK_REAL = MCK * prescale
	// MCK is the system clock speed and can be prescaled just like any other microcontroller
	// In ATSAM3X8E case, the peripherals on the high speed bridge are clocked by MCK
	// HOWEVER On the low-speed bridge, where CAN controller resides, MCK_REAL can be clocked at MCK divided by 2 or 4, this is to save power.
	// This division is set by PMC_PCR register as it is the Peripheral Control Register
	// By setting PMC_PCR we are setting a prescale for MCK_REAL, thus:
	//		prescale = 1/2
	// For more information about MCK (Master Clock), read ATSAM3X8E Data Sheet:
	// Page 39: 9.2 APB/AHB Bridge
	// Page 519 - 525: 27. Clock Generator
	// Page 526 - 566: 28. Power Management Controller (PMC)
	// Page 526: 28.2 Embedded Characteristics
	// Page 528 - 529: 28.7 Peripheral Clock Controller
	// Page 538: 28.15 Power Management Controller (PMC) User Interface
	// Page 566: 28.15.26 PMC Peripheral Control Register
	//		prescale = 1/2
	//		MCK_REAL = MCK * prescale
	//		MCK_REAL = MCK/2
	//		BRP = TQ * MCK_REAL - 1
	//		BRP = ((TQ * MCK)/2) - 1
	// We also have that TQ is time quantum
	//		TQ = 1/bit-rate
	//			bit-rate: data speed on the CAN buss we want
	//			n: Number of time quantums we have per data package sent/received
	//		TQ = 1/500 kbps
	//		BRP = ((TQ * MCK)/2) - 1
	//		BRP = (((1/500 kbps) * MCK)/2) - 1
	//			MCK (Master Clock): Just the external crystal oscillator for the ATSAM3X8E Microcontroller
	//		BRP = (((1/500 kbps) * 84 MHz)/2) - 1
	//		BRP = 83
	//
	// For more information about CAN Buss, read ATSAM3X8E Data Sheet:
	// Page 1185 - 1241: 40. Controller Area Network (CAN)
	// Page 1188 - 1199: 40.7 CAN Controller Features
	// Page 1192 - 1197: 40.7.4 CAN 2.0 Standard Features
	// Page 1192 - 1194: 40.7.4.1 CAN Bit Timing Configuration
	CanInit can_config = {
		.brp = 84,  // Baud Rate Prescaling (83)
		.phase1 = 6,
		.phase2 = 6,
		.propag = 3,
		.sjw = 1,
		.smp = 1
	}; // CAN initialization parameters
	can_init(can_config, 1); // Initialize CAN with the configuration and enable receive interrupts
	
	// Initialize Servo ----------
	servo_driver_init();
	
	// Initialize IR LED ----------
	ir_led_driver_init();
	
	// Initialize Solenoid ----------
	solenoid_driver_init();
	
	// Initialize Motor Control ----------
	int8_t kp = 10; // Proportional gain: reacts to current error (Recomended: 10)
	int8_t ki = 0; // Integral gain: reacts to accumulated error (Recomended: 0)
	int8_t kd = 0; // Derivative gain: reacts to error rate change (Recomended: 0)
	pid_controller_init(kp, ki, kd); // Initialize PID with gain values
	
	motor_driver_init(); // Motor Driver MUST ALWAYS be before Encoder Init!!! (Because Motor Driver resets the 0 point for the system by going to the edge of the box before encoder starts setting 0 point. Ensuring 0 point always stays on the left side of the box for optimal control)
	encoder_driver_init(); // ENcoder MUST come AFTER Motor Driver Init!!! (Because Motor Driver resets the 0 point for the system by going to the edge of the box before encoder starts setting 0 point. Ensuring 0 point always stays on the left side of the box for optimal control)
	
	
	
	// Infinite Loop
    while (1) {
		// Game (START) --------------------------------------------------
		// CAN Receive Logic ----------
		// Read CAN Buss
		CanMsg can_message_rx = can_get_latest_message();
		
		// Typecast all the messages into the correct format
		controls_joystick_y       = (int8_t)can_message_rx.byte[0];
		controls_joystick_x       = (int8_t)can_message_rx.byte[1];
		controls_pad_left         = (int8_t)can_message_rx.byte[2];
		controls_pad_right        = (int8_t)can_message_rx.byte[3];
		controls_joystick_button  = (int8_t)can_message_rx.byte[4];
		controls_pad_left_button  = (int8_t)can_message_rx.byte[5];
		controls_pad_right_button = (int8_t)can_message_rx.byte[6];
		hearts_key                =   (char)can_message_rx.byte[7];
		
		// Print all values
		//printf("Joystick Y: %d\n\r", controls_joystick_y);
		//printf("Joystick X: %d\n\r", controls_joystick_x);
		//printf("Pad Left: %d\n\r", controls_pad_left);
		//printf("Pad Right: %d\n\r", controls_pad_right);
		//printf("Joystick Button: %d\n\r", controls_joystick_button);
		//printf("Pad Left Button: %d\n\r", controls_pad_left_button);
		//printf("Pad Right Button: %d\n\r", controls_pad_right_button);
		//printf("Test Data: %c\n\r", test_data);
		
		// Solenoid Control ----------
		// 1. If button is not pressed, turn solenoid off and reset bounce_state.
		// 2. If button is pressed and bounce_state is 0, turn solenoid on, lock bounce_state, and start timer.
		// 3. If bounce duration has passed while holding the button on, turn solenoid off.
		if (controls_pad_right_button == 0) {
			solenoid_driver_off(); // OFF
			bounce_state = 0; // Reset
		}
		else if ((controls_pad_right_button == 1) && (bounce_state == 0)) {
			solenoid_driver_on(); // ON
			bounce_state = 1; // Lock
			start_time_solenoid = time_now(); // Start timer
		}
		else if ((bounce_state == 1) && ((time_now() - start_time_solenoid) > SOLENOID_BOUNCE_DURATION)) {
			solenoid_driver_off(); // OFF
		}
		
		// Angle Control ----------
		servo_driver_set_position(controls_joystick_x);
		
		// Position Control ----------
		// Calculate the optimal speed to get to our wanted position
		// Because PID takes time to calculate, updating it to many times has no physical benefits, therefore use time for something more productive until PID has a reason to be recalculated
		// Only update speed once PID timer has run out
		if ((time_now() - start_time_pid) > PID_UPDATE_INTERVAL) {
			// Reset timer
			start_time_pid = time_now();
			
			// Get wanted position
			int8_t racket_position_desired = controls_pad_right;
			
			// Get Ping-Pong Racket position
			int8_t racket_position = encoder_driver_get_position();
			
			// Calculate speed control
			racket_speed = pid_controller_get_u(racket_position_desired, racket_position);
			
			// Control Motor
			motor_driver_set_speed(racket_speed);
		}
		
		// Ball Detection ----------
		// Subtract hearts by 1 if the ball was detected AND if cool down period has passed
		if (ir_led_driver_get_status() != 0) {
			if ((time_now() - start_time_ball) >= BALL_INTERVAL) {
				hearts -= 1;

				// Reset the timer to start cool down
				start_time_ball = time_now();
			}
		}
		
		// Hearts Logic ----------
		// Check if enough time has passed since the last CAN message was sent
		if ((time_now() - start_time_can) >= CAN_SEND_INTERVAL) {
			// Reset the CAN start time for the next delay
			start_time_can = time_now();
			
			// Define the CAN message
			CanMsg can_message_tx;
			can_message_tx.id = CAN_ID_NODE2; // CAN ID
			can_message_tx.length = 8; // Message length
			can_message_tx.byte[0] = hearts; // Data bytes to send
			can_message_tx.byte[1] = hearts;
			can_message_tx.byte[2] = hearts;
			can_message_tx.byte[3] = hearts;
			can_message_tx.byte[4] = hearts;
			can_message_tx.byte[5] = hearts;
			can_message_tx.byte[6] = hearts;
			can_message_tx.byte[7] = hearts;
			
			// Send the message on the CAN bus
			can_tx(can_message_tx);
		}
		
		// Check if the reset hearts key was activated
		// If so reset the hearts
		if (hearts_key == HEARTS_RESET_KEY) hearts = HEARTS_START_AMOUNT;
		// Game (STOP) --------------------------------------------------
    }
}
