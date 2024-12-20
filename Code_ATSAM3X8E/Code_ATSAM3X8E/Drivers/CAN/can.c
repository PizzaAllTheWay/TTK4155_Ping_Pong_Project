/*
 * can_improved.c
 *
 * Created: 26/10/2024 18:14:15
 * Author: Martynas
 */



#include "can.h"



// Buffer that all the receive data will be saved to
CanMsg can_rx_message_buffer;



// Utility function to print CAN message details
void can_printmsg(CanMsg m) {
    printf("CanMsg(id:%d, length:%d, data:{", m.id, m.length);
    if (m.length) {
        printf("0x%02X", m.byte[0]);
    }
    for (uint8_t i = 1; i < m.length; i++) {
        printf(", 0x%02X", m.byte[i]);
    }
    printf("})\n\r");
}



// Initialize the CAN controller
void can_init(CanInit init, uint8_t rxInterrupt) {
	// For information about CAN Buss, read ATSAM3X8E Data Sheet:
	// Page 1185 - 1241: 40. Controller Area Network (CAN)
	// Page 1199 - 1210: 40.8 Functional Description
	// Page 1199: 40.8.1 CAN Controller Initialization
	
    // Disable CAN to configure
    CAN0->CAN_MR &= ~CAN_MR_CANEN;

    // Configure GPIO for CAN
    PIOA->PIO_IDR = PIO_PA8A_URXD | PIO_PA9A_UTXD;
    PIOA->PIO_ABSR &= ~(PIO_PA1A_CANRX0 | PIO_PA0A_CANTX0);
    PIOA->PIO_PDR = PIO_PA1A_CANRX0 | PIO_PA0A_CANTX0;
    PIOA->PIO_PUER = PIO_PA1A_CANRX0 | PIO_PA0A_CANTX0;

    // Enable CAN0 clock
	// NOTE: This will also prescale MCK (Master Clock) for peripheral clock, witch is the clock CAN Controller uses
	//
	// MCK is the system clock speed and can be prescaled just like any other microcontroller
	// In ATSAM3X8E case, the peripherals on the high speed bridge are clocked by MCK
	// HOWEVER On the low-speed bridge, where CAN controller resides, MCK_REAL can be clocked at MCK divided by 2 or 4, this is to save power.
	// This division is set by PMC_PCR register as it is the Peripheral Control Register
	// By setting PMC_PCR we are setting a prescale for MCK_REAL, thus:
	//		prescale = 1/2
	//
	// For more information about MCK (Master Clock), read ATSAM3X8E Data Sheet:
	// Page 39: 9.2 APB/AHB Bridge
	// Page 519 - 525: 27. Clock Generator
	// Page 526 - 566: 28. Power Management Controller (PMC)
	// Page 526: 28.2 Embedded Characteristics
	// Page 528 - 529: 28.7 Peripheral Clock Controller
	// Page 538: 28.15 Power Management Controller (PMC) User Interface
	// Page 566: 28.15.26 PMC Peripheral Control Register
    PMC->PMC_PCR = PMC_PCR_EN | PMC_PCR_CMD | (ID_CAN0 << PMC_PCR_PID_Pos);
    PMC->PMC_PCER1 |= (1 << (ID_CAN0 - 32));

    // Set CAN baud rate and timing
    CAN0->CAN_BR = init.reg;

    // Configure TX mailbox (Mailbox 0) for transmission
	// Set Mailbox in transmit mode by setting the MOT bits in CAN_MMR register
	// Also, set the priority of the TX mailbox (CAN_MMR_PRIOR_Pos) to 0 (highest priority)
	// This ensures that this mailbox will have the highest chance of winning the arbitration process
	// (We don't use multiple mailboxes for transmitting so this is overkill, but we just added it :P)
	//
	// For more information CAN TX Setup and ID setup, read ATSAM3X8E Data Sheet:
	// Page 1188: 40.7.2 Mailbox Organization
	// Page 1188: 40.7.2.1 Message Acceptance Procedure
	// Page 1231: 40.9.14 CAN Message Mode Register
    CAN0->CAN_MB[TX_MAILBOX].CAN_MMR = CAN_MMR_MOT_MB_TX | (0 << CAN_MMR_PRIOR_Pos);

    // Configure RX mailboxes for chained reception
	// This is done so that if one has extended CAN data transmission one can still receive the whole message
	// As 1 single mailbox can only hold 8 bytes of data 
	// Whilst extended CAN buss can have 9+ bytes like 16 bytes ect
	// By daisy chaining mailboxes one can receive all the data without worrying of overflow
	// We decided to implement the mailboxes feature but disable it (read "Steps to daisy chain mailboxes to enable it again")
	// Instead we chose to use 1 single mailbox as we expect to get NO extended CAN messages from Node 1, only 8 bytes max
	// Instead we use overwriting mode on the first mailbox
	// This way if microcontroller still processing data from first data one receives, and the CAN buss goes to fast, we don't lose the next message
	// Instead, the old mailbox 1 message that is still being processed by microcontroller, will be overwritten
	// This way when microcontroller finishes processing the first CAN message, it can immediately process the next message as it has been buffered to the mailbox 1 already
	// This way we never let any message escape even if CAN data buss is to fast for the controller, as all the data will be overwritten with the new data
	//
	// NOTE: CAN Controller inside the ARM Controller has up to 8 Mailboxes you can daisy chain
	//
	// Steps to daisy chain mailboxes:
	// 1. Configure the first mailbox in Receive Mode to accept the first part of the message.
	//		Example: CAN0->CAN_MB[RX_MAILBOX_0].CAN_MMR = CAN_MMR_MOT_MB_RX;  
	// 2. Set subsequent mailboxes (except the last) in Receive Mode, each set to receive the next parts.
	//		Example: CAN0->CAN_MB[RX_MAILBOX_1].CAN_MMR = CAN_MMR_MOT_MB_RX; 
	// 3. Configure the last mailbox in the chain to Receive with Overwrite Mode to capture remaining data.
	//		Example: CAN0->CAN_MB[RX_MAILBOX_1].CAN_MMR = CAN_MMR_MOT_MB_RX_OVERWRITE;
	// NOTE: The first mailbox in the list will receive messages first, and so forth until the last. This is done automatically by the CAN Controller inside the ATSAM3X8E Microcontroller
	//	
	// Steps to configure Receive with Overwrite Mode:
	// 1. Configure the mailbox for Receive with Overwrite Mode by setting the MOT field in the CAN_MMRx register.
	//      Example: CAN0->CAN_MB[RX_MAILBOX].CAN_MMR = CAN_MMR_MOT_MB_RX_OVERWRITE;
	// 2. Set the Message ID and Message Acceptance Mask before enabling Receive Mode, to specify which messages to accept.
	//      Example:
	//      CAN0->CAN_MB[RX_MAILBOX].CAN_MID = MESSAGE_ID;
	//      CAN0->CAN_MB[RX_MAILBOX].CAN_MAM = ACCEPTANCE_MASK;
	// 3. After enabling Receive Mode, the MRDY flag in CAN_MSR will be cleared until the first message is received.
	//      - Once a message is received, MRDY is automatically set, indicating the mailbox is ready, and an interrupt is generated if enabled.
	// 4. If a new message arrives while MRDY is still set, it will overwrite the current message in the mailbox.
	//      - The MMI flag in CAN_MSRx notifies the application that a message was overwritten. This flag clears upon reading CAN_MSRx.
	// 5. To ensure data integrity when reading the mailbox, check the MMI flag in CAN_MSRx before and after reading CAN_MDHx and CAN_MDLx.
	//      - If MMI is set again after reading, re-read CAN_MDHx and CAN_MDLx to capture the latest message.
	//      - Example:
	//      if (CAN0->CAN_MB[RX_MAILBOX].CAN_MSR & CAN_MSR_MMI) { <handle message loss> }
	// NOTE: If the CAN controller overwrites the mailbox data while the microcontroller is reading it,
	// the application should check MMI to avoid handling inconsistent data across registers.
	//	
	// For more information on mailboxes, daisy chaining, multi mailboxes and overwriting, read ATSAM3X8E Data Sheet:
	// Page 1185 - 1241: 40. Controller Area Network (CAN)
	// Page 1199 - 1210: 40.8 Functional Description
	// Page 1201 - 1207: 40.8.3 CAN Controller Message Handling
	// Page 1202: Receive with Overwrite Mailbox
	// Page 1202 - 1203: Chaining Mailboxes
	
    // RX_MAILBOX_0: First mailbox in chain (Receive Mode) + Overwrite Mode
    CAN0->CAN_MB[RX_MAILBOX_0].CAN_MAM = 0; // Accept all messages
    CAN0->CAN_MB[RX_MAILBOX_0].CAN_MID = CAN_MID_MIDE;
    CAN0->CAN_MB[RX_MAILBOX_0].CAN_MMR = CAN_MMR_MOT_MB_RX_OVERWRITE; // Overwrite Mode
    CAN0->CAN_MB[RX_MAILBOX_0].CAN_MCR |= CAN_MCR_MTCR; // Mark as ready to receive

    // RX_MAILBOX_1: Second mailbox in chain (Receive with Overwrite Mode)
	// NOTE: SInce RX_MAILBOX_0 is in overwrite mode, this mailbox is disabled
    CAN0->CAN_MB[RX_MAILBOX_1].CAN_MAM = 0; // Accept all messages
    CAN0->CAN_MB[RX_MAILBOX_1].CAN_MID = CAN_MID_MIDE;
    CAN0->CAN_MB[RX_MAILBOX_1].CAN_MMR = CAN_MMR_MOT_MB_RX_OVERWRITE;
    CAN0->CAN_MB[RX_MAILBOX_1].CAN_MCR |= CAN_MCR_MTCR; // Mark as ready to receive

    // Enable receive interrupt if requested
	// This way when we receive a message we read from the mailbox buffer immediately
	//
	// For information about CAN Buss, read ATSAM3X8E Data Sheet:
	// Page 1185 - 1241: 40. Controller Area Network (CAN)
	// Page 1199 - 1210: 40.8 Functional Description
	// Page 1200 - 1201: 40.8.2 CAN Controller Interrupt Handling
    if (rxInterrupt) {
        CAN0->CAN_IER |= (1 << RX_MAILBOX_0) | (1 << RX_MAILBOX_1);
        NVIC_EnableIRQ(ID_CAN0);
    }

    // Enable CAN controller
    CAN0->CAN_MR |= CAN_MR_CANEN;
}



