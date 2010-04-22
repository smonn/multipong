/*
 * menu.c
 *
 *  Created on: Apr 15, 2009
 *      Author: alexander
 *
 */

/* Local header files */
#include "../header/menu.h"

int menu(t_gamedata *game) {
	int choice = 3;
	int running = 1;
	int returnchoice = 0;
	int charcount[3] = {0};
	int update = 1;

	SDL_Event keyboardevent;
	Uint16 menucolors[5];
	SDL_Surface *text = NULL, *bgimage = NULL;
	TTF_Font *font = NULL;
	SDL_Color textcolor[3];
	char name[NAMELENGTH+1],serverip[IPLENGTH+1],port[PORTLENGTH+1];

	font = TTF_OpenFont("./assets/font.ttf", 20);

	if (font == NULL) {
	    printf("Unable to load font: %s \n", SDL_GetError());
	    return -1;
	}

	bgimage = IMG_Load("./assets/bg.png");

	if( !bgimage ) {
		printf("IMG_Load: %s\n", SDL_GetError() );
		return -1;
	}

	memset(&name[0],	 0, sizeof(name));
	memset(&serverip[0], 0, sizeof(serverip));
	memset(&port[0], 	 0, sizeof(port));
	/*
	name[0]=' ';
	strcpy( serverip,   "localhost");
	strcpy( port, 		"12345" );
	 */

	ReadGameFile( game );

	strcpy( name, 		game->name);
	strcpy( serverip,   game->serverip);
	strcpy( port, 		game->port );


	SetColors(menucolors, game, textcolor);

	while (running == 1) {
		while (SDL_PollEvent(&keyboardevent)) {
				if (keyboardevent.type == SDL_QUIT
						|| (keyboardevent.type == SDL_KEYDOWN
						&& keyboardevent.key.keysym.sym == SDLK_ESCAPE)) {
					returnchoice = 0;
					running = 0;
				} else if (keyboardevent.type == SDL_KEYDOWN
						&& keyboardevent.key.keysym.sym == SDLK_DOWN
						&& choice > 1){
					choice--;
					update = 1;
				}
				else if (keyboardevent.type == SDL_KEYDOWN
						&& keyboardevent.key.keysym.sym == SDLK_UP
						&& choice < 3){
					choice++;
					update = 1;
				}
				else if( keyboardevent.type == SDL_KEYDOWN
						&& keyboardevent.key.keysym.sym == SDLK_BACKSPACE
						&& charcount[choice-1] > 0 ){
					if( choice == 3 )
						charcount[choice-1]=Backspace( name,
						charcount[choice-1], NAMELENGTH );
					else if( choice == 2)
						charcount[choice-1]=Backspace( serverip,
						charcount[choice-1], IPLENGTH );
					else
						charcount[choice-1]=Backspace( port,
						charcount[choice-1], PORTLENGTH );
					update = 1;
				}
				else if(keyboardevent.type == SDL_KEYDOWN
					&& keyboardevent.key.keysym.sym == SDLK_DELETE
					&& charcount[choice-1] >= 0 ){
					if( choice == 3 )
						delete( name, charcount[choice-1], NAMELENGTH );
					else if( choice == 2)
						delete( serverip, charcount[choice-1], IPLENGTH );
					else
						delete( port, charcount[choice-1], PORTLENGTH );
					update = 1;
				}
				else if (keyboardevent.type == SDL_KEYDOWN
						&& keyboardevent.key.keysym.sym == SDLK_RETURN) {
					strcpy(game->name,		name);
					strcpy(game->serverip,	serverip);
					strcpy(game->port,		port);
					WriteGameFile( game );
					returnchoice = 1;
					running = 0;
				}
				else if (UNICODE == (Uint16)'.' || ( (Uint16)'a' <= UNICODE
						&& UNICODE <= (Uint16)'z') || ((Uint16)'0' <= UNICODE
						&& UNICODE <= (Uint16)'9') || ( (Uint16)'A' <= UNICODE
						&& UNICODE <= (Uint16)'Z')) {
					switch (choice) {
					case 3:
						if (charcount[2] < 10) {
							name[charcount[2]] = (char)UNICODE;
							charcount[2]++;
							update = 1;
						}
						break;
					case 2:
						if (charcount[1] < 15){
							serverip[charcount[1]] = (char)UNICODE;
							charcount[1]++;
							update = 1;
						}
						break;
					case 1:
						if (charcount[0] < 5) {
							port[charcount[0]] = (char)UNICODE;
							charcount[0]++;
							update = 1;
						}
					}
				} else
					break;
		}
		if( update == 1){
			if (DrawMenu(menucolors, game, font, text, bgimage, textcolor,
					choice, name, serverip, port) == 1)
				return -1;
			update = 0;
		}

		SDL_UpdateRect(game->screen, 0, 0, 0, 0);
		SDL_Delay(10);
	}
	SDL_FreeSurface( bgimage );
	return returnchoice;
}

