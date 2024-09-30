/*
 * sprite_nyan_cat.c
 *
 * Created: 28.09.2024 15:27:38
 *  Author: benja
 */ 

#include "sprite_nyan_cat.h"



void nyan_cat(){
	oled_clear_screen();
	// Nyan Cat animation <3
	for (int8_t i = 0; i < 100; i+=1) {
		oled_set_sprite(sprite_nyan_cat, 8, 8, i, 0); // 8x8 nibbles, 1 nibble = 8 pixels
	}
	return;
	oled_clear_screen();
}