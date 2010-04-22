/*
 * GamePlay.h
 *
 *  Created on: Apr 21, 2009
 *      Author: alexander
 */

#ifndef GAMEPLAY_H_
#define GAMEPLAY_H_

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
#include <SDL/SDL_net.h>

typedef struct paddleinfo{
	int length;
	int width;
}t_paddleinfo;

int GamePlay( t_gamedata *game, t_netdata *net, char playernames[5][11],
		int nroplayers, int myid );

int DrawGameBG( int nroplayers, GLuint texture );

int DrawPlayer( int nroplayers, GLuint texture, int myid,t_gamestats gamestats,
		t_playerpositions *positions, t_paddleinfo paddleinfo);

int DrawBall( t_playerpositions *positions, GLuint texture );

int CreateTexture( GLuint *texture );

int SetupPaddles( t_paddleinfo *paddleinfo, int nroplayers );

int DrawLifes( t_gamestats gamestats, int nroplayers, int myid, GLuint texture );

int NextPowerOfTwo(int x);

int RoundD(double x);

int UnpackString(char *stringdata, char playernames[][11], char *begingame,
		int length, int *playeriid, int *nroplayers, t_gamestats *gamestats);

void DrawCorners( int nroplayers );

int CheckWinner( t_gamestats *gamestats);

int GetDirection( Uint8 *keystate, int *direction, int myid, int nroplayers);

int RotatePlayers( int myid, int nroplayers);

void DrawText(char *text, TTF_Font *font, SDL_Color color,
		int x, int y);

#endif /* GAMEPLAY_H_ */
