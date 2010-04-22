/*
 * File: communication.h
 * Author: Fredrik Jansson
 * Description:
 * 	The servers interface to communicate with the client.
 */

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include "../header/collision.h"

#define STD_PORT 3030
#define BACKLOG 10
#define MSG_MAXLEN 100

/*The different types of tcp-messages*/
#define NAME_MSG   2
#define STATUS_MSG 3
#define PLAYER_ID  4


/* Initialize the basic server, and set it to listen.
 *
 * Params: Ip and port to use for the server, give 0 to use the defaults.
 *
 * Return: The initilized socket or -1 on error.
 *
 * Example:
 * 	To init with default values, do something like this: 
 * 		int sockFD;
 * 		sockFD = InitializeServer(0,0);
 * 	And DON'T forget to check for errors:
 * 		if((sockFD = InitializeServer(0,0)) == -1) {
 * 			printf("Ghe, it didn't work...");
 * 			return 1;
 * 		}
 */

int InitializeServer(char *ip, int port);

/* Initialize a new client socket.
 *
 * ToDo: Should also yield the address back to caller.. take a string for that?
 * 
 * Params: The socket previosly initialized with InitializeServer()
 *
 * Return: The new client socket, or -1 on error.
 * 
 * Example:
 * 	To init a new client socket:  (sockFD initilized with InitializeServer)
 * 		int clientFD
 * 		clientFD = AcceptClient(sockFD);
 * 	And DON'T forget to check for errors:
 * 		if((clientFD = AcceptClient(sockFD)) == -1){
 * 			printf("Ghe, it didn't work...");
 * 			return 1;
 * 		}
 */

int AcceptClient(int sockFD, char *ip);

/* Send a message to a tcp socket 
 * 
 * Params: sockFD is the socket to send over, the type specifies what
 * 			kind of message that is to be sent, and the msg is the message to be sent
 * 			(maximum of 99 characters)
 *
 * Returns: -1 on failiure, 0 when it's ok
 */

int SendTCPMessage(int clientFD, int type, char *msg);

/* Used to prepare the names array to be sent with SendTCPMessage. It will put the contents
 * of names into namesString, each name separated with ':'.
 *
 * Params: names should be an array containing a list of names while namesString is where the function
 * 		   will store the "stringified" result.
 */

void StringifyNames(char names[][11], char *namesString);

/* Recive data over a tcp socket
 *
 * Params: clienFD is the socket to retrive data from, type will after eval. 
 * 			contain what type the recieved messege are, and the msg is where
 * 			to store the msg.
 *
 * Returns: -1 on failure, 0 when it's ok
 */

int RecvTCPMessage(int clientFD, int *type, char *msg);

/* Wait for a client to send a message.
 *
 * Params: The accepting socket, an array of sockets that will be checked and the number of clients.
 *
 * Returns -1 on failure or the socket that has something to say.
 *
 * Example:
 * 	int sockets[MAX_NUM_SOCKETS];   		Where MAX should include the socket listening for new
 * 											clients.
 * 	
 * 	int hasMsg;								(Sofie, my imagination for vars isn't that great, use 
 * 											what ever you like...).
 * 	
 * 	hasMsg = WaitForIncoming(sockets, num); Where num simply is the total number of
 * 											sockets you currently have	
 *
 * 	------------------------------
 * 	hasMsg is now the talkative socket, and may be used the same way.
 */

int WaitForIncoming(int sockFD, int sockets[], int num);

/* Initialize the socket on wich we will recive input from the clients.
 *
 * Params: Ip and port to use for the server, give 0 to use the defaults.
 *
 * Return: The new client socket, or -1 on error.
 */

int InitializeUDPInput(char *ip, int port);

/* Initilize the output socket.
 *
 * Return: The new client socket, or -1 on error.
 */

int InitializeUDPOutput();

/* Wait for a client to send a direction.
 *
 * Params: The priviously initilized input socket, the total number of players, a list of
 * 		   the players ip and an address to a t_direction enum.
 *
 * Returns: Client ID, or -1 on error. Also sets 'direction' to the recieved value
 */

int RecvUDPInput(int sockFD, int numPlayers, char ip[][17], t_direction *direction);

/* Send the statusData to all clients
 *
 * Parameters: sockFd is the socket over witch to send, numPlayers is the number of active
 * 		       Players, ip should be a list containing the ip-addresses of the clients and 
 * 		       statusData is, well, the status data.
 *
 * Returns: -1 on failure
 */

int SendUDPOutput(int sockFD, int numPlayers,char ip[][17], t_gameData statusData);

#endif
