/*
 * File: lobby.h
 * Author: Sofie Willander
 * Description: Structures and funtions for handling
 * 		playernames and ids.
 * Created: April 19th, 19:05
 * Updated: May 8th, 18.29 by Sofie Willander
 */

#ifndef LOBBY_H_
#define LOBBY_H_

#include "gameplay.h"

typedef struct listenParms {
	int sockFD;
	int clientFD[6];
	char clientip[6][17];
} t_listenParms;


/* This function will be changed a lot today... 
 * 
 * This is the first function called after the connection is established.
 * Input: 	the socket that has got something to say
 * Returns: Nothing
 * Does: This is where the clients gets in line, get their ids etc. This 
 * 		function will call other functions that register players, listening for 
 * 		startsignal and more.
 */

void Lobby(int iveGotAMsg);

/* Placing the player of a new connection in the line.
 * Return id 0 for head-player, 1-4 for other player or 5 if it's full. 
 * These values are of thype char.
 * 
 * Input: The new playerName. 
 * Return: playerId.
 */
 
int RegisterPlayer(char newPlayer[]);

/* Closes the socket when a client disconnected.
 * Input: what socket to close (iveGotAMsg) 
 * 
 * Removes the playNames from list, and pushes all playersNames and sockets up
 * one step. 
 */

void CloseConnection(int iveGotAMsg);

/*
 * Send all player ids of player 0-4 to clients via a socket. 
 * player 0 is head-player.
 */

void SendIdsToPlayers();

/*
 * Sends the array playerNames to all connected clients.
 */
 
void SendPlayerNames();

/* 
 * When the is game over, the list will be emptied, so that new players can come
 * Return: All playernames set to "\0" and sockets closed.
 */
 
void ResetPlayers();

/* When the startsignal is received, this function counts the number of players
 * that are connected, and sends a message to all cliets that the game starts
 * and how many players there are.
 * Type STATUS_MSG, msg: 1n, (3 players= 13)
 */

int StartSignal();
/* 
 * Calls the resetplayers function and sets notPlaying=1
 */
void PrepareNewGame();

/* The funtion to which the thread comes. Listenes for new messages and 
 * connections with the funtion WaitForIncoming from communication.h.
 * When a new message arrives, it will forward it to Lobby, which takes care
 * of it.
 */
void *ListenNewConnections(void *parameters);

/* This function is onle used by main. It's a while-loop that will stop spinning
 * when the startsignal is received (notPlaying=0)
 */

int StartReceived();

#endif /* LOBBY_H_ */
