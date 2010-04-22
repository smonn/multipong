/*
 * Lobby.c
 *
 *  Created on: Apr 20, 2009
 *      Author: alexander
 */
/* Local header files */
#include "../header/lobby.h"

int Lobby(t_gamedata *game, t_netdata *net){
	SDL_Event keyboardevent;
	Uint16 menucolors[5];
	SDL_Surface *bgimage, *star;
	TTF_Font *font;
	SDL_Color textcolor[3];
	int gamestarted = 1;
	int nroplayers;
	int playeriid;
	int winner = -1;
	char playernames[5][11];
	char recievebuffer[100];
	char sendbuffer[30];
	int  result;
	char begingame = '0';
	int socketready;

	memset( &playernames[0],	 0, sizeof(playernames)   );
	strcpy( playernames[0], " ");
	strcpy( playernames[1], " ");
	strcpy( playernames[2], " ");
	strcpy( playernames[3], " ");
	strcpy( playernames[4], " ");

	SetColors( menucolors, game, textcolor);

	font = TTF_OpenFont("./assets/font.ttf", 20);

	if (font == NULL) {
	    printf("Unable to load font: %s \n", SDL_GetError());
	    return -1;
	}

	bgimage = IMG_Load("./assets/lobbybg.png");

	if( !bgimage ) {
		printf("IMG_Load: %s\n", SDL_GetError() );
		return -1;
	}

	star = IMG_Load("./assets/star.png");

	if( !star ) {
			printf("IMG_Load: %s\n", SDL_GetError() );
			return -1;
		}

	if( CreateTcpSocket( net, game ) <0 )
		return -1;
	memset(&sendbuffer[0], 0, sizeof(sendbuffer) );

	strncat(sendbuffer, "210", 3 );
	strncat(sendbuffer, game->name, 10 );

	printf("sendbuffer:%s \nsendlength: %d\n", sendbuffer, (int) strlen(sendbuffer));

	if( SDLNet_TCP_Send( net->tcpsocket, sendbuffer, strlen(sendbuffer)+1 )
			< strlen(game->name)+1 )	{
		printf("Error sending name.\n");
		return -1;
	}

	while( gamestarted == 1){
		while (SDL_PollEvent(&keyboardevent)) {
			if (keyboardevent.type == SDL_QUIT
				|| (keyboardevent.type == SDL_KEYDOWN
				&& keyboardevent.key.keysym.sym == SDLK_ESCAPE))
					return 0;
			if ((keyboardevent.type == SDL_KEYDOWN
				&& keyboardevent.key.keysym.sym == SDLK_SPACE
				&& playeriid == 0 )){
				if( SDLNet_TCP_Send( net->tcpsocket, "303101", 7 )
							< strlen("303101")){
					printf("Error sending start.\n");
					return -1;
				}
				else
					printf("Sent start to server.\n");
			}

		}

		memset(&recievebuffer[0], 0, sizeof(recievebuffer) );
		socketready = SDLNet_CheckSockets( net->socketset, 100 );
		if( socketready == -1 )	{
			printf("SDLNet_CheckSockets: %s\n",SDL_GetError());
			return -1;
		}
		if( socketready > 0 && SDLNet_SocketReady(net->tcpsocket) > 0 )	{
			result = SDLNet_TCP_Recv( net->tcpsocket, &recievebuffer,  MSG_MAXLEN - 1  );
			if( result <= 0 ){
				printf("Error with recieve on tcp socket: %s\n", SDL_GetError());
				return -1;
			}
			else{
				gamestarted = UnpackString( recievebuffer, playernames,
						&begingame, result, &playeriid, &nroplayers, NULL);
			}
		}
		DrawLobby( font, bgimage, playernames, game, star );
		if( begingame == '1'){
			winner = GamePlay( game, net, playernames, nroplayers, playeriid);
			printf("Winner: %s\nExiting game.\n",playernames[winner]);
			fflush(stdout);
			gamestarted = 0;
		}
		SDL_UpdateRect(game->screen, 0, 0, 0, 0);
		SDL_Delay(16);
	}

	SDL_FreeSurface( bgimage );
    SDL_FreeSurface( star );
	CloseSocket( net );
	return 0;
}

