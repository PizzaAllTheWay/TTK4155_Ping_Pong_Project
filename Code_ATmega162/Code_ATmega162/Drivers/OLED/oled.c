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
    _oled_write_command(0xA6); // Set normal display

    _oled_write_command(0xAF); // Display ON
	// Setup OLED to basic mode (STOP) --------------------------------------------------
}







/**
 * @brief Moves the OLED cursor to the specified line (page).
 * 
 * @param line The line number (0 to OLED_MAX_LINES - 1)
 */
/*
void oled_goto_line(uint8_t line) {
    if (line >= OLED_MAX_LINES) {
        return; // Ensure the line number is within range
    }

    _oled_write_command(0xB0 + line); // Set page address (0xB0 is the base for page 0, up to page 7)
    _oled_write_command(0x00);        // Set lower column address
    _oled_write_command(0x10);        // Set higher column address (start at column 0)
}

/**
 * @brief Sends a single character to the OLED by using the font and OLED protocol.
 * 
 * @param c Character to print
 */
/*
void oled_print_char(char c) {
    const uint8_t *char_bitmap = font_get_bitmap(c);  // Assuming font_get_bitmap() is defined to return the bitmap

    // Each character is represented by a series of bytes (e.g., 5x7 or 8x8 fonts).
    for (int i = 0; i < FONT_WIDTH; i++) {
        _oled_write_data(char_bitmap[i]);  // Send each column of the character to the OLED
    }
}
/**
 * @brief Prints a string on the OLED.
 * 
 * @param str The string to display
 */
/*
void oled_print(const char *str) {
    while (*str) {
        oled_print_char(*str++);  // Print each character in the string
    }
}

/**
 * @brief Displays "Hello World" on the OLED.
 */
/*
void oled_display_hello_world(void) {
    oled_goto_line(0);        // Start at first line
    oled_print("Hello World"); // Display the text
}
*/