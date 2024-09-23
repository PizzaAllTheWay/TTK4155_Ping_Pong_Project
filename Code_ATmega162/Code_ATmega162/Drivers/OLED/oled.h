/*
 * oled_driver.h
 *
 * Created: 23/09/2024 01:57:19
 *  Author: Martynas
 */ 


#ifndef OLED_DRIVER_H_
#define OLED_DRIVER_H_



// NOTE: In order for External SRAM, ADC and OLED to function properly this initialization MUST stay here
// This initialization configures the data and register buss between them
#include "../External_SRAM/external_sram.h"

// Fonts library for bit mapping in OLED Controller SSD1308
// This makes it easier to display characters on OLED without witting 8 lines of code for each letter
#include "fonts.h"



#define OLED_COMMAND_ADDRESS  0x1000
#define OLED_DATA_ADDRESS 0x1200
#define OLED_MAX_LINES 8  // There are 8 pages/lines on a 128x64 display (8 bits per segment => 8 segments per letter => 16 letters per line(page) => 8 lines(pages) per screen)
#define OLED_PIXEL_WIDTH 128



void oled_set_letter(uint8_t letter);
void oled_clear_page(uint8_t page_rn);
void oled_clear_creen();

void oled_init();



#endif /* OLED_DRIVER_H_ */