// Send a CAN message using TX mailbox
void can_tx(CanMsg m) {
	// For information about CAN Buss, read ATSAM3X8E Data Sheet:
	// Page 1185 - 1241: 40. Controller Area Network (CAN)
	// Page 1199 - 1210: 40.8 Functional Description
	// Page 1201 - 1207: 40.8.3 CAN Controller Message Handling
	
    // Wait until TX mailbox is ready
	//
	// For information about CAN Buss, read ATSAM3X8E Data Sheet:
	// Page 1235 - 1237: 40.9.18 CAN Message Status Register
    while (!(CAN0->CAN_MB[TX_MAILBOX].CAN_MSR & CAN_MSR_MRDY)) {};

    // Set message ID
	// The CAN_MID is set to activate mailbox to deal with version 2.0 Part B messages (CAN_MID_MIDE = 1)
	// All this to say that we can handle extended CAN message format
	// (Even tho we end up not using extended and going for the standard 8 byte CAN Message format with ID of 11 - bits)
	//
	// For more information CAN ID setup, read ATSAM3X8E Data Sheet:
	// Page 1188: 40.7.2 Mailbox Organization
	// Page 1188: 40.7.2.1 Message Acceptance Procedure
	// Page 1233: 40.9.16 CAN Message ID Register
	CAN0->CAN_MB[TX_MAILBOX].CAN_MID = CAN_MID_MIDvA(m.id) | CAN_MID_MIDE; // Set CAN ID and enable extended frame format

    // Load message data into mailbox
	//
	// For more information CAN Data Registers, read ATSAM3X8E Data Sheet:
	// Page 1238: 40.9.19 CAN Message Data Low Register
	// Page 1239: 40.9.20 CAN Message Data High Register
    CAN0->CAN_MB[TX_MAILBOX].CAN_MDL = m.dword[0];
    CAN0->CAN_MB[TX_MAILBOX].CAN_MDH = m.dword[1];

    // Mark mailbox for transmission
	// We set the MDLC register instead of MRTR bit, because we don't want to send a remote frame
	// We in fact don't want to deal with remote frames or any interrupts and just want a minimalistic CAN message setup
	// Also we set CAN_MCR_MDLC_Pos field as the length of the message
	// Most of the time it should be 8 bytes long, but in case to much we mask the length
	//
	// For information about CAN Buss, read ATSAM3X8E Data Sheet:
	// Page 1235 - 1237: 40.9.18 CAN Message Status Register
	// Page 1240 - 1241: 40.9.21 CAN Message Control Register
	uint8_t length_modified = m.length > 8 ? 8 : m.length;  // Limit message length to a maximum of 8 bytes
    CAN0->CAN_MB[TX_MAILBOX].CAN_MCR = (length_modified << CAN_MCR_MDLC_Pos) | CAN_MCR_MTCR;
}

