/*
 *		  File: collision.h
 *		Author: dan
 * Description: ...
 *	   Created: April 22nd 2009, 16:59
 *	   Updated: April 22nd 2009, 21:38 by simon
 */

#ifndef COLLISION_H_
#define COLLISION_H_

#include "vector.h"

/*
 * Structure containing information about a ball, needed by all collision tests
 * speed may be needed to prevent a slow corruption of the magnitude of velocity
 */
typedef struct {
	t_vector position, velocity;
	float radius, speed;
} t_ball;

/*
 * Structure containing Paddle-data
 * (will maybe convert pos to a float (float v * direction = pos))
 */
typedef struct {
	t_vector position;
	t_line paddleLine;
	float paddleWidth;
} t_paddle;

/*
 * A structure type connecting a player with a paddle.
 */
typedef struct {
	t_paddle paddle;
	int lives, id;
} t_player;

/*
 * A structure type collecting player and game data. Used when
 * sending data to players.
 *
 * TODO: add number of players, and more..
 */
typedef struct {
	t_player players[5];
	t_ball ball;
} t_gameData;

/*
 * Enum type for player movement directions. Used when
 * receiving data from players.
 */
typedef enum {
	NONE, LEFT, RIGHT
} t_direction;

/*
 * Will take: ball pointer and player-data for all players
 * and all corner-points
 * will test all the players/corners for collisions individually
 *
 * also updates the ball position
 *
 * will return: 1:  if collision
 * 				0:  else
 * (& maybe    -1: error)
 */
int TestCollision(t_ball *ball, t_gameData *game, int nplayers, t_vector corners[], t_line walls[], int nWalls);

/*
 * Tests if a ball is colliding with a player(including edges)
 * and automatically bounces the ball
 * takes: ball pointer and player
 * returns: ball vel magnitude factor that has been updated
 * 		  0-1:if collision
 * 			0: 	no collision
 */
float PlayerTestCollision(t_ball *ball, t_paddle player);

 /*
  * test if the ball is over the player line
  * 	returns:1 -> ball is over line
  * 			0 -> ball is not over line
  */
int TestPlayerLine(t_ball *ball, t_line pLine);

/*
 * Tests if the ball is colliding with a point
 * and in that case bounces the ball
 * takes: ball pointer, and point position
 * returns: ball vel magnitude factor that has been updated
 * 			0-1:	if collision
 * 			0: 		no collision
 */
float PointTestCollision(t_ball *ball, t_vector pointPos);

/*
 * Tests if the ball is colliding with a sphere
 * and in that case bounces the ball
 * takes: ball pointer, and point position
 * returns: ball vel magnitude factor that has been updated
 * 			0-1:	if collision
 * 			0: 		no collision
 */
float SphereTestCollision(t_ball *ball, t_vector spherePos, float radius);

/*
 * Tests if the ball is colliding with wall
 * and in that case bounces the ball
 * takes: ball pointer, and wall line
 * returns: ball vel magnitude factor that has been updated
 * 			0-1:	if collision
 * 			0: 		no collision
 */
float TestWall(t_ball *ball, t_line wall);

/*
 * Bounces the ball
 * takes: ball pointer, the position where bounce occurred,
 * 		  the remaining velocity magnitude, and the surface(line) normal.
 */
void Bounce(t_ball *ball, t_vector bouncePosition, t_vector normal);

#endif /* COLLISION_H_ */
