/*
 * menu.h
 *
 *  Created on: Apr 15, 2009
 *      Author: alexander
 *
 *menu.h: Function for drawing a menu. Returns 0 when user exits program with ESC or clicks exit button.
 *returns 1 when user presses ENTER and input is OK.
 */

#ifndef MENU_H_
#define MENU_H_

/* Local header files */
#include "gfxinit.h"
#include "communication.h"

/* Unix header files */
#include <stdlib.h>

/* SDL and OpenGL header files */
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

#define UNICODE keyboardevent.key.keysym.unicode
#define NAMELENGTH 10
#define IPLENGTH 15
#define PORTLENGTH 5

int menu(t_gamedata *game);

int DrawMenu(Uint16 *menucolors, t_gamedata *game, TTF_Font *font,
		SDL_Surface *text, SDL_Surface *bgimage, SDL_Color *textcolor, int choice, char* name,
		char* serverip, char* port);

void SetColors(Uint16 *menucolors, t_gamedata *game, SDL_Color *textcolor);

int Cleanup(SDL_Surface *text, TTF_Font *font);

int Backspace(char *text, int charcount, int length);

int delete(char *text, int charcount, int length);

int WriteGameFile( t_gamedata *game );

int ReadGameFile( t_gamedata *game );

#endif /* MENU_H_ */
