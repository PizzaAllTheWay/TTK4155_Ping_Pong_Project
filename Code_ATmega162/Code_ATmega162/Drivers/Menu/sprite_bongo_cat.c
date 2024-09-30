/*
 * sprite_bongo_cat.c
 *
 * Created: 28.09.2024 14:18:42
 *  Author: benja
 */ 

#include "sprite_bongo_cat.h"

void sprite_bongo_cat(){
	
	oled_set_sprite(sprite_bongo_cat_0, 8, 8, 63, 0); // 8x8 nibbles, 1 nibble = 8 pixels
	oled_set_sprite(sprite_bongo_cat_1, 8, 8, 63, 0); // 8x8 nibbles, 1 nibble = 8 pixels
	//oled_set_sprite(sprite_bongo_cat_inverted_0, 8, 8, 32, 0); // 8x8 nibbles, 1 nibble = 8 pixels
	//oled_set_sprite(sprite_bongo_cat_inverted_1, 8, 8, 32, 0); // 8x8 nibbles, 1 nibble = 8
	//oled_clear_screen();
	return;
}