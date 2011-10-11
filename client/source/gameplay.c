/*
 * GamePlay.c
 *
 *  Created on: Apr 21, 2009
 *      Author: alexander
 */
/* Local header files */
#include "../header/gameplay.h"

/* Unix header files */
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>

/* SDL and OpenGL header files */
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_net.h>


int GamePlay( t_gamedata *game, t_netdata *net, char playernames[5][11],
		int nroplayers, int myid ){

	SDL_Event keyboardevent;
	GLuint texture;
	Uint8 *keystate;
	t_playerpositions positions[6];
	t_gamestats gamestats;
	t_paddleinfo paddleinfo;
	TTF_Font *font;
	char winner[19];
	int k;
	int protate = 0;
	int direction = 0;
	SDL_Color textcolor[2];
	float lastframe = 0,thisframe = 0;
	int ff, fps;
	const Uint32 game_time_updatelength = 15;
	Uint32 game_time_update_last = 0;

	font = TTF_OpenFont("./assets/font.ttf", 30);

		if (font == NULL) {
		    printf("Unable to load font: %s \n", SDL_GetError());
		    return -1;
		}

	textcolor[0].g = 255;
	textcolor[0].r = 0;
	textcolor[0].b = 0;
	textcolor[1].g = 0;
	textcolor[1].r = 255;
	textcolor[1].b = 0;

	memset(&gamestats.lifes[0], 0, sizeof(gamestats.lifes));
	for(k = 0; k<=5; k++){
		positions[k].x = 0;
		positions[k].y = 0;
	}

	InitOpenGl( game );
	CreateTexture( &texture );

	gamestats.winner = -1 ;
	SetupPaddles( &paddleinfo, nroplayers );
	protate = RotatePlayers(myid, nroplayers);

	strcpy(net->stringip,game->serverip);
	if( CreateUdpSocket( net, game ) <0 ){
		return -1;
	}

	while( gamestats.winner < 0 ){
		while (SDL_PollEvent(&keyboardevent)) {
			if (keyboardevent.type == SDL_QUIT
					|| (keyboardevent.type == SDL_KEYDOWN
					&& keyboardevent.key.keysym.sym == SDLK_ESCAPE)) {
					/*
					 * Add to send "im leaving to server"
					 */
				glDeleteTextures( 1, &texture );
				TTF_CloseFont(font);
				return 0;
			}
		}
        
		if ( ReceiveUDPMessage(net, &gamestats, positions) < 0)
			return -1;
			/*Reduces fps to ~60 */
		   if ( (SDL_GetTicks() - game_time_update_last) > game_time_updatelength ){

			   SDL_GL_SwapBuffers();
			   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
			   DrawGameBG( nroplayers, texture );
		  	   DrawPlayer(nroplayers, texture, myid, gamestats, positions, paddleinfo );
		       DrawLifes ( gamestats, nroplayers, myid, texture );
		       DrawBall( positions, texture );
		       DrawCorners ( nroplayers );

		       if( SendUDPMessage(net, direction) < 0 )
		       return -1;

		       CheckWinner( &gamestats );
		      if (gamestats.winner >= 0){
		    	  SDLNet_TCP_Close(net->tcpsocket);
		    	  sprintf(winner,"Winner: %s", playernames[gamestats.winner]);
		    	  glShadeModel(GL_SMOOTH);
		    	  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		    	  glClearDepth(1.0f);
		    	  glDepthFunc(GL_LEQUAL);
		    	  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   	 	    	  glBlendFunc(GL_ONE, GL_ONE);
				  glEnable(GL_BLEND);
		    	  DrawText( winner, font, textcolor[0], 400, 250);
		    	  SDL_GL_SwapBuffers();
		    	  SDL_Delay(7000);
		      }

		       game_time_update_last = SDL_GetTicks();

		       /*FPS*/
		       thisframe=SDL_GetTicks();
		       ff++;
		       if((thisframe-lastframe) > 1000) {
		    	   fps=ff;
		    	   ff=0;
		    	   lastframe=thisframe;
		       }
		    /*   printf("FPS: %d\n",fps); */
		       /*
				sprintf(cfps, "%d", fps );
				SDL_WM_SetCaption( cfps, "Multi Pong" );
		        */
		   }

		keystate = SDL_GetKeyState( NULL );
		GetDirection( keystate, &direction, myid, nroplayers);
	/*	printf("Direction: %d\n",direction); */

	}

	glDeleteTextures( 1, &texture );
	TTF_CloseFont(font);

	return gamestats.winner;
}

