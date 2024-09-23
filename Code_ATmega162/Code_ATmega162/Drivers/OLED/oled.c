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

void _oled_select_page(uint8_t page_nr){
	// Only select page if page nr is less than maximum pages on OLED controller
	if (page_nr < OLED_MAX_LINES) {
		_oled_write_command(0x22); // Setup page start and end address
		_oled_write_command(0x07 & page_nr); // Column start address
		_oled_write_command(0x07 & page_nr); // Column end address
	}
}

void _oled_select_column(uint8_t column_nr){
	// Only select column if column nr is less than maximum columns on OLED controller
	if (column_nr < OLED_PIXEL_WIDTH) {
		_oled_write_command(0x21); // Setup column start and end address
		_oled_write_command(0x7F & column_nr); // Column start address
		_oled_write_command(0x7F & column_nr); // Column end 
	}
}

void _oled_set_letter(uint8_t letter, uint8_t x) {
	// Offset since letters start from 0x20, however our font array starts from 0x00
	uint8_t font_offset = 0x20;
	letter -= font_offset;
	
	for (uint8_t i = 0; i < 8; i++) {
		// font 8 has 8 columns
		// We have to iterate witch column we write to
		_oled_select_column(x + i); // x
		
		// We saved our fonts in Programmable Memory
		// In order to get fonts back form programmable memory we use pgm_read_word()
		_oled_write_data(pgm_read_word(&font8[letter][i]));
	}
}



// Text related functions
void oled_set_text(uint8_t* text, uint8_t x, uint8_t y) {
	// Set start position of the text
	_oled_select_page(y); // y
	_oled_select_column(x); // x
	
	// Loop through the string and print each letter
	for (uint8_t i = 0; text[i] != '\0'; i++) {
		_oled_set_letter(text[i], x);  // Print each character
		x += 8;  // Move to the next position for the next letter (assuming 8 pixels per character width)
	}
}



// Cleaning related functions
void oled_clear_page(uint8_t page_rn) {
	if (page_rn < OLED_MAX_LINES) {
		_oled_select_page(page_rn); // y
		
		for (uint8_t i = 0; i < OLED_PIXEL_WIDTH; i++) {
			_oled_select_column(i); // x
			_oled_write_data(0x00);
		}
	}
}

void oled_clear_screen() {
	for (uint8_t page_nr = 0; page_nr < OLED_MAX_LINES; page_nr++) {
		oled_clear_page(page_nr);
	}
}



// Pixel related functions
void oled_set_area_pattern(uint8_t pattern, uint8_t x_start, uint8_t y_start, uint8_t x_stop, uint8_t y_stop) {
	// Ensure x_start, x_stop, y_start, and y_stop are within the valid OLED range
	if (x_start >= OLED_PIXEL_WIDTH || x_stop >= OLED_PIXEL_WIDTH || y_start >= OLED_MAX_LINES || y_stop >= OLED_MAX_LINES) {
		return; // Do nothing if out of bounds
	}

	// Loop through the pages (Y-axis) from y_start to y_stop
	for (uint8_t page = y_start; page <= y_stop; page++) {
		_oled_select_page(page);  // Set the page (Y-axis position)
		
		// Set the selected area with the pattern
		for (uint8_t column = x_start; column <= x_stop; column++) {
			_oled_select_column(column); // Set the column (X-axis position)
			
			// Write 0x00 to clear the area
			_oled_write_data(pattern);  
		}
	}
}

void oled_set_nibble(uint8_t* nibble, uint8_t x, uint8_t y) {
	// Ensure x and y are within the valid OLED range
	if (x >= OLED_PIXEL_WIDTH || y >= OLED_MAX_LINES) {
		return; // Do nothing if out of bounds
	}

	// Set the nibble
	_oled_select_page(y); // Set the page (Y-axis position)
		
	// Set the specified nibble in it requested position
	// 8 because each nibble is 8 segments
	for (uint8_t i = 0; i < 8; i++) {
		uint8_t column = x + i;
		_oled_select_column(column); // Set the column (X-axis position)
		
		// Write nibble segment
		// In order to get fonts back form programmable memory we use pgm_read_word()
		_oled_write_data(pgm_read_byte(&nibble[i]));
	}
}