// Receive a CAN message from a specified mailbox.
// Does not block. Returns 0 if there is no message, 1 otherwise.
// `mailbox` argument specifies the mailbox number to read from (e.g., 0 or 1).
uint8_t can_rx(CanMsg* m, uint8_t mailbox) {
	// For information about CAN Buss, read ATSAM3X8E Data Sheet:
	// Page 1185 - 1241: 40. Controller Area Network (CAN)
	// Page 1199 - 1210: 40.8 Functional Description
	// Page 1201 - 1207: 40.8.3 CAN Controller Message Handling
	
    // Check if RX mailbox has a new message
    if (!(CAN0->CAN_MB[mailbox].CAN_MSR & CAN_MSR_MRDY)) {
        return 0;
    }

    // Retrieve message ID and data length
    m->id = (uint8_t)((CAN0->CAN_MB[mailbox].CAN_MID & CAN_MID_MIDvA_Msk) >> CAN_MID_MIDvA_Pos);
    m->length = (uint8_t)((CAN0->CAN_MB[mailbox].CAN_MCR & CAN_MCR_MDLC_Msk) >> CAN_MCR_MDLC_Pos);

    // Retrieve message data
    m->dword[0] = CAN0->CAN_MB[mailbox].CAN_MDL;
    m->dword[1] = CAN0->CAN_MB[mailbox].CAN_MDH;

    // Reset mailbox for new reception
    CAN0->CAN_MB[mailbox].CAN_MCR |= CAN_MCR_MTCR;
    return 1;
}



