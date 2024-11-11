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



void menu_pingpont_set(uint8_t score) {
	oled_clear_screen();
	oled_set_area_pattern(0xFF, 0, 0, 127, 0);
	oled_set_text(" Ping Pong ", 20, 0);
	
	oled_set_text("Hearts: ", 0, 1);
	
	// Need a 0x30 offset as that is where the letters start
	uint8_t text_score[2] = {0, 0};
	text_score[0] = score + 0x30;
	oled_set_text(&text_score, 61, 1);
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
		default:
			break;
	}
}



void menu_init() {
	oled_init();
	menu_pingpong();
}
