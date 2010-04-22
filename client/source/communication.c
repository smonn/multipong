/*
 * communication.c
 *
 *  Created on: Apr 20, 2009
 *      Author: alexander
 */
/* Local header files */
#include "../header/gfxinit.h"
#include "../header/menu.h"
#include "../header/main.h"
#include "../header/communication.h"
#include "../header/gameplay.h"

/* Unix header files */
#include <stdlib.h>

/* SDL and OpenGL header files */
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_net.h>

int InitSdlNet() {
	if( SDLNet_Init() < 0){
		printf("SDLNET_Init error: %s\n",SDL_GetError());
		return -1;
	}
	return 0;
}

int CreateTcpSocket( t_netdata *net, t_gamedata *game ) {
	net->socketset = SDLNet_AllocSocketSet(2);
		if( !net->socketset ){
			printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
			return -1;
		}
	if ( SDLNet_ResolveHost( &net->serverip, game->serverip,
			atoi(game->port)) < 0 )	{
		printf("SDLNet_ResolveHost: %s\n", SDL_GetError());
		return -1;
	}
	if ( ( net->tcpsocket = SDLNet_TCP_Open( &net->serverip) ) == NULL ) {
		printf("SDLNet_TCP_Open: %s\n", SDL_GetError());
		return -1;
	}
	else if( net->tcpsocket != NULL)
		printf("Connected to server: %x on port %d\n",
				SDLNet_Read32(&net->serverip.host),
				SDLNet_Read16(&net->serverip.port));
	if( SDLNet_TCP_AddSocket( net->socketset, net->tcpsocket ) == -1 )	{
			printf("SDLNet_TCP_AddSocket: %s\n",SDL_GetError());
			return -1;
	}
	return 0;
}

int CreateUdpSocket( t_netdata *net, t_gamedata *game){
	if ( ( net->udpsocket = SDLNet_UDP_Open(3032) ) == NULL ){
		printf("SDLNet_UDP_Open: %s\n", SDL_GetError());
		return -1;
	}
	return 0;
}

int CloseSocket( t_netdata *net) {
	if (net == NULL || (*net).tcpsocket == NULL)
		return -1;
	/* This is a troubled line..
	 * TCPsocket (the type) is a pointer by default:
	 * http://jcatki.no-ip.org:8080/SDL_net/SDL_net.html#SEC49
	 *
	 * And SDLNet_TCP_Close wants a pointer to it:
	 * http://jcatki.no-ip.org:8080/SDL_net/SDL_net.html#SEC19
	 *
	 * Currently, this line generates a memory dump (invalid pointer).
	 *
	 * Variations of this that have failed:
	 *		- net->tcpsocket
	 *		- &(net->tcpsocket)
	 *		- *net->tcpsocket
	 *		- *(net->tcpsocket)
	 *		- &((*net).tcpsocket)
	 *		- any more bright ideas?
	 */
/*	SDLNet_TCP_Close( &((*net).tcpsocket) );*/
	return 0;
}

int SendUDPMessage(t_netdata *net, int directionCode){

	UDPpacket *udpPacket;
	char direction[2];
	static	IPaddress ip;

	if( !ip.host )
	if ( SDLNet_ResolveHost( &ip, net->stringip, 3031 ) < 0 )	{
			printf("SDLNet_ResolveHost: %s\n", SDL_GetError());
			return -1;
		}
	sprintf(direction, "%d", directionCode );
	udpPacket = SDLNet_AllocPacket(strlen(direction) + 1);
	if(!udpPacket) {
		printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		return -1;
	}
	else {
		/* Assign values to the packet.
		 * Currently the server ip address is part of the packet,
		 * but this will be changed to a channel to minimize header size.
		 */
		udpPacket->address = ip;
		sprintf((char *)udpPacket->data,"%s",direction);
		udpPacket->len = strlen((char *)udpPacket->data) + 1;
		udpPacket->address.host = ip.host;	/* Set the destination host */
		udpPacket->address.port = ip.port;	/* And destination port */
		if( SDLNet_UDP_Send(net->udpsocket, -1, udpPacket) == 0 ) /* This sets the p->channel */
			printf("UDP Send error.\n");
	}
	SDLNet_FreePacket(udpPacket);

	return 0;
}

int ReceiveUDPMessage(t_netdata *net, t_gamestats *gamestats,
		t_playerpositions *positions) {
	UDPpacket *udpPacket;

	udpPacket = SDLNet_AllocPacket(42);
	if(!udpPacket) {
		printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		return -1;
	}
	memset(&udpPacket->data[0], 0, sizeof(udpPacket->data));
	/* Wait a packet. UDP_Recv returns != 0 if a packet is coming */
	if (SDLNet_UDP_Recv(net->udpsocket, udpPacket)){
		/*
		printf("UDP Packet incoming\n");
		printf("\tChan:    %d\n", udpPacket->channel);
		printf("\tData:    %s\n", (char *)udpPacket->data);
		printf("\tLen:     %d\n", udpPacket->len);
		printf("\tMaxlen:  %d\n", udpPacket->maxlen);
		printf("\tStatus:  %d\n", udpPacket->status);
		printf("\tAddress: %x %x\n", udpPacket->address.host, udpPacket->address.port);
		*/
	/*	printf("\tData:    %s\n", (char *)udpPacket->data); */
		UnpackUdpString( udpPacket, gamestats, positions);
	}

		/* Clean and exit */

	SDLNet_FreePacket(udpPacket);

	return 0;
}


int UnpackUdpString(UDPpacket *packet, t_gamestats *gamestats,
		t_playerpositions *positions){
/*	LivesPxPy:LivesPxPy:LivesPxPy:::BxBy */
/*	1    3  3   1   3 3 osv chars */
	int k, player=0;
/*	char buffer[50];

	memset( &buffer[0], 0, sizeof(buffer));

	sprintf( buffer, "%d" , (int)packet->data );
*/
	for( k = 0; k < 35; k += 7 ){
		/*if( player == 4 )
			break; */

		gamestats->lifes[player] = (packet->data[k] - 48);

		positions[player].x = ((packet->data[k+1] - 48) * 100) +
			((packet->data[k+2] - 48) * 10) + ((packet->data[k+3] - 48));

		positions[player].y = ((packet->data[k+4] - 48) * 100) +
			((packet->data[k+5] - 48) * 10) + ((packet->data[k+6] - 48));

		player++;
	}
	positions[5].x = ((packet->data[35] - 48) * 100) +
		((packet->data[36] - 48) * 10) + ((packet->data[37] - 48));

	positions[5].y = ((packet->data[38] - 48) * 100) +
		((packet->data[39] - 48) * 10 ) + ((packet->data[40] - 48));

	return 0;
}
