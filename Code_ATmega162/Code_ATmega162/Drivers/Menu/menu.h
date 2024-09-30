/*
 * menu.h
 *
 * Created: 23/09/2024 18:23:17
 *  Author: Martynas
 */ 


#ifndef MENU_H_
#define MENU_H_

#include "../UART/uart_driver.h"
#include "../Controls/controls.h"
#include "sprite_ghost.h"
//#include "sprite_nyan_cat.h"
//#include "sprite_bongo_cat.h"


int menu_loop ();

typedef enum {
    PING_PONG, 
    BONGO_CAT, 
    NYAN_CAT, 
    GHOST,
    MAX_MENU_STATE = GHOST // Ensure this is always the last value
} Menu_State;
int CURRENT_MENU_STATE;


#endif /* MENU_H_ */