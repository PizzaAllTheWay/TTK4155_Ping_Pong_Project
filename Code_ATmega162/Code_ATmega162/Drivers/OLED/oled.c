/*
 * oled_driver.c
 *
 * Created: 23/09/2024 01:56:52
 *  Author: Martynas
 */ 



#include "oled.h"



// OLED Handy functions for internal use
void _oled_write_command(uint8_t command){
	volatile char* oled_command = (char*) OLED_COMMAND_ADDRESS;
	oled_command[0] = command;
}

void _oled_write_data(uint8_t data){
	volatile char* oled_data = (char*) OLED_DATA_ADDRESS;
	oled_data[0] = data;
}

void oled_set_letter(uint8_t letter) {
	uint8_t font_offset = 0x20;
	
	// Offset since letters start from 0x20, however our font array starts from 0x00
	letter -= font_offset;
	
	for (uint8_t i = 0; i < 8; i++) {
		_oled_write_data(font8[letter][i]);
	}
}

void oled_clear_page(uint8_t page_rn) {
	if (page_rn < OLED_MAX_LINES) {
		page_rn += 0xB0; // Add the offset for the page nr comand start [0xB0 - 0xB7]
		_oled_write_command(page_rn); // Select page nr
		
		for (uint8_t i = 0; i < OLED_PIXEL_WIDTH; i++) {
			_oled_write_data(0x00);
		}
	}
}

void oled_clear_creen() {
	for (uint8_t page_nr = 0; page_nr < OLED_MAX_LINES; page_nr++) {
		oled_clear_page(page_nr);
	}
}

void oled_init() {
	// Setup External Data buss (START) --------------------------------------------------
	// NOTE: In order for External SRAM, ADC and OLED to function properly this initialization MUST stay here
	// This initialization configures the data and register buss between them
	external_sram_init();
	// Setup External Data buss (STOP) --------------------------------------------------
	
	
	
	// Setup OLED to basic mode (START) --------------------------------------------------
	_oled_write_command(0xAE); // Display OFF

	_oled_write_command(0xA1); // Segment remap
	_oled_write_command(0xDA); // Common pads hardware configuration
	_oled_write_command(0x12);

	_oled_write_command(0xC8); // Common output scan direction (COM63~COM0)

	_oled_write_command(0xA8); // Multiplex ratio
	_oled_write_command(0x3F); // 1/64 duty (64MUX)

	_oled_write_command(0xD5); // Display divide ratio/oscillator frequency
	_oled_write_command(0x80); // Default setting

	_oled_write_command(0x81); // Contrast control
	_oled_write_command(0x50); // Set contrast to 0x50

	_oled_write_command(0xD9); // Set pre-charge period
	_oled_write_command(0x21); // Pre-charge period 0x21

	_oled_write_command(0x20); // Set Memory Addressing Mode
	_oled_write_command(0x02); // Page addressing mode

	_oled_write_command(0xDB); // VCOM deselect level mode
	_oled_write_command(0x30); // 0.83*Vcc

	_oled_write_command(0xAD); // Master configuration
	_oled_write_command(0x00); // No external Vcc

	_oled_write_command(0xA4); // Resume to RAM content display
	// 0xA6 - Normal
	// 0xA7 - Inverse
	_oled_write_command(0xA6); // Set normal display mode

	_oled_write_command(0xAF); // Display ON
	// Setup OLED to basic mode (STOP) --------------------------------------------------
	
	// Clear the screen on startup
	oled_clear_creen();
}