int DrawGameBG( int nroplayers, GLuint texture){

		glBindTexture( GL_TEXTURE_2D, texture );
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		glPushMatrix();
		glTranslatef( 0, 0, 0.0 );
		glBegin( GL_QUADS );
			glTexCoord2f( 0, 0 );
			glVertex2i  ( 0, 0  );

			glTexCoord2f( 0, (600.0 / 1024.0) );
			glVertex2i  ( 0, 600 );

			glTexCoord2f( (800.0 / 1024.0), (600.0 / 1024.0) );
			glVertex2i  ( 800, 600 );

			glTexCoord2f( (800.0 / 1024.0), 0 );
			glVertex2i  ( 800, 0  );
		glEnd();
		glPopMatrix();

	if( nroplayers == 2 || nroplayers == 4 ){
		glPushMatrix();
		glDisable(GL_TEXTURE_2D);
		glTranslatef( 0, 0, 0.1 );
		glColor4f( 0.3f, 0.3f, 0.2f, 0.9f );
		glBegin( GL_QUADS );
			glVertex2i( 210, 10  );
			glVertex2i( 210, 590 );
			glVertex2i( 790, 590 );
			glVertex2i( 790, 10  );
		glEnd();
		glEnable(GL_TEXTURE_2D);
		glPopMatrix();
		return 0;
	}

	else if( nroplayers == 3 ){
		glPushMatrix();
		glDisable(GL_TEXTURE_2D);
		glColor4f( 0.3f, 0.3f, 0.2f, 0.9f );
		glTranslatef( 0.0f, 0.0f, 0.1f );
		glBegin( GL_TRIANGLES );
			glVertex2i( 500, 49 );
			glVertex2i( 210, 551 );
			glVertex2i( 790, 551 );
		glEnd();
		glEnable(GL_TEXTURE_2D);
		glPopMatrix();
		return 0;
	}

	else if( nroplayers == 5 ){
		glPushMatrix();
		glDisable(GL_TEXTURE_2D);
		glColor4f( 0.3f, 0.3f, 0.2f, 0.9f );
		glTranslatef( 0, 0, 0.1 );
		glBegin( GL_TRIANGLES );
				glVertex2i( 500, 10  );
				glVertex2i( 210, 220 );
				glVertex2i( 790, 220 );

				glVertex2i( 790, 220  );
				glVertex2i( 210, 220 );
				glVertex2i( 330, 560 );

				glVertex2i( 330, 560  );
				glVertex2i( 790, 220 );
				glVertex2i( 680, 560 );
		glEnd();
		glEnable(GL_TEXTURE_2D);
		glPopMatrix();
		return 0;
	}

	return 0;
}

int DrawPlayer( int nroplayers, GLuint texture, int myid,t_gamestats gamestats,
		t_playerpositions *positions, t_paddleinfo paddleinfo ){
	int k;
	float rotate=0;

	glBindTexture( GL_TEXTURE_2D, texture );
	for( k = 0; k < nroplayers; k++ ){
		glPushMatrix();
		if( k == myid)
			glColor4f( 1.0f, 0.5f, 1.0f, 1.0f );
		else
			glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

		glTranslatef(positions[k].x, positions[k].y, 0.2);
		glRotatef( rotate, 0, 0, 1 );
		if( gamestats.lifes[k] > 0){
		glBegin( GL_QUADS );
			glTexCoord2f( 0, (608.0 / 1024.0) );
			glVertex2f( -paddleinfo.length / 2, paddleinfo.width / 2 );

			glTexCoord2f( 0, (638.0 / 1024.0) );
			glVertex2f( -paddleinfo.length / 2, -paddleinfo.width / 2 );

			glTexCoord2f( ( 169.0 / 1024.0), (638 / 1024.0) );
			glVertex2f( paddleinfo.length / 2, -paddleinfo.width / 2);

			glTexCoord2f( (169.0 / 1024.0), (608.0 / 1024.0) );
			glVertex2f( paddleinfo.length / 2 , paddleinfo.width / 2 );
		glEnd();
		}
		if( nroplayers == 2)
			rotate = 0;
		else if ( nroplayers == 3){
			if( k == 0 ){
				rotate = -60.0;
			}
			else if( k == 1 ){
				rotate = 60.0;
			}
		}
		else if( nroplayers == 4 ){
			rotate += 90;
		}
		else if (nroplayers == 5)
			rotate += 72;
		glPopMatrix();
	}
	return 0;

}

