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
	// Initialize CAN Controller
	mcp2515_driver_init(mode);
	
	// Set CAN Controller to have Interrupts when it receives data
	mcp2515_driver_write(MCP_CANINTE, MCP_RX_INT);
	
	// Configure the external interrupt INT1 on PD3 to trigger on the falling edge
	// This is because CAN SPI Mode is set to communicate when:
	// CPOL (Clock Polarity): 0 (Falling)
	// CPHA (Clock Phase): 0 (Falling)
	MCUCR |= (1 << ISC11);  // Set INT0 to trigger on the falling edge (ISC11 = 1, ISC10 = 0)
	MCUCR &= ~(1 << ISC10);
	GICR |= (1 << INT1);    // Enable external interrupt INT1 (on PD3)
	
	// Enable global interrupts
	sei();
}



// Receive Functions ----------
// External service routine for INT1 (PD3)
// Every time CAN Controller receives data, it triggers this external interrupt pin
// Letting us know a message has been sent to us
ISR(INT1_vect){
	_is_message_pending = 1;
}

// Function for checking if the CAN Controller has a pending message for us
uint8_t can_driver_is_message_available() {
	if (_is_message_pending) {
		_is_message_pending = 0; // Reset message as we now we know its pending once we return the values we pass the pending data on
		return 1; // YES
	}
	else {
		return 0; // NO
	}
}

// Read message that was sent to CAN Micro controller
void can_driver_read_message(char *buffer) {
	// WARNING!!!
	// char *buffer
	// buffer MUST be more or equal to 8+1=9 bytes!!!
	// This is because CAN Controller can store up to 8 bytes of data
	// +1 byte at the end for null termination
	
	// Check interrupt flags
	// Check specifically the RX0, if that pin on CAN Micro Controller has received any interrupts
	// If so, that means it has received some message
	uint8_t interrupt_flag = mcp2515_driver_read(MCP_CANINTF); // Read CAN Micro Controller address for Interrupt Flags
	uint8_t message_available_RX0 = (interrupt_flag & MCP_RX0IF); // Filter out for only RX0 pin On Can Controller so check if that pin has received data and thus an interupt
	
	
	if (message_available_RX0) {
		// Get Sender ID ----------
		// Read the ID of the sender
		uint8_t can_id_sender_msb = mcp2515_driver_read(MCP_RXB0SIDH);
		uint8_t can_id_sender_lsb = mcp2515_driver_read(MCP_RXB0SIDL);
		uint8_t mask = 0x07;

		// Apply Mask for the first 3 bits (0x07) to ensure it's 11 bits total for the CAN ID
		// CAN ID is 11 bits long, so we keep the lower 3 bits of SIDL (SIDL has the lower 3 bits of the ID)
		can_id_sender_msb = can_id_sender_msb & mask;

		// Shift the MSB by 8 bits to the left
		// Then combine the MSB and LSB to form the full 11-bit CAN ID
		uint16_t can_message_id = ((uint16_t)can_id_sender_msb << 8) | can_id_sender_lsb;
		
		// Get Message Length ----------
		uint8_t can_message_length = mcp2515_driver_read(MCP_RXB0DLC);
		
		// Get Message ----------
		for (uint8_t i = 0; i < can_message_length; i++) {
			// Read Register of data
			// There are multiple registers of data in CAN Controller
			// Read as many as there is data in
			buffer[i] = mcp2515_driver_read(MCP_RXB0DM + i);
		}
		
		// End with null terminator
		buffer[can_message_length] = '\n';
	}
	else {
		buffer[0] = '\n'; // No message on RX0, probably message on other RX(x)
	}
	
	// Since we now have read the message
	// We must tell CAN Controller that we have read it
	// And thus we have to reset CAN Controllers Register for Interrupt flag
	// Set CAN Controllers Register for Interrupt flag to 0 on RX0
	// Ie, only reset the first bit (RX0IF = 0)
	mcp2515_driver_bit_modify(MCP_CANINTF, MCP_RX0IF, 0);
	
	// Reset message as we now have read the message
	_is_message_pending = 0;
}



// Send Functions ----------
// Check if TXB0 has completed transmission
int _can_driver_transmition_complete() {
	// Read the CAN interrupt flag register
	uint8_t transmit_flag = mcp2515_driver_read(MCP_CANINTF);
	
	// Check if TXB0 interrupt flag is set (meaning transmission is complete)
	if (transmit_flag & MCP_TX0IF) {
		return 0; // Transmission complete
	}
	
	return 1; // Still transmitting data, you should probably wait with sending new data to CAN buss
}

//
// Send a single byte CAN message with a custom ID
void can_driver_send_message(uint16_t can_id, char* data) {
	// WARNING!!!
	// char* data
	// data WILL ALWAYS be 8 bytes long!!!
	
	// Wait until TXB0 is free (transmission is complete) ----------
	while (!_can_driver_transmition_complete()) {
		// Busy wait until TXB0 is ready
	}
	
	// Send to CAN Buss the Message ID/Who should receive the message ID ----------
	// Since CAN ID is up to 11 digits long, we need to split it into most and least significant bit (msb and lsb)
	// msb needs masking as only the 3 first most significant bits are in the 11 bit range
	uint8_t can_id_receiver_msb = (can_id >> 8) & 0x07;
	uint8_t can_id_receiver_lsb = can_id & 0xFF;
	
	// Send ID of the recipient that is going to receive message
	// Send this data to the CAN Controller in the specific registers for that job
	mcp2515_driver_write(MCP_TXB0SIDH, can_id_receiver_msb);
	mcp2515_driver_write(MCP_TXB0SIDL, can_id_receiver_lsb);
	
	// Set data length of message we want to send ----------
	uint8_t can_message_length = 8; // Always 8 bytes long data message, no more, no less
	mcp2515_driver_write(MCP_TXB0DLC, can_message_length);
	
	// Write Message to CAN Controller ----------
	for (uint8_t i = 0; i < can_message_length; i++) {
		// Since there are multiple registers on CAN Controller for data bytes
		// In total 8 registers where we can store data we would like to send
		// Loop through data buffer and store all the data in CAN Controller
		mcp2515_driver_write(MCP_TXB0Dm + i, data[i]);
	}
	
	// Request CAN Controller to send the message
	mcp2515_driver_request_to_send(MCP_RTS_TX0);
}
