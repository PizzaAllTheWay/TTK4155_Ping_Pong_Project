/*
 * can_improved.h
 *
 * Created: 28/10/2024 22:13:39
 *  Author: Martynas
 */ 


#ifndef CAN_IMPROVED_H_
#define CAN_IMPROVED_H_

#pragma once

#include <stdint.h>



// Define mailboxes for transmission and reception
#define TX_MAILBOX 0
#define RX_MAILBOX_0 1
#define RX_MAILBOX_1 2



// Struct with bit timing information
// See `can_init` for usage example
typedef struct CanInit CanInit;
__attribute__((packed)) struct CanInit {
    union {
        struct {
            uint32_t phase2:4;  // Phase 2 segment
            uint32_t propag:4;  // Propagation time segment
            uint32_t phase1:4;  // Phase 1 segment
            uint32_t sjw:4;     // Synchronization jump width
            uint32_t brp:8;     // Baud rate prescaler
            uint32_t smp:8;     // Sampling mode
        };
        uint32_t reg;
    };
};

// Initialize CAN bus, with bit timings and optional interrupt
// If `rxInterrupt` is not 0, an interrupt will be triggered when a message is received in either mailbox.
// Example:
//    can_init((CanInit){.brp = F_CPU/2000000-1, .phase1 = 5, .phase2 = 1, .propag = 6}, 1);
void can_init_improved(CanInit init, uint8_t rxInterrupt);


// Dummy type for use with `union_cast`, see below
typedef struct Byte8 Byte8;
struct Byte8 {
	uint8_t bytes[8];
};

// CAN message data type
// Data fields have 3 access methods (via union):
//  - 8 bytes
//  - 2 double words (32-bit ints)
//  - 1 Byte8 dummy struct
typedef struct CanMsg CanMsg;
struct CanMsg {
    uint8_t id;
    uint8_t length;
    union {
        uint8_t     byte[8];
        uint32_t    dword[2];
        Byte8       byte8;
    };
};

// Send a CAN message on the bus.
// Blocks if the bus does not receive the message (typically because one of the
// receiving nodes has not cleared a buffer).
void can_tx_improved(CanMsg m);

// Receive a CAN message from a specified mailbox.
// Does not block. Returns 0 if there is no message, 1 otherwise.
// `mailbox` argument specifies the mailbox number to read from (e.g., 0 or 1).
uint8_t can_rx_improved(CanMsg* m, uint8_t mailbox);

// Print a CAN message (using `printf`).
void can_printmsg_improved(CanMsg m);



#endif /* CAN_IMPROVED_H_ */