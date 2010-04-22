/*
 * File: communication.c
 * Author: Fredrik Jansson
 * Description:
 * 	The servers interface to communicate with the client.
 */

#include "../header/communication.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*private functions*/
void ExtractMessage(char *buf, int *type, char *dest);
void PadPosValue(char *position);

int InitializeServer(char *ip, int port){
	struct sockaddr_in serverAddress;
	int sockFD, yes=1;

	if((sockFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return -1;
	}

	if((setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) == -1) {
		perror("setsockopt");	
		return -1;
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = (port != 0) ? htons(port) : htons(STD_PORT);
	serverAddress.sin_addr.s_addr = (ip != 0) ? inet_addr(ip) : INADDR_ANY;
	memset(&(serverAddress.sin_zero), '\0', 8);

	if(bind(sockFD, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		return -1;
	}

	if(listen(sockFD, BACKLOG) == -1) {
		perror("listen");
		return -1;
	}

	return sockFD;
}

int AcceptClient(int sockFD, char *ip) {
	int clientFD;
	socklen_t sin_size;
	struct sockaddr_in clientAddress;

	sin_size = sizeof(struct sockaddr_in);

	if((clientFD = accept(sockFD, (struct sockaddr *) &clientAddress, 
					&sin_size)) == -1 ){
		perror("Accept");
		return -1;
	}

	strcpy(ip, inet_ntoa(clientAddress.sin_addr));

	return clientFD;
}

int SendTCPMessage(int clientFD, int type, char *msg) {
	int len;
	char *sendData, charType, lengthOfMsg[3];
	
	charType='0'+ type;

	sprintf(lengthOfMsg, "%d", (int) strlen(msg));
	if(strlen(msg) < 10){	/*pad lengths below 10 to 01 02 03 etc*/
		lengthOfMsg[1] = lengthOfMsg[0];
		lengthOfMsg[0] = '0';
	}

	printf("%c\n", charType);

	sendData = malloc((1 + strlen(lengthOfMsg) + strlen(msg))*sizeof(char));
	sendData[0] = '\0';
	strncat(sendData, &charType, 1);
	strncat(sendData, lengthOfMsg, 2);	
	strncat(sendData, msg, strlen(msg));

	printf("%s\n", sendData);
	fflush(stdout);

	if((len=send(clientFD, sendData, strlen(sendData), 0)) == -1) {
		perror("send");
		return -1;
	}

	if(len!=strlen(sendData)){
		printf("Failed to send the whole packet\n");
		fflush(stdout);
	}

	free(sendData);

	return 0;
}

void StringifyNames(char names[][11], char *namesString){
	int i;
		printf("%s\n", namesString);
		fflush(stdout);

	for(i=0; i<5; i++) {
		strncat(namesString, names[i], strlen(names[i]));
		if(i < 4) {		/*Separate all names with ':'*/
			strncat(namesString, ":", 1);
		}
	}
}

void ExtractMessage(char *buf, int *type, char *dest){
	int lengthOfMsg;
	char lengthExtract[2];

	lengthExtract[0] = buf[1];
	lengthExtract[1] = buf[2];

	*type=atoi(&buf[0]);
	lengthOfMsg = atoi(lengthExtract);

	/*Debug msg, can safely be removed when all works*/
	printf("Length of msg to extract,char:%c%c digit: %d\n", lengthExtract[0],lengthExtract[1], lengthOfMsg);
	fflush(stdout);

	/*shift the buffer, so it begins at the start of the msg*/
	buf += 3;

	printf("Msg: %s", buf);
	fflush(stdout);

	strncat(dest, buf, (size_t) lengthOfMsg);
}

int RecvTCPMessage(int clientFD, int *type, char *msg) {
	char buf[MSG_MAXLEN]={0};

	int numbytes;
	
	msg[0]='\0';

	if((numbytes = recv(clientFD, buf, MSG_MAXLEN-1, 0)) == -1) {
		perror("recv");
		return -1;
	}
	
	buf[numbytes] = '\0';

	printf("Recived msg: %s\n",buf);

	ExtractMessage(buf, type, msg);

	return 0;
}

int WaitForIncoming(int sockFD, int sockets[], int num){
	int i, fdMax;
	struct timeval tv;
	fd_set master, readFdS;

	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	FD_ZERO(&master);
	FD_ZERO(&readFdS);

	FD_SET(sockFD, &master);
	fdMax = sockFD;

	for(i=0; i < num; i++){
		FD_SET(sockets[i], &master);
		if(sockets[i] > fdMax) {
			fdMax = sockets[i];
		}
	}

	for(;;) {
		readFdS = master;
		if(select(fdMax+1, &readFdS, NULL, NULL, &tv) == -1){
			perror("select");
			return -1;
		}

		for(i = 0;i <= fdMax; i++) {
			if(FD_ISSET(i, &readFdS)) {
				return i; /*i is the socket with something to say...*/
			}
		}
	}
}

int InitializeUDPInput(char *ip, int port){
	struct sockaddr_in serverAddress;
	int sockFD, yes=1;

	if((sockFD = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		return -1;
	}

	if((setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) == -1) {
		perror("setsockopt");	
		return -1;
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = (port != 0) ? htons(port) : htons(STD_PORT+1);
	serverAddress.sin_addr.s_addr = (ip != 0) ? inet_addr(ip) : INADDR_ANY;
	memset(&(serverAddress.sin_zero), '\0', 8);

	if(bind(sockFD, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		return -1;
	}

	return sockFD;
}

int InitializeUDPOutput(){
	return socket(AF_INET, SOCK_DGRAM, 0);
}

int RecvUDPInput(int sockFD, int numPlayers, char ip[][17], t_direction *direction){
	struct sockaddr_in clientAddress;
	int i, numbytes;
	unsigned int addrLen;
	char buf[MSG_MAXLEN];

	addrLen = sizeof(struct sockaddr);
	if((numbytes = recvfrom(sockFD, buf, MSG_MAXLEN-1, 0,
				   	(struct sockaddr *)&clientAddress, &addrLen)) == -1){
		perror("RecvUDPInput");
		return -1;
	}

	for(i=0;i<numPlayers;i++) {
		if(inet_addr(ip[i]) == clientAddress.sin_addr.s_addr) {
			*direction = atoi(buf);
			return i;
		}
	}

	return -1;
}

/* Pads '0' infront of the pos string, so it's always 3 chars long */

void PadPosValue(char *pos) {
	if(atoi(pos)<100) {
		pos[3]='\0';
		if(atoi(pos)<10) {
			pos[2]=pos[0];
			pos[1]='0';
			pos[0]='0';
		}
		else {
			pos[2]=pos[1];
			pos[1]=pos[0];
			pos[0]='0';
		}
	}
}
	

int SendUDPOutput(int sockFD, int numPlayers, char ip[][17], t_gameData statusData){
	int i, numbytes;
	struct sockaddr_in targetAddr;
	char lives[2],pX[4],pY[4], statusString[MSG_MAXLEN]={0};

	/*Put all the status values of the players in a string*/
	for(i=0; i<numPlayers; i++) {
		sprintf(lives,"%d", statusData.players[i].lives);
		sprintf(pX,"%d", (int) statusData.players[i].paddle.position.x);
		sprintf(pY,"%d", (int) statusData.players[i].paddle.position.y);

		PadPosValue(pX);
		PadPosValue(pY);

		strncat(statusString, lives, strlen(lives));
		strncat(statusString, pX, strlen(pX));
		strncat(statusString, pY, strlen(pY));
	}

	/*Add zeros to fill upp nonexisting players in the string*/
	for(i=0; i<5-numPlayers;i++){
		strncat(statusString, "00000000", 7);
	}

	/*Add the balls pos data to the end of the status string*/
	sprintf(pX,"%d", (int) statusData.ball.position.x);
	sprintf(pY,"%d", (int) statusData.ball.position.y);

	PadPosValue(pX);
	PadPosValue(pY);

	strncat(statusString, pX, strlen(pX));
	strncat(statusString, pY, strlen(pY));

	/*Send the status string to all the clients*/
	for(i=0; i<numPlayers; i++){
		targetAddr.sin_family = AF_INET;
		targetAddr.sin_port = htons(STD_PORT+2);
		targetAddr.sin_addr.s_addr = inet_addr(ip[i]);
		memset(&(targetAddr.sin_zero), '\0', 8);

		if((numbytes=sendto(sockFD, statusString, strlen(statusString), 0, 
						(struct sockaddr *)&targetAddr,sizeof(struct sockaddr))) == -1) {
			perror("sendto");
			return -1;
		}
	}

	return 0;
}
