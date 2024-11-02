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
	// 500 kbps Data buss
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
	//   - SMP = 1
	//
	// We chose these values as it adheres to data sheet specifications
	//		bit-rate MUST be 16 TQ (ie 500 kbps in our case)
	//		SJW + PROP + PHASE1 + PHASE2 = 16 TQ
	// TQ: Time Quantum
	// These TQ are responsible for the Data Rate on the buss and should be alike for all systems on the buss
	// This way each controller even while having different clock speeds, can communicate with each other synchronously
	// Other requirements for sending data through MPC2515 CAN Controller include:
	//		PROP + PHASE1 >= PHASE2
	//		PROP + PHASE1 >= T_DELAY
	//		PHASE2 > SJW
	// T_DELAY: represents the delay time required to account for the propagation delay in signal transmission on the CAN bus. This delay is typically 1-2 Time Quanta (TQ) and is factored in to ensure accurate sampling and synchronization of data across different nodes on the bus
	// Read more on this topic in MPC2515 CAN Controller data sheet:
	// Page 39 - 49: 5.0 BIT TIMING
	
	
	
	// CNF1: Configure BRP (Baud Rate Prescaler) and SJW (Sync Jump Width)
	// for a 500 kbps CAN speed with FOSC at 16 MHz we must have
	// NOTE: 16MHz is NOT ATmega162 F_osc (witch is around 4 Mhz instead), its the MCP2515 CAN Controller F_osc, thats why its 16 MHz
	//
	// Way to figure out BRP (Baud Rate Prescaler)
	//		TQ = 2 * (BRP + 1) / F_OSC_REAL
	//		F_OSC_REAL = F_OSC * prescaler
	// Just like with UART in Node 1 with ATmega162 Microcontroller, so does the MCP2515 CAN Controller has a Clock Prescaler
	// This Clock prescaler is set by manipulating CANCTRL register and setting bits 0-1 (CLKPRE)
	// We have set them to 11 =>  FCLKOUT = System Clock/8
	// Read More on Prescaler Clock in MCP2515 CAN Controller data sheet:
	// Page 55: 8.0 OSCILLATOR
	// Page 55: 8.2 CLKOUT Pin
	// Page 60: CANCTRL – CAN CONTROL REGISTER
	// Either way now that we know our prescale value we set it in to the function for BRP
	//		F_OSC_REAL = F_OSC * prescaler
	//		F_OSC_REAL = F_OSC * 8
	//		TQ = 2 * (BRP + 1) / F_OSC_REAL
	//		TQ = 2 * (BRP + 1) / (8 * F_OSC)
	//		TQ = (BRP + 1)/(4 * F_OSC)
	//		TQ * (4 * F_OSC) = BRP + 1
	//		BRP = TQ * (4 * F_OSC) - 1
	// We also have that TQ is time quantum
	//		TQ = bit-rate/n
	//			bit-rate: data speed on the CAN buss we want
	//			n: Number of time quantums we have per data package sent/received
	//		TQ = 500 kbps/16
	//		BRP = (500 kbps/16) * (4 * F_OSC) - 1
	//			F_OSC: Just the external crystal oscillator for the MCP2515 CAN Controller
	//		BRP = (500 kbps/16) * (4 * 16 MHz) - 1
	//		BRP = 7
	//
	// BRP = 7
	// SJW = 1
	//
	// CNF1 bit 0-5: BRP = 0x07 (BRP = 7)
	// CNF1 bit 7-6: SJW = 0x01 (Length = 1 * TQ)
	//
	// Read more on this topic in MPC2515 CAN Controller data sheet:
	// Page 39 - 49: 5.0 BIT TIMING
	// Page 43:  5.5 Bit Timing Configuration Registers
	// Page 44:  REGISTER 5-1: CNF1 – CONFIGURATION 1
	uint8_t BRP = 0x07; // BRP = 7
	uint8_t SJW = 0x01; // SJW = 1
	uint8_t cnf1_mode = (SJW << 6) | (BRP);
	mcp2515_driver_write(MCP_CNF1, cnf1_mode);

	// CNF2: Configure PROPSEG, PHASE1, and sample mode (SAM)
	// Set BTLMODE to enable programmable Phase2 (configured in CNF3)
	//
	// SAM = 0 (Single sample mode) (same as SMP)
	// BTLMODE = 1 (bit 7) enables programmable PHASE2 in CNF3.
	//
	// PROP = 6 TQ
	// PHASE1 = 5 TQ
	//
	// CNF2 bit 7: BTLMODE = 0x01
	// CNF2 bit 6: SAM = 0x01 (For noise reduction enter triple mode to sample each data point 3 times to average it out)
	// CNF2 bits 5-3: PHSEG1 = 0x05
	// CNF2 bits 2-0: PROPSEG = 0x06
	//
	// Read more on this topic in MPC2515 CAN Controller data sheet:
	// Page 39 - 49: 5.0 BIT TIMING
	// Page 43:  5.5 Bit Timing Configuration Registers
	// Page 44:  REGISTER 5-2: CNF2 – CONFIGURATION 1
	uint8_t BTLMODE = 0x01;     // BTLMODE = 1 (enables programmable PHASE2)
	uint8_t SAM = 0x01;         // Single sample mode = 1
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
	//
	// Read more on this topic in MPC2515 CAN Controller data sheet:
	// Page 39 - 49: 5.0 BIT TIMING
	// Page 43:  5.5 Bit Timing Configuration Registers
	// Page 45:  REGISTER 5-3: CNF3 – CONFIGURATION 1
	uint8_t SOF = 0x00;           // Disable Start-of-Frame
	uint8_t WAKFIL = 0x00;        // Disable wake-up filter
	uint8_t PHSEG2 = 0x06;        // PHASE2 = 6 TQ
	uint8_t cnf3_mode = (SOF << 7) | (WAKFIL << 6) | (PHSEG2);
	mcp2515_driver_write(MCP_CNF3, cnf3_mode);

	
	
	// Set up Receive ----------
	// In order to read and receive data we must configure the RXB0CTRL register
	//
	// Bit 6-5: RXM = 0x03 (bit 11) => Turn mask/filters off; receive any message
	// 
	// Read more on this topic in MPC2515 CAN Controller data sheet:
	// Page 24 - 38: 4.0 MESSAGE RECEPTION
	// Page 27: RXB0CTRL – RECEIVE BUFFER 0 CONTROL
	uint8_t RXM = 0x03;
	uint8_t receive_control_mode = (RXM << 5);
	mcp2515_driver_write(RXB0CTRL, receive_control_mode);
	
	
	

	// Set up Interrupts ----------
	// In order to know when we receive a message, we must enable interrupts
	// This is done by setting PIN CONTROL AND STATUS register (BFPCTRL)
	// We want data from buffer 0 as it is the first one to receive data and will give out interupt first
	// This means we must set buffer 0 interupt up
	// This is done by setting B0BFE, B0BFM, and B0BFS bits
	// 
	// To configure interupt pin for buffer 0 (INT PIN => RX0BF)
	// We must set the bits in the register the following way:
	// bit 4: B0BFS = 0
	// bit 2: B0BFE = 1
	// bit 0: B0BFM = 1
	//
	// Read more on this topic in MPC2515 CAN Controller data sheet:
	// Page 24 - 38: 4.0 MESSAGE RECEPTION
	// Page 23: 4.1.3 RECEIVE FLAGS/INTERRUPTS
	// Page 24: 4.4.1 DISABLED
	// Page 24: 4.4.2 CONFIGURED AS BUFFER FULL
	// Page 25: 4.4.3 CONFIGURED AS DIGITAL OUTPUT
	// Page 29: BFPCTRL – RXnBF PIN CONTROL AND STATUS
	uint8_t B0BFS = 0x00; // B0BFS = 0
	uint8_t B0BFE = 0x01; // B0BFE = 1
	uint8_t B0BFM = 0x01; // B0BFS = 1
	uint8_t pin_control_and_status_mode = (B0BFS << 4) | (B0BFE << 2) | (B0BFM);
	mcp2515_driver_write(BFPCTRL, pin_control_and_status_mode);
	
	// Now that we have set up out pin as interrupt pin for buffer 0
	// We must now connect this pin to the interrupt service routine in the CAN Controller
	// THis way when we get a message, the buffer 0 interrupt pin (RX0BF) will respond with an interrupt signal
	// For this we must set up CANINTE register
	// We must enable interrupt on RX0IE bit
	//
	// bit 0: RX0IE = 1
	//
	// Read more on this topic in MPC2515 CAN Controller data sheet:
	// Page 24 - 38: 4.0 MESSAGE RECEPTION
	// Page 23: 4.1.3 RECEIVE FLAGS/INTERRUPTS
	// Page 52: CANINTE – INTERRUPT ENABLE
	uint8_t RX0IE = 0x01; // RX0IE = 1
	uint8_t interupt_eanble_mask = 0x01;
	uint8_t interupt_eanble_mode = RX0IE & interupt_eanble_mask;
	mcp2515_driver_bit_modify(MCP_CANINTE, interupt_eanble_mask, interupt_eanble_mode);
	
    
	
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

