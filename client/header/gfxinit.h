/*
 * init.h
 *
 *  Created on: Apr 15, 2009
 *      Author: alexander
 *
 *init.h: initiates opengl and sld.
 */

#ifndef GFXINIT_H_
#define GFXINIT_H_

/* Unix header files */
#include <stdlib.h>

/* SDL and OpenGL header files */
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GAME_SPEED 30

/*
 * Struct for storing gamedata.
 */
typedef struct {
	SDL_Surface *screen;
	char serverip[16];
	char name[11];
	char port[6];
} t_gamedata;

/*
 * Function for initilizing opengl.
 */
int InitOpenGl();

/*
 * Function for initilizing SDL.
 */
int InitSdl();

#endif /* GFXINIT_H_ */
