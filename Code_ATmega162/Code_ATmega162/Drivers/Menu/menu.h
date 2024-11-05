/*
 * menu.h
 *
 * Created: 23/09/2024 18:23:17
 *  Author: Martynas
 */ 


#ifndef MENU_H_
#define MENU_H_



#include "../OLED/oled.h"

#include "sprite_pingpong.h"
#include "sprite_bongo_cat.h"
#include "sprite_nyan_cat.h"
#include "sprite_ghost.h"
#include "sprite_pokemon.h"



void menu_pingpong();
void menu_bongocat();
void menu_nyancat();
void menu_ghost();
void menu_pokemon();

void menu_pingpont_set(uint8_t score);
void menu_bongocat_set(int8_t state);
void menu_nyancat_set(int8_t state);
void menu_ghost_set(int8_t state);
void menu_pokemon_set(int8_t state);

void menu_init();



#endif /* MENU_H_ */