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
    // Initialize SPI
    spi_driver_init();
    
    // Reset MCP2515
    mcp2515_driver_reset();
    
    // Set up MCP2515 to operate in the desired mode (loopback or normal)
    //uint8_t wanted_mode = MODE_LOOPBACK;  // Set to loopback mode for testing
    //uint8_t wanted_mode = MODE_NORMAL;  // Set to normal mode for regular operation
    
    //mcp2515_driver_bit_modify(MCP_CANCTRL, MODE_MASK, wanted_mode);  // Set the mode
	mcp2515_driver_write(MCP_CANCTRL, mode);

    // Wait until the mode changes
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