void oled_set_sprite(uint8_t* sprite, uint8_t sprite_nibble_width, uint8_t sprite_nibble_height, uint8_t x, uint8_t y) {
	// Ensure x, y, width, and height are within the valid OLED range
	// 8 is here because we check if the whole sprite will fit inn
	if (x >= OLED_PIXEL_WIDTH || y >= OLED_MAX_LINES || (x + (sprite_nibble_width * 8)) > OLED_PIXEL_WIDTH || (y + sprite_nibble_height) > OLED_MAX_LINES) {
		return; // Do nothing if out of bounds
	}

	// Iterate through the height (Y-axis) in terms of pages
	for (uint8_t page = 0; page < sprite_nibble_height; page++) {
		// For each page (Y), we set the corresponding nibbles (X-axis)
		for (uint8_t nibble_idx = 0; nibble_idx < sprite_nibble_width; nibble_idx++) {
			// Get the nibble data from program memory
			uint8_t* nibble = &sprite[(page * sprite_nibble_width * 8) + (nibble_idx * 8)];
			
			// Draw the nibble at the correct X and Y positions
			oled_set_nibble(nibble, x + (nibble_idx * 8), y + page);
		}
	}
}



// Animation Functions
void oled_move_screen(
	uint8_t y_start,
	uint8_t y_stop,
	uint8_t speed_x,
	uint8_t speed_y,
	uint8_t direction_x
) {
	// y_start (0-7)
	// y_stop (0-7)
	// speed_x (0 - 7)
	// 0 - SLOW
	// 7 - FAST
	// speed_y (0 - 63)
	// 0 - SLOW
	// 63 - FAST
	// direction (0/1)
	// 0 - LEFT
	// 1 - RIGHT
	
	// Deactivate any previous scroll before setting up a new one
	_oled_write_command(0x2E); // Deactivate scroll
	
	// Continuous Vertical and Horizontal Scroll Setup (START) --------------------------------------------------
	// Choose right or left scroll based on the direction
	if (direction_x == 1) {  
		// 1 - Vertical Right Scroll
		_oled_write_command(0x29); // Vertical and Right Horizontal Scroll
	} else {  
		// 0 - Vertical Left Scroll
		_oled_write_command(0x2A); // Vertical and Left Horizontal Scroll
	}
	
	// A[2:0] Dummy byte (always 0x00)
	_oled_write_command(0x00);
	
	// B[2:0] Start page address (Y start position)
	// Apply filter for the bits we care about (0x07)
	_oled_write_command(0x07 & y_start);
	
	// C[2:0] Set the speed of x (0x00 = slowest, 0x07 = fastest)
	// Apply filter for the bits we care about (0x07)
	_oled_write_command(0x07 & speed_x); 
	
	// D[2:0] End page address (Y position)
	// Apply filter for the bits we care about (0x07)
	_oled_write_command(0x07 & y_stop);
	
	// E[5:0] Set Vertical scrolling offset 
	// Apply filter for the bits we care about (0x3F)
	_oled_write_command(0x3F & speed_y);
	// Continuous Vertical and Horizontal Scroll Setup (STOP) --------------------------------------------------
	
	// Set Vertical Scroll Area (START) --------------------------------------------------
	// Ensure the number of pages does not exceed the display
	uint8_t scroll_pages = y_stop - y_start;
	if (scroll_pages > 7) {
		scroll_pages = 7; // Limit to maximum of 7 pages/rows
	}

	// Set the vertical scroll area
	// Set Vertical Scroll Area command
	_oled_write_command(0xA3);  
	
	// A[5:0] Define the start row of the scrolling area
	// Apply filter for the bits we care about (0x3F)
	// *8 because every nibble is 8 pixels long and we don't want to ruin the symmetry
	_oled_write_command(0x3F & (y_start * 8));
	
	// B[6:0] Define the number of rows for the scroll area
	// Apply filter for the bits we care about (0x7F)
	// +1 Because we need to take the end of the page pixel and not the start, so it will count as extra pixel
	// *8 because every nibble is 8 pixels long and we don't want to ruin the symmetry
	_oled_write_command(0x7F & ((scroll_pages + 1) * 8));
	// Set Vertical Scroll Area (STOP) --------------------------------------------------
	
	// Activate scrolling
	_oled_write_command(0x2F);
}



// Initialization function itself
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
	_oled_write_command(0x00); // Horizontal addressing mode

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
	oled_clear_screen();
}

