/*
 * main.c
 *
 *  Created on: Apr 15, 2009
 *      Author: alexander
 */

/* Local header files */
#include "../header/main.h"

int main(int argc, char *argv[]) {
	t_gamedata game;
	t_netdata net;
	int menuchoice;

	if (InitSdl(&game) > 0)
		return 1;
	if (InitSdlNet() < 0)
		return -1;
    menuchoice = menu(&game);
    if(menuchoice == 1)
    	Lobby(&game, &net);


	TTF_Quit();
	SDL_Quit();
	return 0;
}
