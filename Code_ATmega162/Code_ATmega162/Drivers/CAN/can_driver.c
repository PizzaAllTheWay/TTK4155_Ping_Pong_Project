/*
 * can_driver.c
 *
 * Created: 19/10/2024 22:36:44
 *  Author: Martynas
 */ 



#include "can_driver.h"



// Private Global variables for this driver
uint8_t _is_message_pending = 0;



void can_driver_init(uint8_t mode) {
	// Configure the external interrupt INT1 on PD3 to trigger on the falling edge
	// This is because CAN Controller INT pin goes HIGH at start of received message and LOW when message is fully received
	// We would like to read the message once the whole message is stored in the buffer, ie when INT pin on CAN Controller goes LOW again
	MCUCR |= (1 << ISC11);  // Set INT1 to trigger on the falling edge (ISC11 = 1, ISC10 = 0)
	MCUCR &= ~(1 << ISC10);
	GICR |= (1 << INT1);    // Enable external interrupt INT1 (on PD3)
	
	// Enable global interrupts
	sei();
	
	// Initialize CAN Controller
	mcp2515_driver_init(mode);
}



// Receive Functions ----------
// External service routine for INT1 (PD3)
// Every time CAN Controller receives data, it triggers this external interrupt pin
// Letting us know a message has been sent to us
ISR(INT1_vect){
	_is_message_pending = 1;
}

// Function for checking if the CAN Controller has a pending message for us
uint8_t can_driver_message_available() {
	if (_is_message_pending) {
		_is_message_pending = 0; // Reset message as we now we know its pending once we return the values we pass the pending data on
		return 1; // YES
	}
	else {
		return 0; // NO
	}
}

void can_driver_read_message(can_message_t* message) {
	// Read CANINTF to check if a message was received in RX0
	uint8_t interrupt_flag = mcp2515_driver_read(MCP_CANINTF);
	uint8_t message_available_RX0 = (interrupt_flag & MCP_RX0IF);
	
	if (message_available_RX0) {
		// Get Sender ID ----------
		uint8_t can_id_sender_msb = mcp2515_driver_read(MCP_RXB0SIDH);
		uint8_t can_id_sender_lsb = mcp2515_driver_read(MCP_RXB0SIDL);

		// Apply Mask for the first 3 bits (0x7FF ie 0x700) to ensure it's 11 bits total for the CAN ID
		// CAN ID is 11 bits long, so we keep the lower 3 bits of SIDL (SIDL has the lower 3 bits of the ID)
		message->id = ((can_id_sender_msb << 8) | (can_id_sender_lsb)) & 0x7FF;  // 11-bit CAN ID

		// Get Message Length ----------
		uint8_t can_message_length = mcp2515_driver_read(MCP_RXB0DLC) & 0x0F;  // Only lower 4 bits are used for length
		message->length = can_message_length;

		// Get Message Data ----------
		for (uint8_t i = 0; i < can_message_length; i++) {
			// Read Register of data
			// There are multiple registers of data in CAN Controller
			// Read as many as there is data in
			message->data[i] = mcp2515_driver_read(MCP_RXB0DM + i);  // Read data from RX buffer
		}
	} 
	else {
		// If no message is available, set length to 0
		message->length = 0;
		
		// End with null terminator
		message->data[0] = '\0';
	}

	// Clear interrupt flag for RX0 after reading the message
	mcp2515_driver_bit_modify(MCP_CANINTF, MCP_RX0IF, 0);	
}


// Send Functions ----------
int _can_driver_transmition_complete() {
	// Read the CAN interrupt flag register
	uint8_t transmit_flag = mcp2515_driver_read(MCP_CANINTF);
	
	// Check if TXB0 interrupt flag is set (meaning transmission is complete)
	if (transmit_flag & MCP_TX0IF) {
		// Clear the TX0IF flag to allow further transmissions
		mcp2515_driver_bit_modify(MCP_CANINTF, MCP_TX0IF, 0);
		return 0; // Transmission complete
	}
	
	return 1; // Still transmitting, wait before sending new data
}

void can_driver_send_message(can_message_t* message) {
	// Wait until TXB0 is free (transmission complete) ----------
	while (!_can_driver_transmition_complete()) {
		// Busy-wait until TXB0 is ready
		_delay_ms(1);
	}

	// Send to CAN Buss the Message ID/Who should receive the message ID ----------
	// Since CAN ID is up to 11 digits long, we need to split it into most and least significant bit (msb and lsb)
	// msb needs masking as only the 3 first most significant bits are in the 11 bit range
	uint8_t can_id_msb = (message->id >> 8) & 0x07;
	uint8_t can_id_lsb = (message->id & 0xFF);

	// Send CAN ID of the recipient that is going to receive message
	// Send this data to the CAN Controller in the specific registers for that job
	mcp2515_driver_write(MCP_TXB0SIDH, can_id_msb);
	mcp2515_driver_write(MCP_TXB0SIDL, can_id_lsb);

	// Set data length of message we want to send ----------
	uint8_t can_message_length = message->length;
	mcp2515_driver_write(MCP_TXB0DLC, can_message_length);
	
	// Write Message to CAN Controller ----------
	for (uint8_t i = 0; i < can_message_length; i++) {
		// Since there are multiple registers on CAN Controller for data bytes
		// In total 8 registers where we can store data we would like to send
		// Loop through data buffer and store all the data in CAN Controller
		mcp2515_driver_write(MCP_TXB0Dm + i, message->data[i]);
	}

	// Request to send the message
	mcp2515_driver_request_to_send(MCP_RTS_TX0);
}