int DrawMenu(Uint16 *menucolors, t_gamedata *game, TTF_Font *font,
		SDL_Surface *text, SDL_Surface *bgimage, SDL_Color *textcolor,
		int choice, char* name,	char* serverip, char* port) {

	SDL_Rect destrect;
	int active;

	destrect.x = 0;
	destrect.y = 0;
	if( (SDL_BlitSurface(bgimage, NULL, game->screen, &destrect)) < 0){
		printf("Cant blit surface: %s\n",SDL_GetError());
		return 1;
	}


	if (choice == 3)
		active = 2;
	else
		active = 0;

	text = TTF_RenderText_Blended(font, "Name:", textcolor[active]);

	if (text == NULL) {
		printf("Cant render text (Name:): %s\n", SDL_GetError());
		return 1;
	}

	destrect.w = text->w;
	destrect.h = text->h;
	destrect.x = 250;
	destrect.y = SCREEN_HEIGHT / 2-destrect.h / 2;

	if ((SDL_BlitSurface(text, NULL, game->screen, &destrect)) < 0) {
		printf("Cant blit surface: %s\n", SDL_GetError());
		return 1;
	}

	text = TTF_RenderText_Blended(font, name, textcolor[active]);

	if (text == NULL) {
		printf("Cant render text (name): %s\n", SDL_GetError());
		return 1;
	}

	destrect.w = text->w;
	destrect.h = text->h;
	destrect.x = 400;
	destrect.y = SCREEN_HEIGHT / 2-destrect.h / 2;

	if ((SDL_BlitSurface(text, NULL, game->screen, &destrect)) < 0) {
		printf("Cant blit surface: %s\n", SDL_GetError());
		return 1;
	}

	if (choice == 2)
		active = 2;
	else
		active = 0;

	text = TTF_RenderText_Blended(font, "Server ip:", textcolor[active]);

	if (text == NULL) {
		printf("Cant render text(Server ip:): %s\n", SDL_GetError());
		return 1;
	}

	destrect.w = text->w;
	destrect.h = text->h;
	destrect.x = 250;
	destrect.y = SCREEN_HEIGHT / 2-destrect.h / 2 + text->h;

	if ((SDL_BlitSurface(text, NULL, game->screen, &destrect)) < 0) {
		printf("Cant blit surface: %s\n", SDL_GetError());
		return 1;
	}

	text = TTF_RenderText_Blended(font, serverip, textcolor[active]);

	if (text == NULL) {
		printf("Cant render text (serverip): %s\n", SDL_GetError());
		return 1;
	}

	destrect.w = text->w;
	destrect.h = text->h;
	destrect.x = 400;
	destrect.y = SCREEN_HEIGHT / 2-destrect.h / 2 + text->h;

	if ((SDL_BlitSurface(text, NULL, game->screen, &destrect)) < 0) {
		printf("Cant blit surface: %s\n", SDL_GetError());
		return 1;
	}


	if (choice == 1)
		active = 2;
	else
		active = 0;

	text = TTF_RenderText_Blended(font, "Server port:", textcolor[active]);

	if (text == NULL) {
		printf("Cant render text(Server ip:): %s\n", SDL_GetError());
		return 1;
	}

	destrect.w = text->w;
	destrect.h = text->h;
	destrect.x = 250;
	destrect.y = SCREEN_HEIGHT / 2-destrect.h / 2 + 2 * text->h;

	if ((SDL_BlitSurface(text, NULL, game->screen, &destrect)) < 0){
		printf("Cant blit surface: %s\n", SDL_GetError());
		return 1;
	}

	text = TTF_RenderText_Blended(font, port, textcolor[active]);

	if (text == NULL) {
		printf("Cant render text (serverip): %s\n", SDL_GetError());
		return 1;
	}

	destrect.w = text->w;
	destrect.h = text->h;
	destrect.x = 400;
	destrect.y = SCREEN_HEIGHT / 2-destrect.h / 2 + 2 * text->h;

	if ((SDL_BlitSurface(text, NULL, game->screen, &destrect)) < 0) {
		printf("Cant blit surface: %s\n", SDL_GetError());
		return 1;
	}
	SDL_FreeSurface( text );
	return 0;
}
void SetColors(Uint16 *menucolors, t_gamedata *game, SDL_Color *textcolor) {
	menucolors[0] = SDL_MapRGB(game->screen->format, 0x00, 0x00, 0x00);
	menucolors[1] = SDL_MapRGB(game->screen->format, 0xFF, 0xFF, 0xFF);
	menucolors[2] = SDL_MapRGB(game->screen->format, 0xFF, 0x00, 0x00);
	menucolors[3] = SDL_MapRGB(game->screen->format, 0x00, 0xFF, 0x00);
	menucolors[4] = SDL_MapRGB(game->screen->format, 0x00, 0x00, 0xFF);

	textcolor[0].r = 255;
	textcolor[0].g = 0;
	textcolor[0].b = 0;
	textcolor[1].r = 255;
	textcolor[1].g = 255;
	textcolor[1].b = 255;
	textcolor[2].r = 0;
	textcolor[2].g = 255;
	textcolor[2].b = 0;
}