int DrawBall( t_playerpositions *positions, GLuint texture )	{
	float r = 7.5;

	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, texture );
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glPushMatrix();
	glTranslatef( (float)positions[5].x - r, (float)positions[5].y - r, 0.6 );
	glBegin(GL_QUADS);
			glTexCoord2f( 0.0f, 659.0/1024.0f );
			glVertex2i( 0.0f, 0.0f);
			glTexCoord2f( 0.0f, 674.0/1024.0f );
			glVertex2i( 0.0f, 15.0f);
			glTexCoord2f( 15.0f / 1024.0f, 674.0f/1024.0f );
			glVertex2i( 15.0f, 15.0f);
			glTexCoord2f( 15.0f / 1024.0f, 659.0f/1024.0f );
			glVertex2i( 15.0f, 0.0f);
	glEnd();
	glPopMatrix();
	return 0;
}

void DrawCorners( int nroplayers ){
	float r = 15.0;
	int k;
	float delta_theta = 0.01;
	float angle, x,y;
	glDisable(GL_TEXTURE_2D);
	if( nroplayers == 2 || nroplayers == 4){
		x = 210.0;
		y = 10.0;
		for( k = 1; k <= 5; k++ ){
			glPushMatrix();
			glColor4f( 0.1f, 0.1f, 0.1f, 1.0f );
			glTranslatef( x, y, 0.7f );
			glBegin( GL_POLYGON );
			for( angle = 0 ; angle < (2 * 3.14159) ; angle += delta_theta )
				glVertex3f( r * cos(angle), r * sin(angle), 0);
			glEnd();
			if(k == 2){
				x = 210.0;
				y = 590.0;
			}
			else if (k == 3){
				x = 790.0;
				y = 590.0;
			}
			else if (k == 4){
				x = 790.0;
				y = 10.0;
			}
			glPopMatrix();
		}
	}
	else if( nroplayers == 3){
		x = 500.0;
		y = 49.0;
		for( k = 1; k<=4; k++ ){
			glPushMatrix();
			glTranslatef( x, y, 0.7f );
			glColor4f( 0.1f, 0.1f, 0.1f, 1.0f );
			glBegin( GL_POLYGON );
				for( angle = 0 ; angle < (2 * 3.14159) ; angle += delta_theta )
					glVertex3f( r * cos(angle), r * sin(angle), 0);
			glEnd();
			if(k == 2){
				x = 210.0;
				y = 551.0;
			}
			else if (k == 3){
				x = 790.0;
				y = 551.0;
			}
			glPopMatrix();
		}
	}

	else if( nroplayers == 5){
		x = 330.0;
		y = 560.0;
		for( k = 1; k <= 6; k++ ){
			glPushMatrix();
			glColor4f( 0.1f, 0.1f, 0.1f, 1.0f );
			glTranslatef( x, y, 0.7f );
			glBegin( GL_POLYGON );
			for( angle = 0 ; angle < (2 * 3.14159) ; angle += delta_theta )
				glVertex3f( r * cos(angle), r * sin(angle), 0);
			glEnd();
			if(k == 2){
				x = 210.0;
				y = 220.0;
			}
			else if (k == 3){
				x = 500.0;
				y = 10.0;
			}
			else if (k == 4){
				x = 790.0;
				y = 220.0;
			}
			else if (k == 5){
				x = 680.0;
				y = 560.0;
			}
			glPopMatrix();
		}
	}

	glEnable(GL_TEXTURE_2D);

}

