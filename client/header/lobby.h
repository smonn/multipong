/*
 * lobby.h
 *
 *  Created on: Apr 20, 2009
 *      Author: alexander
 */

#ifndef LOBBY_H_
#define LOBBY_H_

#include "gfxinit.h"
#include "menu.h"
#include "communication.h"
#include "gameplay.h"

#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_net.h>

int Lobby(t_gamedata *game, t_netdata *net);

int DrawLobby(TTF_Font *font, SDL_Surface *bgimage, char playernames[5][11],
		t_gamedata *game, SDL_Surface *star);

int DisplayWinner(int winner, TTF_Font *font, SDL_Surface *bgimage, char playernames[5][11],
		t_gamedata *game, SDL_Surface *star);
/* MOVED THIS TO GamePlay.h (and from Lobby.c to GamePlay.c
int unPackString(char *stringdata, char playernames[][11], char *begingame,
		int length, int *playeriid, int *nroplayers);
*/
#endif /* LOBBY_H_ */