int Cleanup(SDL_Surface *text, TTF_Font *font) {
	/* SDL_FreeSurface(text);
	 * TTF_CloseFont(font); */
	return 0;
}

int Backspace(char *text, int charcount, int length){
	int k;
	for( k = charcount; k < length; k++){
		text[k] = text[k+1];
	}
	charcount--;
	return charcount;
}

int delete(char *text, int charcount, int length){
	int k;
	for( k = charcount; k < length - charcount; k++){
		text[k] = text[k+1];
	}
	if( charcount == 0 && text[0] == '\0' )
		text[0] = ' ';
	return 0;
}

int WriteGameFile( t_gamedata *game ){
	FILE *gamefile;

	gamefile = fopen( "gamefile", "wt" );

	fprintf( gamefile, "%s\n",(char *)&game->name);
	fprintf( gamefile, "%s\n",(char *)&game->serverip);
	fprintf( gamefile, "%s\n", (char *)&game->port);

	fclose( gamefile );

	return 0;
}
int ReadGameFile( t_gamedata *game ){
	FILE *gamefile;

	gamefile = fopen( "gamefile", "rt" );
	if(!gamefile){
		printf("Error opening gamefile.\n");
		return -1;
	}
	if(fscanf( gamefile, "%s\n", game->name) <= 0){
		printf("Error reading gamefile\n");
		return -1;
	}
	if(fscanf( gamefile, "%s\n", game->serverip) <= 0){
		printf("Error reading gamefile\n");
		return -1;
	}
	if(fscanf( gamefile, "%s", game->port) <= 0){
		printf("Error reading gamefile\n");
		return -1;
	}


	fclose( gamefile );
	return 0;
}
