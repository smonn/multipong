#include "../header/collision.h"

#define PADDLE_DIR_FACTOR 2.5
#define CORNER_RADIUS 15
#define ACC_PER_BOUNCE 1.10

void Bounce(t_ball *ball, t_vector bouncePos, t_vector normal) {
	t_vector inverseVel;
	inverseVel = Multiply(ball->velocity, -1.0);
	ball->velocity = Subtract(Multiply(Multiply(normal, DotProduct(inverseVel,normal)), 2.0), inverseVel);
	ball->velocity = Multiply(ball->velocity, ACC_PER_BOUNCE);
	ball->speed *= ACC_PER_BOUNCE;
	ball->position = bouncePos;
	/*
	 * Add(bouncePos, Multiply(ball->velocity, velRemainder));
	 */
}

int TestCollision(t_ball *ball, t_gameData *game, int nplayers, t_vector corners[], t_line walls[], int nWalls) {
	float temp;
	int j;
	for(j = 0; j < nplayers; j++) {
		if(game->players[j].lives>0){
			temp = PlayerTestCollision(ball, game->players[j].paddle);
			if(temp>0){
				return 1;
			}
		}
	}
	for(j = 0; j < nWalls; j++) {
		temp = TestWall(ball, walls[j]);
		if(temp>0){
			return 1;
		}
	}
	for(j = 0; j < nplayers; j++) {
		temp = SphereTestCollision(ball, corners[j], CORNER_RADIUS);
		if(temp>0){
			return 1;
		}
	}
	ball->position = Add(ball->position, ball->velocity);
	return 0;
}



/*
 * notes:
 *  if(ball.pos "dot" player.paddleLine.normal)<0
 *  		the ball has passed the paddle-line = goal!!
 */
float PlayerTestCollision(t_ball *ball, t_paddle player) {
	t_vector ballFront;
	t_vector collPos;
	t_vector paddleEdge1, paddleEdge2;
	float f = 0, f1, f2;
	ballFront = Subtract(ball->position, Multiply(player.paddleLine.normal ,ball->radius));
	if(DotProduct(Add(Subtract(ballFront, player.paddleLine.point), ball->velocity), player.paddleLine.normal)< 0){

		/*
		 * ball will overlap or be through line
		 */
		if(DotProduct(Subtract(ballFront, player.paddleLine.point), player.paddleLine.normal)>= 0){
			/*
			 * collision w/ line
			 */

			while(f <= 1 && DotProduct(Subtract(Add(ballFront, Multiply(ball->velocity, f)), player.paddleLine.point), player.paddleLine.normal)> 0){
				f+=0.1;
			}
			collPos = Add(Multiply(ball->velocity, f), ball->position);
			if(Length(Subtract(Multiply(player.paddleLine.direction, DotProduct(Subtract(collPos, player.paddleLine.point), player.paddleLine.direction)), Subtract(player.position, player.paddleLine.point)))<player.paddleWidth/2){
				Bounce(ball, collPos, player.paddleLine.normal);
				ball->velocity = Multiply(Normalized(Add(ball->velocity, Multiply(player.paddleLine.direction, DotProduct(Subtract(ball->position, player.position), player.paddleLine.direction) * PADDLE_DIR_FACTOR / player.paddleWidth))), ball->speed);
				return f;
			}
		}
		/*
		 * test edges
		 */
		paddleEdge1 = Add(player.position, Multiply(player.paddleLine.direction, player.paddleWidth/2));
		paddleEdge2 = Subtract(player.position, Multiply(player.paddleLine.direction, player.paddleWidth/2));
		f1 = PointTestCollision(ball, paddleEdge1);
		f2 = PointTestCollision(ball, paddleEdge2);
		if(f1>0||f2>0)
			return f1+f2;
	}
	return 0;
}
int TestPlayerLine(t_ball *ball, t_line pLine){
	if(DotProduct(Subtract(ball->position, pLine.point), pLine.normal)< 0)
		return 1;
	return 0;
}

float PointTestCollision(t_ball *ball, t_vector pointPos) {
	float f = 0;
	t_vector collPos;

	while(f <= 1 && LengthSquared(Subtract(Add(ball->position, Multiply(ball->velocity, f)), pointPos)) > (ball->radius * ball->radius))
		f+=0.1;
	collPos = Add(Multiply(ball->velocity, f), ball->position);
	if(f<1) {
		Bounce(ball, collPos, Normalized(Subtract(ball->position, pointPos)));
		return f;
	}
	return 0;
}

float TestWall(t_ball *ball, t_line wall) {
	t_vector ballFront;
	t_vector collPos;
	float f = 0;

	ballFront = Subtract(ball->position, Multiply(wall.normal ,ball->radius));
	if(DotProduct(Add(Subtract(ballFront, wall.point), ball->velocity), wall.normal)< 0){
		/*
		 * ball will overlap or be through line
		 */
		if(DotProduct(Subtract(ballFront, wall.point), wall.normal)>= 0){
			/*
			 * collision w/ line
			 */
			while(f <= 1 && DotProduct(Add(Subtract(ballFront, wall.point), Multiply(ball->velocity, f)), wall.normal)> 0){
				f+=0.1;
			}
			collPos = Add(Multiply(ball->velocity, f), ball->position);
			Bounce(ball, collPos, wall.normal);
			return f;
		}
		Bounce(ball, ball->position, wall.normal);
		return f;
	}
	return 0;
}

float SphereTestCollision(t_ball *ball, t_vector spherePos, float radius){
	float f = 0;
	t_vector collPos;
	/* ball direction is towards sphere, prevents ball from getting stuck */
	if(DotProduct(Subtract(spherePos, ball->position), ball->velocity)>0){
		while(f <= 1 && LengthSquared(Subtract(Add(ball->position, Multiply(ball->velocity, f)), spherePos)) > (ball->radius+radius)*(ball->radius+radius))
			f+=0.1;
		collPos = Add(Multiply(ball->velocity, f), ball->position);
		if(f<1) {
			Bounce(ball, collPos, Normalized(Subtract(ball->position, spherePos)));
			return f;
		}
	}
	return 0;
}
