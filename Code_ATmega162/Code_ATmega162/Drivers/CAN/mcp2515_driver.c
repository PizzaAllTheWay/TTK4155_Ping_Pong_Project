/*
 * mcp2515.c
 *
 * Created: 07/10/2024 15:53:21
 *  Author: Martynas
 */



#include "mcp2515_driver.h"



// Resets MCP2515 internal registers to default state and sets Configuration mode
void mcp2515_driver_reset() {
    spi_driver_select();          // Select MCP2515 (CS low)
    spi_driver_write(MCP_RESET);  // Send reset command
    spi_driver_deselect();        // Deselect MCP2515 (CS high)
	_delay_ms(10); // A small delay just in case
}

// Allows the user to set or clear individual bits in a particular register
void mcp2515_driver_bit_modify(uint8_t address, uint8_t mask, uint8_t data) {
    spi_driver_select();             // Select MCP2515 (CS low)
    spi_driver_write(MCP_BITMOD);    // Command to modify bits
    spi_driver_write(address);       // Register address to modify
    spi_driver_write(mask);          // Mask: which bits to modify
    spi_driver_write(data);          // New data to set in the register
    spi_driver_deselect();           // Deselect MCP2515 (CS high)
}

// Write data to a register starting at a specific address
void mcp2515_driver_write(uint8_t address, uint8_t data) {
	spi_driver_select();          // Select MCP2515 (CS low)
	spi_driver_write(MCP_WRITE);  // Send write command
	spi_driver_write(address);    // Specify the register address
	spi_driver_write(data);       // Write the data
	spi_driver_deselect();        // Deselect MCP2515 (CS high)
}

// Read data from register beginning at selected address
uint8_t mcp2515_driver_read(uint8_t address) {
    uint8_t result;
    
    spi_driver_select();            // Select MCP2515 (CS low)
    spi_driver_write(MCP_READ);     // Send read command
    spi_driver_write(address);      // Send address we want to read
    result = spi_driver_read();     // Read the result from the MCP2515
    spi_driver_deselect();          // Deselect MCP2515 (CS high)

    return result;
}



// Initialization Function
uint8_t mcp2515_driver_init(int8_t mode) {
    // Initialize SPI ----------
    spi_driver_init();
    
    // Reset MCP2515 ----------
    mcp2515_driver_reset();
	
	
	
	// Set up Bit Timing ----------
	// Bit timing can only be set up in CONFIG mode
	// Set MCP2515 to Configuration mode
	mcp2515_driver_write(MCP_CANCTRL, MODE_CONFIG);

	// Wait until the mode changes to Configuration
	while ((mcp2515_driver_read(MCP_CANSTAT) & MODE_MASK) != MODE_CONFIG);
	
	// Bit Timing Configuration: Set CNF1, CNF2, and CNF3 registers
	// We are setting a 1 Mbps CAN speed with standard timing:
	//   - BRP = 7
	//   - PHASE1 = 6
	//   - PHASE2 = 6
	//   - PROP = 3
	//   - SJW = 1
	//   - SMP = 0

	// CNF1: Configure BRP (Baud Rate Prescaler) and SJW (Sync Jump Width)
	// for a 500 kbps CAN speed with FOSC at 16 MHz we must have
	// NOTE: 16MHz is NOT ATmega162 F_osc (witch is around 4 Mhz instead), its the MCP2515 CAN Controller F_osc, thats why its 16 MHz
	//
	// TQ = 2 * (BRP + 1) / FOSC
	//
	// BRP = 3
	// SJW = 1
	//
	// CNF1 bit 0-5: BRP = 0x03 (BRP = 3)
	// CNF1 bit 7-6: SJW = 0x01 (Length = 1 * TQ)
	uint8_t BRP = 0x07; // BRP = 7
	uint8_t SJW = 0x01; // SJW = 1
	uint8_t cnf1_mode = (SJW << 6) | (BRP);
	mcp2515_driver_write(MCP_CNF1, cnf1_mode);

	// CNF2: Configure PROPSEG, PHASE1, and sample mode (SAM)
	// Set BTLMODE to enable programmable Phase2 (configured in CNF3)
	//
	// SAM = 0 (Single sample mode)
	// BTLMODE = 1 (bit 7) enables programmable PHASE2 in CNF3.
	//
	// PROP = 6 TQ
	// PHASE1 = 5 TQ
	//
	// CNF2 bit 7: BTLMODE = 0x01
	// CNF2 bit 6: SAM = 0x00 (For noise reduction enter triple mode to sample each data point 3 times to average it out, WE DO NOT USE IT THO, WE DISABLED IT)
	// CNF2 bits 5-3: PHSEG1 = 0x05
	// CNF2 bits 2-0: PROPSEG = 0x06
	uint8_t BTLMODE = 0x01;     // BTLMODE = 1 (enables programmable PHASE2)
	uint8_t SAM = 0x00;         // Single sample mode = 0
	uint8_t PHSEG1 = 0x06;      // PHASE1 = 6 TQ
	uint8_t PROPSEG = 0x03;     // PROPSEG = 3 TQ
	uint8_t cnf2_mode = (BTLMODE << 7) | (SAM << 6) | (PHSEG1 << 3) | (PROPSEG);
	mcp2515_driver_write(MCP_CNF2, cnf2_mode);

	// CNF3: Configure PHASE2 and WAKFIL (Wake-up Filter)
	//
	// PHASE2 (PHSEG2) = 2 TQs (set as 1 for zero-based indexing)
	// WAKFIL = 0 (disable wake-up filter)
	// SOF = 0 (Start-of-Frame signal disabled)
	//
	// CNF3 bit 7: SOF = 0x00 (Start-of-Frame disabled, CLKOUT is used for clockout function)
	// CNF3 bit 6: WAKFIL = 0x00 (Wake-up filter disabled)
	// CNF3 bits 5-3: Unimplemented (must be set to 0)
	// CNF3 bits 2-0: PHSEG2 = 0x01 (PHSEG2 = 1 TQ)
	uint8_t SOF = 0x00;           // Disable Start-of-Frame
	uint8_t WAKFIL = 0x00;        // Disable wake-up filter
	uint8_t PHSEG2 = 0x06;        // PHASE2 = 6 TQ
	uint8_t cnf3_mode = (SOF << 7) | (WAKFIL << 6) | (PHSEG2);
	mcp2515_driver_write(MCP_CNF3, cnf3_mode);


    
    // Set the mode ----------
	mcp2515_driver_write(MCP_CANCTRL, mode);

    // Wait until the mode changes ----------
    while ((mcp2515_driver_read(MCP_CANSTAT) & MODE_MASK) != mode);

    return 0;  // Initialization successful
}



// Instructs MCP2515 to begin message transmission sequence for all transmit buffers (TXB0, TXB1, and TXB2)
void mcp2515_driver_request_to_send() {
	spi_driver_select();             // Select MCP2515 (CS low)
	spi_driver_write(MCP_RTS_ALL);   // Request to send from all buffers (TXB0, TXB1, TXB2)
	spi_driver_deselect();           // Deselect MCP2515 (CS high)
}

// Quick polling command to read the status of the transmit and receive functions
uint8_t mcp2515_driver_read_status() {
    uint8_t status;
    
    spi_driver_select();             // Select MCP2515 (CS low)
    spi_driver_write(MCP_READ_STATUS);  // Send the read status command
    status = spi_driver_read();      // Read the status register
    spi_driver_deselect();           // Deselect MCP2515 (CS high)

    return status;
}
