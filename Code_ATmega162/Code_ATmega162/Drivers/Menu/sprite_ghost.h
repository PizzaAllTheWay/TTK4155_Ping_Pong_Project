/*
 * ghost_sprite.h
 *
 * Created: 23/09/2024 18:24:06
 *  Author: Martynas
 */ 


#ifndef SPRITE_GHOST_H_
#define SPRITE_GHOST_H_



#include <avr/pgmspace.h>
#include <util/delay.h>
#include "../OLED/oled.h"


// Ghost Sprite 32x32
const uint8_t PROGMEM sprite_ghost_binary[4][32];

void sprite_ghost();




#endif /* SPRITE_GHOST_H_ */