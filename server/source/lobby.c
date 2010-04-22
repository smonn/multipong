/*
 * File: lobby.c
 * Author: Sofie Willander
 * Description: This is where the new clients come after the 
 * 			connection is established and before they start to play.
 * Created: April 19th, 19:10
 * Updated: May 8th, 18.29 by Sofie Willander
 */
 
 #include "../header/lobby.h"
 #include "../header/communication.h"

 #include <string.h>
 #include <stdio.h>
 #include <pthread.h>
  
 /*
  * Global variables in the lobby. Used in almost every function.
  */ 
 int clientFD[6], connectedPlayers, notPlaying, new;
 char playerNames[5][11];
 
void *ListenNewConnections(void *parameters) { 
	
	int i, iveGotAMsg;
	struct listenParms* p = (struct listenParms*) parameters;

	for(i=0; i<6; i++) {
		i=connectedPlayers;
			printf("loop %d\n", i); fflush(stdout);

	if((iveGotAMsg=WaitForIncoming(p->sockFD, clientFD, connectedPlayers)) == p->sockFD) {
		printf("iveGotAMsg: %d\n", iveGotAMsg); fflush(stdout);

		if((clientFD[i] = AcceptClient(p->sockFD, p->clientip[i])) == -1) {
	 		printf("Ghe, it didn't work..."); fflush(stdout);
 		} 

 		else {
 			connectedPlayers++;
 			printf("Client connected successfully.\n");fflush(stdout);
 			p->clientFD[i]=clientFD[i];
 		}
 		if(i==5) {
 			i=4;
 		}
	}

	else{
		printf("Before Lobby. IveGotAMsg: %d\n", iveGotAMsg);fflush(stdout);
		Lobby(iveGotAMsg);
		i--;

	}
	}

	return NULL;

}
 
 
void Lobby(int iveGotAMsg) {
	
	int isType, id, i, nPlayers;
	char msg[11]="\0", idc[3];
 	
	printf("In Lobby. connectedPlayers: %d\n", connectedPlayers);
		
	if(RecvTCPMessage(iveGotAMsg, &isType, msg)==-1) {
		printf("MsgReceive failure.\n");			
	}
	else {
		printf("Msg received successfully. %s\n", msg);
		printf("isType: %d\n", isType);
	}
	
	
	switch(isType) {
		case 0: /* Client disconnected */ 
			CloseConnection(iveGotAMsg);
			break;
		case 303101: /* Start signal received */ 
			printf("Start signal received\n");
			if(connectedPlayers>1 && iveGotAMsg==clientFD[0]) {
				nPlayers=StartSignal();
			}
			else {
				printf("To few players to start.\n");
			}
			break;
		case 210: /*New clients playername received */ 
			id=RegisterPlayer(msg);
			if (id== 5) {
				printf("List full. Close socket. \n");
				for(i=0; i<5; i++) {
					if(clientFD[i]==iveGotAMsg) {
						close(clientFD[i]);
					}
				}
				
			}			 
			else {
				printf("Client is in playerlist. He got id %d.\n", id);
			}
			
			sprintf(idc, "%d", id);
			if (SendTCPMessage(iveGotAMsg, PLAYER_ID, idc)==-1) {
				printf("An error has occured. Couldn't send id to client.\n");
			}
			else {
				printf("id sent to client.\n");
			}
			sleep(1);
		 	if(notPlaying) {
		 		if(id>=0 && id<5){
	 				SendPlayerNames();
	 			}
			}
			
			if(iveGotAMsg!=clientFD[id]) {
				for(i=0; i<6; i++) {
					if(clientFD[i]==iveGotAMsg) {
						close(clientFD[i]);
					}
				}
				clientFD[id]=iveGotAMsg;
			}

			break;
		default: /* Any other type of message */ 
			printf("Another type of message received.\n"); 
			break;
	}	

} 


void CloseConnection(int iveGotAMsg) {

	int i, removedid=-1;
	
	connectedPlayers--;
	
	for(i=0; i<5; i++) {
		printf("Player %d: %s\n", i, playerNames[i]);
		if(clientFD[i]==iveGotAMsg) {
			strcpy(playerNames[i],"\0");
			removedid=i;
			printf("removedid: %d\n", removedid);
			close(clientFD[i]);
		}
	}

	if(removedid==-1) {
		return;
	}

	for(i=removedid; i<4; i++) {
		strcpy(playerNames[i],playerNames[i+1]);
		clientFD[i]=clientFD[i+1];
	}
	
	strcpy(playerNames[4],"\0");
	
	SendIdsToPlayers();
	sleep(1);
	SendPlayerNames();
	
	return;

}

int RegisterPlayer(char newPlayer[]) {

	int reg=0, id=5, i;
	
	/*Placing newPlayer in list. If no room. id will not change from 5.*/
	if(notPlaying) {	
		for (i=0; i<5; i++) {
			if(strcmp(playerNames[i],"\0")==0 && reg==0) {
				strcpy(playerNames[i],newPlayer);
				id=i;
				reg=1;
			}
		}
	}
		
	return id;
	
}

void SendIdsToPlayers() {
	
	int i;
	char idc[3];
	
	for(i=0; i<5; i++) {
		if(strcmp(playerNames[i],"\0")!=0) {
			sprintf(idc, "%d", i);
			if (SendTCPMessage(clientFD[i], PLAYER_ID, idc)==-1) {
				printf("Couldn't send id to client %d.\n", i);
			}
			else {
				printf("id sent to client.\n");
			}
		}
	}

}

void SendPlayerNames() {
	
	int i;
	char namesString[60]={0};
	
	StringifyNames(playerNames, namesString);
		
	for(i=0; i<5; i++) {
		if(strcmp(playerNames[i], "\0")!=0) {
			if (SendTCPMessage(clientFD[i], NAME_MSG, namesString)==-1) {
				printf("Couldn't send playerNames to client.\n");
			}
			else {
				printf("playerNames sent to client. \n");
			}
		}
	}
}

void ResetPlayers() {
	
	int i;

    for(i=0; i<5; i++) {
        strcpy(playerNames[i],"\0");
        close(clientFD[i]);
    }

}

int StartSignal() { 
	
	int i, n=0; /*n is the number of players registered in the list*/
	char msg[5];
	
	notPlaying=0;
	for(i=0; i<5; i++) {
		if(strcmp(playerNames[i],"\0")!=0) {
			n++;
		}
	}
	sprintf(msg, "%d", (10+n));
	printf("StartSignal msg: %s, n: %d, connectedPlayers: %d\n", msg, n, connectedPlayers);
	for(i=0; i<5; i++) {
		if(strcmp(playerNames[i],"\0")!=0) {
			if (SendTCPMessage(clientFD[i], STATUS_MSG, msg)==-1) {
				printf("An error has occured. Couldn't tell clients to start.\n");
			}
			else {
				printf("Told client %d to start\n", i);
			}
		}
	}
	return n;
}

void PrepareNewGame() {

	connectedPlayers=0;
	ResetPlayers();
	notPlaying=1;

}

int StartReceived() {
	
	int i, n=0;
	
	while(1) {
		if(notPlaying==0) {
			for(i=0; i<5; i++) {
				if(strcmp(playerNames[i],"\0")!=0) {
					n++;
				}
			}
			return n;
		}
		else {
			sleep(1);
		}
	}

}
