/*
 * can_driver.h
 *
 * Created: 19/10/2024 22:36:58
 *  Author: Martynas
 */ 


#ifndef CAN_DRIVER_H_
#define CAN_DRIVER_H_



#include <avr/io.h> // Basic IO pin convention for when configuring MCU to CAN Controller pins
#include <avr/interrupt.h> // For the interrupt service routine to know when the CAN Controller has pending messages for MCU, when the interupt pin goes low



#include "mcp2515_driver.h"



// Data format
typedef struct {
	uint16_t id;         // CAN ID (11 bits max)
	char data[8];        // CAN data (8 bytes max)
	uint8_t length;      // Length of the CAN message (1-8)
} can_message_t;



// Functions
void can_driver_init(uint8_t mode);

uint8_t can_driver_message_available();
void can_driver_send_message(can_message_t* message);

void can_driver_read_message(can_message_t* message);




#endif /* CAN_DRIVER_H_ */