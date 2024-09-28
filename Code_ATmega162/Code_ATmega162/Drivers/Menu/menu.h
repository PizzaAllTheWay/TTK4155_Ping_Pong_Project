/*
 * menu.h
 *
 * Created: 23/09/2024 18:23:17
 *  Author: Martynas
 */ 


#ifndef MENU_H_
#define MENU_H_

#include "Drivers/Menu/sprite_ghost.h"
#include "Drivers/Menu/sprite_nyan_cat.h"
#include "Drivers/Menu/sprite_bongo_cat.h"


int menu_loop ();

enum menu_state {
    PING_PONG, 
    BONGO_CAT, 
    NYAN_CAT, 
    GHOST,
    MAX_MENU_STATE = GHOST // Ensure this is always the last value
};

#endif /* MENU_H_ */