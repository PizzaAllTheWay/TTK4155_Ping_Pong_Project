/*
 * can_improved.c
 *
 * Created: 26/10/2024 18:14:15
 * Author: Martynas
 */



#include "can.h"



// Utility function to print CAN message details
void can_printmsg(CanMsg m) {
    printf("CanMsg(id:%d, length:%d, data:{", m.id, m.length);
    if (m.length) {
        printf("%d", m.byte[0]);
    }
    for (uint8_t i = 1; i < m.length; i++) {
        printf(", %d", m.byte[i]);
    }
    printf("})\n");
}



// Initialize the CAN controller
void can_init(CanInit init, uint8_t rxInterrupt) {
	// For information about CAN Buss, read ATSAM3X8E Data Sheet:
	// Page 1185 - 1241: 40. Controller Area Network (CAN)
	// Page 1199 - 1210: 40.8 Functional Description
	// Page 1199: 40.8.1 CAN Controller Initialization
	
    // Disable CAN to configure
    CAN0->CAN_MR &= ~CAN_MR_CANEN;

    // Clear status register
    __attribute__((unused)) uint32_t ul_status = CAN0->CAN_SR;

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
    CAN0->CAN_MB[TX_MAILBOX].CAN_MID = CAN_MID_MIDE;
    CAN0->CAN_MB[TX_MAILBOX].CAN_MMR = CAN_MMR_MOT_MB_TX;

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
    while (!(CAN0->CAN_MB[TX_MAILBOX].CAN_MSR & CAN_MSR_MRDY)) {}

    // Set message ID and length (max 8 bytes)
    CAN0->CAN_MB[TX_MAILBOX].CAN_MID = CAN_MID_MIDvA(m.id) | CAN_MID_MIDE;
    m.length = m.length > 8 ? 8 : m.length;

    // Load message data into mailbox
    CAN0->CAN_MB[TX_MAILBOX].CAN_MDL = m.dword[0];
    CAN0->CAN_MB[TX_MAILBOX].CAN_MDH = m.dword[1];

    // Mark mailbox for transmission
    CAN0->CAN_MB[TX_MAILBOX].CAN_MCR = (m.length << CAN_MCR_MDLC_Pos) | CAN_MCR_MTCR;
}

// Receive a CAN message from RX mailbox
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
		if (can_rx(&received_msg, RX_MAILBOX_0)) {
			// Debugging
			//printf("Interrupt: Message received in RX_MAILBOX_0\n");
			//can_printmsg_improved(received_msg);
		}
	}

	// Check if RX_MAILBOX_1 received a message (Overwrite Mode)
	if (can_sr & (1 << RX_MAILBOX_1)) {
		if (can_rx(&received_msg, RX_MAILBOX_1)) {
			// Debugging
			//printf("Interrupt: Message received in RX_MAILBOX_1 (Overwrite Mode)\n");
			//can_printmsg_improved(received_msg);
		}
	}

	NVIC_ClearPendingIRQ(ID_CAN0);
}
