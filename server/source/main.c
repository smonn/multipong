/*
 * File: lobby.c
 * Author: Sofie Willander
 * Description: 
 * Created: April 19th, 19:15
 * Updated: May 8th, 15.33 by Sofie Willander
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../header/lobby.h"
#include "../header/communication.h"

int main(int argc, char *argv[]){

    int i, port=0, winner, nPlayers;
	char serverip[17]={"0"}, msg[6];
	pthread_t newConnections;
	t_listenParms connectionParms;
	
	/*Possible options*/
	for(i=0; i<argc; i++) {
		if(strcmp(argv[i], "-ip") == 0) {
			if(argc<i+2){
				printf("When the ip flag is used, you have to specify a ip\n");
				return -1;
			}
			strcpy(serverip, argv[i+1]);
		}
		if(strcmp(argv[i], "-p") == 0) {
			if(argc<i+2){
				printf("When the port flag is used, you have to specify a port\n");
				return -1;
			}
			port = atoi(argv[i+1]);
		}
	}
/* 
 * Initialize Server
 */
	if((connectionParms.sockFD = InitializeServer(serverip,port)) == -1) {
 		printf("Ghe, it didn't work...");
 		return 1;
 	}
 	else {
 		printf("Server Initialized.\n");
 	} 	

/*
 * PrepareNewGame() empties the playerlist, sets the notPlayingflag to 1 and
 * sets connectedPlayers=1.
 */
 	PrepareNewGame();
	
/*
 * Creates the thread that will be listening for new connections, and sends them
 * to the lobby.
 */
	if(pthread_create(&newConnections, NULL, &ListenNewConnections,  &connectionParms)!=0) {
 			printf("pthread create failure. \n");
 		}
 		else {
 			printf("pthread ok (from main).\n");
 		}
 		
while(1) { 		
 	/* StartReceived will return when the startsignal from player 0 is received.
 	 */
 	nPlayers=StartReceived();
	printf("Back in main. nPlayers %d\n", nPlayers);
	
	
	
	for(i=0; i<5; i++)
	   printf("clientip: %s\n",connectionParms.clientip[i]);
	
	/* Calls the logic of the game. Won't return until the game is finished. */		
 	if((winner=StartGame(nPlayers, serverip, connectionParms.clientip, port))==-1) {
 		printf("Didn't start game.\n");
 	}
 	else {
 	printf("Game started\n");
 	}
 	
 	sprintf(msg, "%d", (50+nPlayers));
 	
 	/* Sends the id of the winner to all connected clients. 
 	 
 	for(i=0; i<nPlayers; i++) {
		if (SendTCPMessage(connectionParms.clientFD[i], STATUS_MSG, msg)==-1) {
			printf("Couldn't send playerNames to client.\n");
		}
 	}
 	*/
 	/* rinses playernames and closes sockets, before starting all over again.
 	 */
 	PrepareNewGame();
	
}
    
/*
 * When the game(s) are finished we wish to join the thread so we haven't got a
 * lot of threads working for no use.
 */    
    if (pthread_join(newConnections, NULL) != 0) {
		printf("pthread join failure\n");
	}
    
    return 0;
    
    }
