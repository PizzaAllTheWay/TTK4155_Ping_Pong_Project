/*
 * can_improved.c
 *
 * Created: 26/10/2024 18:14:15
 * Author: Martynas
 */

#include "sam.h"
#include "can_improved.h"
#include <stdio.h>



// Utility function to print CAN message details
void can_printmsg_improved(CanMsg m) {
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
void can_init_improved(CanInit init, uint8_t rxInterrupt) {
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
    PMC->PMC_PCR = PMC_PCR_EN | PMC_PCR_CMD | (ID_CAN0 << PMC_PCR_PID_Pos);
    PMC->PMC_PCER1 |= (1 << (ID_CAN0 - 32));

    // Set CAN baud rate and timing
    CAN0->CAN_BR = init.reg;

    // Configure TX mailbox (Mailbox 0) for transmission
    CAN0->CAN_MB[TX_MAILBOX].CAN_MID = CAN_MID_MIDE;
    CAN0->CAN_MB[TX_MAILBOX].CAN_MMR = CAN_MMR_MOT_MB_TX;

    // Configure RX mailboxes for chained reception
    // RX_MAILBOX_0: First mailbox in chain (Receive Mode)
    CAN0->CAN_MB[RX_MAILBOX_0].CAN_MAM = 0; // Accept all messages
    CAN0->CAN_MB[RX_MAILBOX_0].CAN_MID = CAN_MID_MIDE;
    CAN0->CAN_MB[RX_MAILBOX_0].CAN_MMR = CAN_MMR_MOT_MB_RX_OVERWRITE; //CAN_MMR_MOT_MB_RX;
    CAN0->CAN_MB[RX_MAILBOX_0].CAN_MCR |= CAN_MCR_MTCR; // Mark as ready to receive

    // RX_MAILBOX_1: Second mailbox in chain (Receive with Overwrite Mode)
    CAN0->CAN_MB[RX_MAILBOX_1].CAN_MAM = 0; // Accept all messages
    CAN0->CAN_MB[RX_MAILBOX_1].CAN_MID = CAN_MID_MIDE;
    CAN0->CAN_MB[RX_MAILBOX_1].CAN_MMR = CAN_MMR_MOT_MB_RX_OVERWRITE;
    CAN0->CAN_MB[RX_MAILBOX_1].CAN_MCR |= CAN_MCR_MTCR; // Mark as ready to receive

    // Enable receive interrupt if requested
    if (rxInterrupt) {
        CAN0->CAN_IER |= (1 << RX_MAILBOX_0) | (1 << RX_MAILBOX_1);
        NVIC_EnableIRQ(ID_CAN0);
    }

    // Enable CAN controller
    CAN0->CAN_MR |= CAN_MR_CANEN;
}

// Send a CAN message using TX mailbox
void can_tx_improved(CanMsg m) {
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
uint8_t can_rx_improved(CanMsg* m, uint8_t mailbox) {
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
	CanMsg received_msg;
	uint32_t can_sr = CAN0->CAN_SR;

	// Check if RX_MAILBOX_0 received a message
	if (can_sr & (1 << RX_MAILBOX_0)) {
		if (can_rx_improved(&received_msg, RX_MAILBOX_0)) {
			//printf("Interrupt: Message received in RX_MAILBOX_0\n");
			//can_printmsg_improved(received_msg);
		}
	}

	// Check if RX_MAILBOX_1 received a message (Overwrite Mode)
	if (can_sr & (1 << RX_MAILBOX_1)) {
		if (can_rx_improved(&received_msg, RX_MAILBOX_1)) {
			//printf("Interrupt: Message received in RX_MAILBOX_1 (Overwrite Mode)\n");
			//can_printmsg_improved(received_msg);
		}
	}

	NVIC_ClearPendingIRQ(ID_CAN0);
}