int CreateTexture( GLuint *texture )	{
	SDL_Surface *surface;
	if ( (surface = IMG_Load ("./assets/gametextures24.png")) ) {
		if ( (surface->w & (surface->w - 1)) != 0 ) {
			printf("warning: gametextures.png width is not a power of 2\n");
		}
			if ( (surface->h & (surface->h - 1)) != 0 ) {
			printf("warning: gametextures.png height is not a power of 2\n");
		}
		glGenTextures( 1, texture );
		glBindTexture( GL_TEXTURE_2D, *texture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D( GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
	}
	else {
		printf("SDL could not load gametextures.png : %s\n", SDL_GetError());
		SDL_FreeSurface( surface );
		return -1;
	}
	SDL_FreeSurface( surface );
	return 0;
}

int SetupPaddles( t_paddleinfo *paddleinfo, int nroplayers)	{
	if( nroplayers == 2 || nroplayers == 4){
		paddleinfo->length = 45;
		paddleinfo->width  = 8;
	}

	else if( nroplayers == 3){
		paddleinfo->length = 45;
		paddleinfo->width  = 8;
	}

	else{
		paddleinfo->length = 40;
		paddleinfo->width = 8;
	}
	return 0;
}

int DrawLifes( t_gamestats gamestats, int nroplayers, int myid, GLuint texture ){
	int k,l;
	float x,y;
	/*
	gamestats.lifes[0] = 3;
	gamestats.lifes[1] = 3;
	gamestats.lifes[2] = 3;
	gamestats.lifes[3] = 3;
	*/
	glBindTexture( GL_TEXTURE_2D, texture );
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glScalef( 1.0f, 1.0f, 1.0f );
	if( nroplayers == 2){
		x = 210;
		y = 565;
		for( k = 0; k < nroplayers; k++ ){
			for( l = 0; l < gamestats.lifes[k]; l++){
				glPushMatrix();
				glTranslatef( x, y, 0.3 );
				glBegin( GL_QUADS );
					glTexCoord2f( 0, (680.0 / 1024.0) );
					glVertex2i( l * 15, k * 15   );
					glTexCoord2f( 0, (710.0 / 1024.0) );
					glVertex2i( l * 15, (k + 1) * 15 );
					glTexCoord2f( ( 30.0 / 1024.0), (710.0 / 1024.0) );
					glVertex2i( (l + 1) * 15, (k + 1) * 15 );
					glTexCoord2f( (30.0 / 1024.0), (680.0 / 1024.0) );
					glVertex2i( (l + 1) * 15  , k * 15  );
				glEnd();
				glPopMatrix();
			}
		x = 210;
		y = 0;
		}
	}
	if( nroplayers == 3){
		x = 380;
		y = 550;
		for( k = 0; k < nroplayers; k++ ){
			for( l = 0; l < gamestats.lifes[k]; l++){
				glPushMatrix();
				glTranslatef( x, y, 0.3 );
				glBegin( GL_QUADS );
					glTexCoord2f( 0, (680.0 / 1024.0) );
					glVertex2i( l * 15, k * 15   );
					glTexCoord2f( 0, (710.0 / 1024.0) );
					glVertex2i( l * 15, (k + 1) * 15 );
					glTexCoord2f( ( 30.0 / 1024.0), (710.0 / 1024.0) );
					glVertex2i( (l + 1) * 15, (k + 1) * 15 );
					glTexCoord2f( (30.0 / 1024.0), (680.0 / 1024.0) );
					glVertex2i( (l + 1) * 15  , k * 15  );
				glEnd();
				glPopMatrix();
			}
			if( k == 0 ){
				x = 210;
				y = 300;
			}
			else if ( k == 1 ){
				x = 700;
				y = 300;
			}
		}
	}
	if( nroplayers == 4){
		x = 260;
		y = 565;
		for( k = 0; k < nroplayers; k++ ){
			for( l = 0; l < gamestats.lifes[k]; l++){
				glPushMatrix();
				glTranslatef( x, y, 0.3 );
				glBegin( GL_QUADS );
					glTexCoord2f( 0, (680.0 / 1024.0) );
					glVertex2i( l * 15, 0 );
					glTexCoord2f( 0, (710.0 / 1024.0) );
					glVertex2i( l * 15, 15 );
					glTexCoord2f( ( 30.0 / 1024.0), (710.0 / 1024.0) );
					glVertex2i( (l + 1) * 15, 15 );
					glTexCoord2f( (30.0 / 1024.0), (680.0 / 1024.0) );
					glVertex2i( (l + 1) * 15  , 0 );
				glEnd();
				glPopMatrix();
			}
			if ( k == 0 ){
				x = 135;
				y = 285;
			}
			if ( k == 1 ){
				x = 210;
				y = 0;
			}
			if ( k == 2 ){
				x = 700;
				y = 0;
			}
		}
	}
	if( nroplayers == 5){
		x = 500;
		y = 565;
		for( k = 0; k < nroplayers; k++ ){
			for( l = 0; l < gamestats.lifes[k]; l++){
				glPushMatrix();
				glTranslatef( x, y, 0.3 );
				glBegin( GL_QUADS );
					glTexCoord2f( 0, (680.0 / 1024.0) );
					glVertex2i( l * 15, 0 );
					glTexCoord2f( 0, (710.0 / 1024.0) );
					glVertex2i( l * 15, 15 );
					glTexCoord2f( ( 30.0 / 1024.0), (710.0 / 1024.0) );
					glVertex2i( (l + 1) * 15, 15 );
					glTexCoord2f( (30.0 / 1024.0), (680.0 / 1024.0) );
					glVertex2i( (l + 1) * 15  , 0 );
				glEnd();
				glPopMatrix();
			}
			if ( k == 0 ){
				x = 155;
				y = 295;
			}
			if ( k == 1 ){
				x = 210;
				y = 105;
			}
			if ( k == 2 ){
				x = 700;
				y = 105;
			}
			if ( k == 3 ){
				x = 720;
				y = 450;
			}
		}
	}


	return 0;
}

void DrawText(char *text, TTF_Font *font, SDL_Color color,
		int x, int y)	{

		SDL_Surface *initial;
		SDL_Surface *intermediary;
		int w,h;
		GLuint texture;

		glTranslatef(0.0f, 0.0f, 0.8f );

		initial = TTF_RenderText_Blended(font, text, color);

		w = NextPowerOfTwo(initial->w);
		h = NextPowerOfTwo(initial->h);

		intermediary = SDL_CreateRGBSurface(0, w, h, 32,
				0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

		SDL_BlitSurface(initial, 0, intermediary, 0);


		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA,
				GL_UNSIGNED_BYTE, intermediary->pixels );

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		glColor3f(1.0f, 1.0f, 1.0f);


		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(x, y);
			glTexCoord2f(1.0f, 0.0);
			glVertex2f(x + w, y);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(x + w, y + h);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(x, y + h);
		glEnd();

		glFinish();

		SDL_FreeSurface(initial);
		SDL_FreeSurface(intermediary);
		glDeleteTextures(1, &texture);
}

int NextPowerOfTwo(int x) {
	double logbase2 = log(x) / log(2);
	return RoundD(pow(2,ceil(logbase2)));
}

int RoundD(double x){
	return (int)(x + 0.5);
}

int UnpackString(char *stringdata, char playernames[][11], char *begingame,
		int length, int *playeriid, int *nroplayers, t_gamestats *gamestats){
	int k,l=0,rad=0;

	printf("length:  %d\n",length);
	stringdata[length+1] = ':';
	printf("string:  %s\n", stringdata);

	if(stringdata[0] == '4' ){
		*playeriid = ( stringdata[3] - 48 );
		printf("player id: %d\n", *playeriid );

		if( *playeriid == 5){
			printf("Server sent server full: closing client.\n");
			return 0;
		}
	}

	else if(stringdata[0] == '3' ){
		switch (stringdata[3]) {
			case '1':
				*begingame = '1';
				*nroplayers = (stringdata[4] - 48 );
				printf("Server sent start signal. Starting game.\n");
				break;
			case '2':
				printf("Server sent server full: closing client.\n");
				return 0;
				break;
			case '3':
				printf("Server sent quit: closing client.\n");
				return 0;
				break;
			case '4':
				return 1;
			case '5':
				/*gamestats->winner = (stringdata[4] - 48 );
				return 66; */
				break;
		}
	}
	else if( stringdata[0] == '2' ){
		for( k = 3; k <= length; k++ ){
			if( stringdata[k] == ':'){
				playernames[rad][l+1] = '\0';
				rad++;
				if( rad == 5 )
					break;
				l = 0;
				k++;
			}
				playernames[rad][l]=stringdata[k];
			l++;
		}
	}
	for( k = 0; k< 5; k++){
		if ( playernames[k][0] == '\0' || playernames[k][0] == ':' )
			strcpy(playernames[k],"-- N/A --");
			printf("name %d: %s\n",k, playernames[k]);
	}
	return 1;
}

int CheckWinner( t_gamestats *gamestats){
	int k;
	int count = 0;
	for(k = 0; k<=4; k++ ){
		if( gamestats->lifes[k] > 0 )
			count++;
	}
	if(count == 1){
		for( k = 0; k<=4; k++)
			if(gamestats->lifes[k] > 0)
				gamestats->winner = k;
	}
	return 0;
}

int GetDirection( Uint8 *keystate, int *direction, int myid, int nroplayers){
	if( myid == 0){
		if( keystate[SDLK_LEFT] == 1 || keystate[SDLK_UP] == 1 )
			*direction = 1;
		else if( keystate[SDLK_RIGHT] == 1 ||keystate[SDLK_DOWN] == 1)
			*direction = 2;
		else
			*direction = 0;
	}

	else if( myid == 1 && nroplayers == 2){
		if( keystate[SDLK_LEFT] == 1 || keystate[SDLK_UP] == 1 )
			*direction = 2;
		else if( keystate[SDLK_RIGHT] == 1 ||keystate[SDLK_DOWN] == 1)
			*direction = 1;
		else
			*direction = 0;
	}
	else if( myid == 1 ){
		if( keystate[SDLK_LEFT] == 1 || keystate[SDLK_UP] == 1 )
			*direction = 1;
		else if( keystate[SDLK_RIGHT] == 1 ||keystate[SDLK_DOWN] == 1)
			*direction = 2;
		else
			*direction = 0;
	}
	else if( myid == 2 ){
			if( keystate[SDLK_LEFT] == 1 || keystate[SDLK_UP] == 1 )
				*direction = 2;
			else if( keystate[SDLK_RIGHT] == 1 ||keystate[SDLK_DOWN] == 1)
				*direction = 1;
			else
				*direction = 0;
		}

	else if( myid == 2 && nroplayers == 3 ){
		if( keystate[SDLK_LEFT] == 1 || keystate[SDLK_UP] == 1 )
			*direction = 1;
		else if( keystate[SDLK_RIGHT] == 1 ||keystate[SDLK_DOWN] == 1)
			*direction = 2;
		else
			*direction = 0;
	}
	else if( myid == 2 && nroplayers == 4 ){
		if( keystate[SDLK_LEFT] == 1 || keystate[SDLK_UP] == 1 )
			*direction = 2;
		else if( keystate[SDLK_RIGHT] == 1 ||keystate[SDLK_DOWN] == 1)
			*direction = 1;
		else
			*direction = 0;
	}
	else if( myid == 3 ){
			if( keystate[SDLK_LEFT] == 1 || keystate[SDLK_UP] == 1 )
				*direction = 1;
			else if( keystate[SDLK_RIGHT] == 1 ||keystate[SDLK_DOWN] == 1)
				*direction = 2;
			else
				*direction = 0;
		}
	else if( myid == 4){
			if( keystate[SDLK_LEFT] == 1 || keystate[SDLK_UP] == 1 )
				*direction = 1;
			else if( keystate[SDLK_RIGHT] == 1 ||keystate[SDLK_DOWN] == 1)
				*direction = 2;
			else
				*direction = 0;
		}
	return 0;
}

int RotatePlayers( int myid, int nroplayers){
	int rotate = 0;

	if(nroplayers == 2)
		rotate = myid * 180;

	if(nroplayers == 3)
		rotate = myid * 120;

	if(nroplayers == 4)
		rotate = myid * 90;

	if(nroplayers == 5)
		rotate = myid * 72;


	return rotate;
}
