/*
 *		  File: gameplay.h
 *		Author: simon, dan
 * Description: This file brings the gameplay logic together and
 *				gives a simple function call to start a game.
 *	   Created: April 24th 2009, 15:35
 */
#ifndef GAMEPLAY_H_
#define GAMEPLAY_H_

#include "communication.h"
#include "jobs.h"
#include <sys/time.h>


/*
 * Struct type that should be used when creating the INPUT thread.
 */
typedef struct inputopts {
	t_jobthread job;
	pthread_t threadID;
	int nplayers;
	int port;
	char serverIP[17];
	char clientIPs[5][17];
} t_inputopts;

/*
 * Starts the game on the server and begins sending and receiving
 * game data to and from the connected clients. Won't return until
 * either an error occurs or the game ends.
 *
 * - NPLAYERS is the number of players that's connected.
 * - SERVERIP is the server's IP. Could be 0 for default values.
 * - CLIENTIPS is a NPLAYERS long array, containing IP's for every player.
 * - PORT is the port that should be used when sending/receiving data.
 *   Could be 0 for default.
 *
 * Returns -1 on failure and player ID on success (i.e. the winner ID).
 * Could return something else if needed...
 */
int StartGame(int nplayers, char serverIP[17],
		char clientIPs[][17], int port);

#endif /* GAMEPLAY_H_ */
