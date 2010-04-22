/*
 * communication.h
 *
 *  Created on: Apr 20, 2009
 *      Author: alexander
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#define TIMEOUT (5000)
#define NERROR (0xff)

/* Local header files */
#include "gfxinit.h"

/* Unix header files */
#include <stdlib.h>

/* SDL and OpenGL header files */
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_net.h>

#define UDPPSIZE  512
#define MSG_MAXLEN 100

/*The different types of tcp-messages*/
#define NAME_MSG   2
#define STATUS_MSG 3

typedef struct netdata {
	IPaddress serverip;
	TCPsocket tcpsocket;
	UDPsocket udpsocket;
	SDLNet_SocketSet socketset;
	char stringip[16];
} t_netdata;

typedef struct positions{
	int x;
	int y;
}t_playerpositions;

typedef struct gamestatus{
	int winner;
	int lifes[5];
}t_gamestats;

int InitSdlNet();

int CreateTcpSocket( t_netdata *net, t_gamedata *game  );

int CreateUdpSocket( t_netdata *net, t_gamedata *game);

int SendUDPMessage(t_netdata *net, int directionCode);
int ReceiveUDPMessage(t_netdata *net, t_gamestats *gamestats,
		t_playerpositions *positions);

int CloseSocket(t_netdata *net);

int UnpackUdpString(UDPpacket *packet, t_gamestats *gamestats,
		t_playerpositions *positions);

#endif /* COMMUNICATION_H_ */
