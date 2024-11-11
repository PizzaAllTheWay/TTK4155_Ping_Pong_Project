/*
 * menu.c
 *
 * Created: 23/09/2024 18:23:01
 *  Author: Martynas
 */ 



#include "menu.h"



void menu_pingpong() {
	oled_clear_screen();
	oled_set_area_pattern(0xFF, 0, 0, 127, 0);
	oled_set_text(" Ping Pong ", 20, 0);
	oled_set_sprite(sprite_pingpong, 6, 6, 32, 1);
}

void menu_game_over() {
	oled_clear_screen();
	oled_set_sprite(sprite_game_over, 8, 8, 32, 0);
}

void menu_bongocat() {
	oled_clear_screen();
	oled_set_area_pattern(0xFF, 0, 0, 127, 0);
	oled_set_text(" Bongo Cat ", 20, 0);
	oled_set_sprite(sprite_bongo_cat_0, 7, 7, 32, 1);
}

void menu_nyancat() {
	oled_clear_screen();
	
	// Set rainbow background
	oled_set_area_pattern(0b00111111, 0, 2, 127, 2);
	oled_set_area_pattern(0b01111110, 0, 3, 127, 3);
	oled_set_area_pattern(0b11000000, 0, 4, 127, 4);
	oled_set_area_pattern(0b00000111, 0, 5, 127, 5);
	
	// SInce its to big we draw it first then overlay text on top
	oled_set_sprite(sprite_nyan_cat, 8, 8, 32, 0);
	
	oled_set_area_pattern(0xFF, 0, 0, 127, 0);
	oled_set_text(" Nyan Cat ", 20, 0);
}

void menu_ghost() {
	oled_clear_screen();
	oled_set_area_pattern(0xFF, 0, 0, 127, 0);
	oled_set_text(" Ghost ", 40, 0);
	oled_set_sprite(sprite_ghost, 4, 4, 40, 3);
}

void menu_pokemon() {
	oled_clear_screen();
	oled_set_area_pattern(0xFF, 0, 0, 127, 0);
	oled_set_text(" Pokemon ", 32, 0);
	
	oled_set_area_pattern(0xFF, 0, 1, 127, 1);
	oled_set_text(" Eevee ", 32, 1);
	
	oled_set_sprite(sprite_pokemon_eevee, 6, 6, 40, 2);
}



void menu_pingpont_game() {
	oled_clear_screen();
	oled_set_area_pattern(0xFF, 0, 0, 127, 0);
	oled_set_text(" Ping Pong ", 20, 0);
	
	oled_set_text("Score: ", 0, 2);
	oled_set_text("Hearts: ", 0, 3);
}

// For setting the score and hearts of the screen itself
// We dont need to update the whole screen and slow everything down
// We can just clear the nible instead and replace it, much faster :)
void menu_pingpont_set(uint8_t score, uint8_t hearts) {
	// Convert score and hearts to three-digit character arrays
	uint8_t text_score[4] = {0, 0, 0, 0};  // 3 digits + null terminator for safety
	uint8_t text_hearts[4] = {0, 0, 0, 0}; // 3 digits + null terminator for safety

	// Fill the arrays with ASCII characters (0x30 offset)
	text_score[0] = (score / 100) + 0x30;        // Hundreds place
	text_score[1] = ((score / 10) % 10) + 0x30;  // Tens place
	text_score[2] = (score % 10) + 0x30;         // Units place

	text_hearts[0] = (hearts / 100) + 0x30;       // Hundreds place
	text_hearts[1] = ((hearts / 10) % 10) + 0x30; // Tens place
	text_hearts[2] = (hearts % 10) + 0x30;        // Units place
	
	// Remove leading zeros for score
	if (score < 100) text_score[0] = ' '; // Replace with <space> if less than 100
	if (score < 10)  text_score[1] = ' '; // Replace with <space> if less than 10

	// Remove leading zeros for hearts
	if (hearts < 100) text_hearts[0] = ' '; // Replace with <space> if less than 100
	if (hearts < 10)  text_hearts[1] = ' '; // Replace with <space> if less than 10

	// Clear the area for score and hearts display (3 nibbles each)
	oled_set_text(text_score, 64, 2); // Display the score at position (64, 2)

	oled_set_text(text_hearts, 64, 3); // Display the hearts at position (64, 3)
}



