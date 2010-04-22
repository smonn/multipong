/*
 * init.c
 *
 *  Created on: Apr 15, 2009
 *      Author: alexander
 */

/* Local header files */
#include "../header/gfxinit.h"

int InitOpenGl(t_gamedata *game) {

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    game->screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_OPENGL);
      if ( !game->screen ) {
  		printf("Unable to set video mode: %s\n", SDL_GetError());
  		return 1;
  	}

    glClearColor( 1, 1, 1, 1 );
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glClear( GL_COLOR_BUFFER_BIT );
    return 0;
}

int InitSdl(t_gamedata *game) {
	if ( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

	if(TTF_Init() <0 ) {
	    printf("TTF_Init: %s\n", SDL_GetError());
	    return 1;
	}
    game->screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_HWSURFACE);
    if ( !game->screen ) {
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	if ( SDL_EnableKeyRepeat( 0, 0 )<0){
		printf("Unable to set keyrepeat: %s\n", SDL_GetError());
		return 1;
	}
	if( SDL_SetColorKey( game->screen, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0xFF00FF ) <0 ){
		printf("Unable to setcolorkey: %s", SDL_GetError());
		return -1;
	}
	SDL_EnableUNICODE(1);
    SDL_WM_SetCaption( "Multi Pong: Alla måste spela!", "Multi Pong" );
	SDL_ShowCursor( SDL_ENABLE );
	return 0;
}
