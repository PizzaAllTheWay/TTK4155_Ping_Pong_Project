/*
 * mcp2515.c
 *
 * Created: 07/10/2024 15:53:21
 *  Author: Martynas
 */ 

#include "mcp2515.h"

//lab forelesning init example
uint8_t mcp2515_init(){
	uint8_t value;
	
	spi_init();
	mcp2515_reset();

	// Set CAN mode

	//// Enter configuration mode
    //mcp2515_bit_modify(MCP_CANCTRL, MODE_MASK, MODE_CONFIG);
    //// Wait for the mode to change
    //while ((mcp2515_read(MCP_CANSTAT) & MODE_MASK) != MODE_CONFIG);
	
	uint8_t wanted_mode = MODE_LOOPBACK;
    // uint8_t wanted_mode = MODE_NORMAL;
	
	// Set mode
    mcp2515_bit_modify(MCP_CANCTRL, MODE_MASK, wanted_mode);
    // Wait for the mode to change
    while ((mcp2515_read(MCP_CANSTAT) & MODE_MASK) != wanted_mode);
	
	
	//mcp2515_read(MCP_CANSTAT, &value);
	//mcp2515_read(MCP_CANSTAT);
	
	//if((value & MODE_MASK) != MODE_CONFIG){
	//if((value & MODE_MASK) != MODE_CONFIG){
	//	printf("MCP2515 is NOT in configuration mode after reset!\n");
	//	return 1;
	//}
	//more initialization
	
	return 0;
}

//Read data from register beginning at selected address.
uint8_t mcp2515_read(uint8_t address){
	uint8_t result;
	
	spi_select();
	spi_write(MCP_READ);
	spi_write(address);
	result = spi_read();
	spi_deselect();
	
	return result;
}

//Write data to register beginning at selected address
void mcp2515_write(uint8_t address, uint8_t data){
	spi_select();
	spi_write(MCP_WRITE);
	spi_write(address);
	spi_write(data);
	spi_deselect();
}



//Instructs controller to begin message transmission sequence for any of the transmit buffers.
void mcp2515_request_to_send(){
	spi_select();
	spi_write(MCP_RTS_ALL);
	spi_deselect();
}

//Allows the user to set or clear individual bits in a particular register
void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data){
	spi_select();
	spi_write(MCP_BITMOD);
	spi_write(address);
	spi_write(mask);
	spi_write(data);
	spi_deselect();
}

//Resets internal registers to default state, set Configuration mode.
void mcp2515_reset(){
	spi_select();
	spi_write(MCP_RESET);
	spi_deselect();
}


//Quick polling command that reads several status bits for transmit and receive functions
uint8_t mcp2515_read_status(){
	uint8_t value;
	
	spi_select();
	spi_write(MCP_READ_STATUS);
	value = spi_read();
	
	spi_deselect();
	
	return value;
}