int DrawLobby(TTF_Font *font, SDL_Surface *bgimage, char playernames[5][11],
		t_gamedata *game, SDL_Surface *star){

	SDL_Rect destrect;
	SDL_Surface *text;
	SDL_Color textcolor[2];
	int k;

	textcolor[0].g = 255;
	textcolor[0].r = 0;
	textcolor[0].b = 0;
	textcolor[1].g = 0;
	textcolor[1].r = 255;
	textcolor[1].b = 0;

	destrect.x=0;
	destrect.y=0;
	if( (SDL_BlitSurface(bgimage, NULL, game->screen, &destrect)) < 0){
		printf("Cant blit surface: %s\n",SDL_GetError());
		return 1;
	}

	destrect.x = 210;
	destrect.y = 210+15;
	if( (SDL_BlitSurface(star, NULL, game->screen, &destrect)) < 0){
		printf("Cant blit surface: %s\n",SDL_GetError());
		return 1;
	}

	text = TTF_RenderText_Blended(font, "Connected players:", textcolor[0] );

	if (text == NULL) {
		printf("Cant render text (Connected players:): %s\n", SDL_GetError());
		return 1;
	}

	destrect.w = text->w;
	destrect.h = text->h;
	destrect.x = 230;
	destrect.y = 210;

	if ((SDL_BlitSurface(text, NULL, game->screen, &destrect)) < 0) {
		printf("Cant blit surface: %s\n", SDL_GetError());
		return 1;
	}
	for( k = 0; k <= 4; k++ ){
		text = TTF_RenderText_Blended(font, playernames[k], textcolor[1] );
		destrect.w = text->w;
		destrect.h = text->h;
		destrect.x = 230;
		destrect.y = 210 + ((k + 1) * destrect.h);
		if ((SDL_BlitSurface(text, NULL, game->screen, &destrect)) < 0) {
			printf("Cant blit surface: %s\n", SDL_GetError());
			return 1;
		}
	}
	SDL_FreeSurface( text );
	return 0;
}
/*
int UnpackString(char *stringdata, char playernames[][11], char *begingame,
		int length, int *playeriid, int *nroplayers){
	int k,l=0,rad=0;

	printf("length:  %d\n",length);
	printf("string:  %s\n", stringdata);

	if(stringdata[0] == '4' ){
		*playeriid = ( stringdata[3] - 48 );
		printf("player id: %d\n", *playeriid );
	}
	if( *playeriid == 5){
		printf("Server sent server full: closing client.\n");
		return 0;
	}


	else if(stringdata[0] == '3' ){
		switch (stringdata[3]) {
			case '1':
				*begingame = 1;
				*nroplayers = (stringdata[4] - 48 );
				break;
			case '2':
				printf("Server sent server full: closing client.\n");
				return 0;
				break;
			case '3':
				printf("Server sent quit: closing client.\n");
				return 0;
				break;
			}
	}
	else if( stringdata[0] == '2' ){
		for( k = 3; k <= length; k++ ){
			if( stringdata[k] == ':'){
				playernames[rad][l+1] = '\0';
				rad++;
				if( rad == 4 )
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
*/

int DisplayWinner(int winner, TTF_Font *font, SDL_Surface *bgimage, char playernames[5][11],
		t_gamedata *game, SDL_Surface *star){
	SDL_Rect destrect;
	SDL_Surface *text;
	SDL_Color textcolor[2];

	SDL_Quit();
	if (InitSdl(&game) > 0)
		return 1;

	textcolor[0].g = 255;
	textcolor[0].r = 0;
	textcolor[0].b = 0;
	textcolor[1].g = 0;
	textcolor[1].r = 255;
	textcolor[1].b = 0;

	destrect.x=0;
	destrect.y=0;
	if( (SDL_BlitSurface(bgimage, NULL, game->screen, &destrect)) < 0){
		printf("Cant blit surface: %s\n",SDL_GetError());
		return 1;
	}

	destrect.x = 210;
	destrect.y = 210+15;
	if( (SDL_BlitSurface(star, NULL, game->screen, &destrect)) < 0){
		printf("Cant blit surface: %s\n",SDL_GetError());
		return 1;
	}

	text = TTF_RenderText_Blended(font, "The winner is:", textcolor[0] );

	if (text == NULL) {
		printf("Cant render text (Connected players:): %s\n", SDL_GetError());
		return 1;
	}

	destrect.w = text->w;
	destrect.h = text->h;
	destrect.x = 230;
	destrect.y = 210;

	if ((SDL_BlitSurface(text, NULL, game->screen, &destrect)) < 0) {
		printf("Cant blit surface: %s\n", SDL_GetError());
		return 1;
	}

	text = TTF_RenderText_Blended(font, playernames[winner], textcolor[1] );
	destrect.w = text->w;
	destrect.h = text->h;
	destrect.x = 230;
	destrect.y = 210 + destrect.h;
	if ((SDL_BlitSurface(text, NULL, game->screen, &destrect)) < 0) {
		printf("Cant blit surface: %s\n", SDL_GetError());
		return 1;
	}
	SDL_UpdateRect(game->screen, 0, 0, 0, 0);
	SDL_Delay(5000);
	SDL_FreeSurface( text );
	return 0;
}