void menu_bongocat_set(int8_t state) {
	switch (state) {
		case 0:
			oled_set_sprite(sprite_bongo_cat_0, 7, 7, 32, 1);
			break;
		case 1:
			oled_set_sprite(sprite_bongo_cat_1, 7, 7, 32, 1);
			break;
		default:
			break;
	}
}



void menu_nyancat_set(int8_t state) {
	switch (state) {
		case 0:
			oled_move_screen_stop();
			break;
		case 1:
			oled_move_screen(
				1,
				7,
				7,
				0,
				1
			);
			break;
		default:
			break;
	}
}

void menu_ghost_set(int8_t state) {
	switch (state) {
		case 0:
			oled_move_screen_stop();
			break;
		case 1:
			oled_move_screen(
				1,
				7,
				7,
				2,
				1
			);
			break;
		case 2:
			oled_move_screen(
				1,
				7,
				7,
				2,
				0
			);
			break;
		case 3:
			oled_move_screen(
				1,
				7,
				7,
				0,
				1
			);
			break;
		case 4:
			oled_move_screen(
				1,
				7,
				7,
				0,
				0
			);
			break;
		default:
			break;
	}
}


void menu_pokemon_set(int8_t state) {
	switch (state) {
		case 0:
			// Eevee
			oled_set_area_pattern(0xFF, 0, 1, 127, 1);
			oled_set_text(" Eevee ", 32, 1);
			oled_set_sprite(sprite_pokemon_eevee, 6, 6, 40, 2);
			break;
		case 1:
			// Pikachu
			oled_set_area_pattern(0xFF, 0, 1, 127, 1);
			oled_set_text(" Pikachu ", 32, 1);
			oled_set_sprite(sprite_pokemon_pikachu, 6, 6, 40, 2);
			break;
		case 2:
			// Snorlax
			oled_set_area_pattern(0xFF, 0, 1, 127, 1);
			oled_set_text(" Snorlax ", 32, 1);
			oled_set_sprite(sprite_pokemon_snorlax, 6, 6, 40, 2);
			break;
		case 3:
			// Mewtwo
			oled_set_area_pattern(0xFF, 0, 1, 127, 1);
			oled_set_text(" Mewtwo ", 32, 1);
			oled_set_sprite(sprite_pokemon_mewtwo, 6, 6, 40, 2);
			break;
		case 4:
			// Mew
			oled_set_area_pattern(0xFF, 0, 1, 127, 1);
			oled_set_text(" Mew ", 32, 1);
			oled_set_sprite(sprite_pokemon_mew, 6, 6, 40, 2);
			break;
		case 5:
			// Lapras
			oled_set_area_pattern(0xFF, 0, 1, 127, 1);
			oled_set_text(" Lapras ", 32, 1);
			oled_set_sprite(sprite_pokemon_lapras, 6, 6, 40, 2);
			break;
		case 6:
			// Gengar
			oled_set_area_pattern(0xFF, 0, 1, 127, 1);
			oled_set_text(" Gengar ", 32, 1);
			oled_set_sprite(sprite_pokemon_gengar, 6, 6, 40, 2);
			break;
		case 7:
			// Squirtle
			oled_set_area_pattern(0xFF, 0, 1, 127, 1);
			oled_set_text(" Squirtle ", 32, 1);
			oled_set_sprite(sprite_pokemon_squirtle, 6, 6, 40, 2);
			break;
		case 8:
			// Bulbasaur
			oled_set_area_pattern(0xFF, 0, 1, 127, 1);
			oled_set_text(" Bulbasaur ", 32, 1);
			oled_set_sprite(sprite_pokemon_bulbasaur, 6, 6, 40, 2);
			break;
		case 9:
			// Charmander
			oled_set_area_pattern(0xFF, 0, 1, 127, 1);
			oled_set_text(" Charmander ", 32, 1);
			oled_set_sprite(sprite_pokemon_charmander, 6, 6, 40, 2);
			break;
		case 10:
			// Ditto
			oled_set_area_pattern(0xFF, 0, 1, 127, 1);
			oled_set_text(" Ditto ", 32, 1);
			oled_set_sprite(sprite_pokemon_ditto, 6, 6, 40, 2);
			break;
		default:
			break;
	}
}



void menu_init() {
	oled_init();
	menu_pingpong();
}