// CAN interrupt handler
void CAN0_Handler(void) {
	// For information about CAN Buss, read ATSAM3X8E Data Sheet:
	// Page 1185 - 1241: 40. Controller Area Network (CAN)
	// Page 1199 - 1210: 40.8 Functional Description
	// Page 1200 - 1201: 40.8.2 CAN Controller Interrupt Handling
	
	CanMsg received_msg;
	uint32_t can_sr = CAN0->CAN_SR;

	// Check if RX_MAILBOX_0 received a message
	if (can_sr & (1 << RX_MAILBOX_0)) {
		// Get message from RX_MAILBOX_0, check that its successfully read
		if (can_rx(&received_msg, RX_MAILBOX_0)) {
			// Save message to the buffer we can read from
			can_rx_message_buffer.id = received_msg.id;
				
			can_rx_message_buffer.length = received_msg.length;
				
			can_rx_message_buffer.byte[0] = received_msg.byte[0];
			can_rx_message_buffer.byte[1] = received_msg.byte[1];
			can_rx_message_buffer.byte[2] = received_msg.byte[2];
			can_rx_message_buffer.byte[3] = received_msg.byte[3];
			can_rx_message_buffer.byte[4] = received_msg.byte[4];
			can_rx_message_buffer.byte[5] = received_msg.byte[5];
			can_rx_message_buffer.byte[6] = received_msg.byte[6];
			can_rx_message_buffer.byte[7] = received_msg.byte[7];
				
			// Print all values
			//printf("%d", (int8_t)(can_rx_message_buffer.byte[0]));printf(" | ");
			//printf("%d", (int8_t)(can_rx_message_buffer.byte[1]));printf(" | ");
			//printf("%d", (int8_t)(can_rx_message_buffer.byte[2]));printf(" | ");
			//printf("%d", (int8_t)(can_rx_message_buffer.byte[3]));printf(" | ");
			//printf("%d", (int8_t)(can_rx_message_buffer.byte[4]));printf(" | ");
			//printf("%d", (int8_t)(can_rx_message_buffer.byte[5]));printf(" | ");
			//printf("%d", (int8_t)(can_rx_message_buffer.byte[6]));printf(" | ");
			//printf("%d", (int8_t)(can_rx_message_buffer.byte[7]));printf("\n\r");
		}
	}

	// Check if RX_MAILBOX_1 received a message
	if (can_sr & (1 << RX_MAILBOX_1)) {
		if (can_rx(&received_msg, RX_MAILBOX_1)) {
			// Debugging
			//printf("Interrupt: Message received in RX_MAILBOX_1 (Overwrite Mode)\n\r");
			//can_printmsg(received_msg);
		}
	}

	NVIC_ClearPendingIRQ(ID_CAN0);
}



CanMsg can_get_latest_message() {
	return can_rx_message_buffer;
}