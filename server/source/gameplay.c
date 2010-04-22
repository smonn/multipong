#include "../header/gameplay.h"

#define UPDATES_PER_SEC 60
#define BALL_SPEED 2.3
#define BALL_RADIUS 9.0
#define CORNER_RADIUS 15
#define PADDLE_SPEED 4.0

/* player movements */
t_direction playermoves[5];

/*
 * Template for the callback function.
 *
 * This function is called from the JOB thread. Its purpose is to update
 * the position of a single players paddle. Shouldn't return anything.
 *
 * It's very important that the types match!
 *
 * We need to convert the type since JOBS.H won't have access to the
 * t_direction type.
 */
static void UpdatePaddlePosition(void *direction, int playerID) {
	t_direction *dir = (t_direction*) direction;
	playermoves[playerID] = *dir;
}

/*
 * The main INPUT thread function. Should never return anything.
 * Runs until cancelled from the thread that called it.
 */
static void* MainInputThread(void* arg) {
	t_inputopts *opts = (t_inputopts*) arg;
	int inputFD;

	/* Get UDP socket... */
	inputFD = InitializeUDPInput(opts->serverIP, opts->port);
	if (inputFD == -1) {
		perror("GAMEPLAY::InitializeUDPInput - failed\n");
		abort();
	}

	/* Run forever... */
	while (1) {
		t_direction dir;
		t_node job;

		/* Get new data from one client. */
		job.playerID = RecvUDPInput(inputFD, opts->job.nplayers,
				opts->clientIPs, &dir);

		/* Player IDs range from 0 to 4, anything else is bad. */
		/*if (job.playerID < 0 || (job.playerID + 2) > opts->nplayers) {*/
		if (job.playerID == -1) {
			fprintf(stderr, "GAMEPLAY::MainInputThread - RecvUDPInput: "
					"got bad player ID\n");
			continue; /* Close our eyes and hope the error goes away. */
		}

	/*	printf("GAMEPLAY - direction: %d\n"
			   "            playerID: %d\n", dir, job.playerID);*/

		/* Set the correct callback here. */
		job.callback = &UpdatePaddlePosition;
		/* Cast our direction to a void pointer to prepare it for our
		 * callback function. */
		job.argument = (void*) &dir;

		/* Add this to our queue. */
		PushJob(&(opts->job), job);
	}

	printf("GAMEPLAY - input thread ending...\n");

	return NULL;
}

/*
 * Wrapper for pthread_create, creates the INPUT thread.
 *
 * Returns -1 on failure, 0 on success.
 */
static int CreateInputThread(t_inputopts *data) {
	if (pthread_create(&(data->threadID), NULL,
				MainInputThread, data) != 0) {

		perror("GAMEPLAY::CreateInputThread - pthread_create");
		return -1;
	}

	return 0;
}

/*
 * Wrapper for pthread_cancel, cancels the INPUT thread.
 */
static void CancelInputThread(pthread_t threadID) {
	if (pthread_cancel(threadID) != 0) {
		perror("GAMEPLAY::CancelInputThread - pthread_cancel");
	}
}

/*
 * Wrapper for pthread_join, joins the INPUT thread.
 */
static void JoinInputThread(pthread_t threadID) {
	if (pthread_join(threadID, NULL) != 0) {
		perror("GAMEPLAY::JoinInputThread - pthread_join");
	}
}

/* Starts the game... */
int StartGame(
		int nplayers,
		char serverIP[17],
		char clientIPs[][17],
		int port
		) {

	t_node job;
	t_inputopts opts;
	int i, outputFD;
	/* Init players */
	t_vector *point;
	t_gameData game;
	t_vector middle;
	int plAlive, j, winner;
	int sideLength;
	int nWalls = 0;
	t_line *walls;
	/* init time */
	struct timeval time1, time2;
	struct timespec req, rem;
	int rval;

	/* stats */
	unsigned int queueSizeSum = 0;
	unsigned int lapCount = 0;


	printf("GAMEPLAY: nplayers: %d\n"
		   "          serverIP: %s\n"
		   "              port: %d\n", nplayers, serverIP, port);
	for (i = 0; i < nplayers; i++) {
		printf("       clientIP[%d]: %s\n", i, clientIPs[i]);
	}

	for(i = 0; i < 5; i++)
		playermoves[i] = NONE;

	/* Create socket for output, input sockets is handled by the INPUT
	 * thread. This might need to be corrected, check communication.h. */
	outputFD = InitializeUDPOutput();
	if (outputFD == -1) {
		perror("GAMEPLAY::StartGame - bad socket");
		return -1;
	}

	/* Setup the JOB thread. */

	/* Setting default values for the JOB and INPUT threads. */
/*	printf("GAMEPLAY - setting options\n");*/
	job.callback      = NULL;
	job.argument      = NULL;
	job.playerID      = -1;
	opts.port         = port;
	opts.job.nplayers = nplayers;
	opts.job.pos.head = 0;
	opts.job.pos.tail = 0;
	opts.job.queue    = InitializeQueue(nplayers);
	opts.job.sem      = InitializeSemaphore();
	strcpy(opts.serverIP, serverIP);
	for (i = 0; i < nplayers; i++) {
		strcpy(opts.clientIPs[i], clientIPs[i]);
	}
	for (i = 0; i < (nplayers * QUEUE_LENGTH); i++) {
		opts.job.queue[i] = job;
	}
	/* Done setting default values for the JOB and INPUT threads. */

	/* Start the JOB and INPUT threads. */
	if (CreateJobThread(&(opts.job)) == -1) {
		perror("GAMEPLAY::StartGame - CreateJobThread");
		exit(EXIT_FAILURE);
	}
	if (CreateInputThread(&opts) == -1) {
		perror("GAMEPLAY::StartGame - CreateInputThread");
		exit(EXIT_FAILURE);
	}


	/* Setup the main loop here. */
/*	printf("GAMEPLAY - entering main loop\n");*/

	/*FREE LATER*/
	point = malloc(sizeof *point *5);

	plAlive = nplayers;
	middle.x = 500;
	middle.y = 300;
	switch(nplayers){
	case 2:
	case 4:
		sideLength = 580;
		point[0].x = 790;
		point[0].y = 590;
		point[1].x = 210;
		point[1].y = 590;
		point[2].x = 210;
		point[2].y = 10;
		point[3].x = 790;
		point[3].y = 10;
		point[4].x = 0;
		point[4].x = 0;
		break;
	case 3:
		sideLength = 580;
		point[0].x = 790;
		point[0].y = 551;
		point[1].x = 210;
		point[1].y = 551;
		point[2].x = 500;
		point[2].y = 49;
		point[3].x = 0;
		point[3].y = 0;
		point[4].x = 0;
		point[4].x = 0;
		break;
	case 5:
		sideLength = 350;
		point[0].x = 680;
		point[0].y = 560;
		point[1].x = 330;
		point[1].y = 560;
		point[2].x = 210;
		point[2].y = 220;
		point[3].x = 500;
		point[3].y = 10;
		point[4].x = 790;
		point[4].y = 220;
		break;
	default:
		/* other number of players than 2,3,4 or 5*/
		break;
	}

	/*init players*/
	for(j = 0; j < nplayers; j++){
		if(nplayers>2)
			game.players[j].paddle.paddleLine =
				(j == nplayers-1)? CreateLine(point[j], point[0], middle)
				: CreateLine(point[j], point[j+1], middle);
		else
			game.players[j].paddle.paddleLine = CreateLine(point[2*j], point[2*j+1], middle);
		game.players[j].paddle.position = Add(game.players[j].paddle.paddleLine.point,
				Multiply(game.players[j].paddle.paddleLine.direction, 200));
		game.players[j].paddle.paddleWidth = (nplayers == 5)? 40: 45;
		game.players[j].lives = 5;
	}

	/* init walls */
	walls = malloc(sizeof *walls * 5);
	if(nplayers == 2) {
		nWalls = 2;
		walls[0] = CreateLine(point[1], point[2], middle);
		walls[1] = CreateLine(point[3], point[0], middle);
	}

	/* init ball*/
	game.ball.position = middle;
	game.ball.velocity = Multiply(Normalized(Subtract(middle, game.players[0].paddle.position)), BALL_SPEED);
	game.ball.speed = BALL_SPEED;
	game.ball.radius = BALL_RADIUS;

	/* MAIN LOOP */
	req.tv_sec = 0;
	req.tv_nsec = 1000000;
	gettimeofday(&time1, NULL);
	while(plAlive > 1) {
		lapCount++;
		queueSizeSum += (int) getQueueSize();
	/*	int count = 0;*/
		/* SEND DATA */
/*		printf("GAMEPLAY - sending data to players\n");*/
/*		printf(".");
		fflush(stdout);
		if (++count == 50) {
			count = 0;
			printf("\n");
		}*/
		if(SendUDPOutput(outputFD, nplayers, clientIPs, game) == -1){
			perror("gameplay::UDPoutput");
			return -1;
		}
		/* measure time & nanosleep */
/*		printf("GAMEPLAY - resting\n");*/
		do{
			rval = nanosleep(&req, &rem);
			if (rval != 0) {
				perror("gameplay::nanosleep");
			}
			gettimeofday(&time2, NULL);
		}while((time2.tv_sec*1000+time2.tv_usec/1000)-(time1.tv_sec*1000+time1.tv_usec/1000) < 1000/UPDATES_PER_SEC);
		gettimeofday(&time1, NULL);
		/* READ DATA */
/*		printf("GAMEPLAY - reading and updating positions\n");*/
		for(j = 0;j < nplayers; j++){
			if(playermoves[j] && game.players[j].lives>0) {
				if(playermoves[j] == RIGHT && LengthSquared(Subtract(game.players[j].paddle.paddleLine.point, game.players[j].paddle.position))>
									(game.players[j].paddle.paddleWidth/2+CORNER_RADIUS)*(game.players[j].paddle.paddleWidth/2+CORNER_RADIUS))
					game.players[j].paddle.position = Add(game.players[j].paddle.position,Multiply(game.players[j].paddle.paddleLine.direction, -PADDLE_SPEED));
				if(playermoves[j] == LEFT && LengthSquared(Subtract(game.players[j].paddle.paddleLine.point, game.players[j].paddle.position)) < (sideLength - game.players[j].paddle.paddleWidth/2+CORNER_RADIUS)*(sideLength - game.players[j].paddle.paddleWidth/2+CORNER_RADIUS))
					game.players[j].paddle.position = Add(game.players[j].paddle.position,Multiply(game.players[j].paddle.paddleLine.direction,PADDLE_SPEED));
			}
		}

		/*
		 * print queue size
		 */
/*		printf("queue size: %zu\n", getQueueSize());*/


		/* test Collision */
/*		printf("GAMEPLAY - testing collisions\n");*/
		TestCollision(&(game.ball), &game, nplayers, point, walls, nWalls);

		/* test if ball goes over Player.paddle.line */
/*		printf("GAMEPLAY - checking if somebody scored\n");*/
		for(j = 0; j < nplayers; j++){
			if(game.players[j].lives>0)
				if(TestPlayerLine(&(game.ball), game.players[j].paddle.paddleLine)) {
					game.players[j].lives--;
					printf("Player with id:%d lost a life! Player now has %d lives left\n",j ,game.players[j].lives);
					game.ball.position = middle;
					game.ball.velocity = Multiply(Normalized(Subtract(game.players[j].paddle.position, middle)), BALL_SPEED);
					game.ball.speed = BALL_SPEED;
					if(game.players[j].lives==0){
						plAlive--;
						walls[nWalls++] = game.players[j].paddle.paddleLine;
					}
				}
		}
	}

	printf("GAMEPLAY - only one left...\n");
	if(SendUDPOutput(outputFD, nplayers, clientIPs, game) == -1){
		perror("gameplay::UDPoutput");
		return -1;
	}
	for(j = 0; j < nplayers; j++)
		if(game.players[j].lives>0)
			winner = j;

	printf("GAMEPLAY - player %d won\n", winner);
	/*
	 * "MAP":
	 *V while(plAlive > 1)
	 *v		send data
	 *V		Time control
	 *v		read data
	 *v		update
	 *V		-	paddle positions
	 *V		-	test collision
	 *V			-	players
	 *V				-	paddle
	 *V				-	Line
	 *V					-	decrement Life
	 *v					->	*(exit point?)
	 *V			-	corners
	 *V			-	walls
	 *V		->	ball position(through TestCollision())
	 */

	free(point);
	free(walls);
	/* Function calls to end the JOB and INPUT threads.
	 * Should be used when the game should end. */
	CancelJobThread(opts.job.threadID);
	JoinJobThread(opts.job.threadID);
	CancelInputThread(opts.threadID);
	JoinInputThread(opts.threadID);

	/* Cleaning up the JOB thread, freeing allocated memory. */
	ClearQueue(opts.job.queue);
	ClearSemaphore(opts.job.sem);

	printf("GAMEPLAY - average queue size: %.3f\n",
			(queueSizeSum / (float) lapCount));

	printf("GAMEPLAY - done, returning to lobby...\n");
	return winner;
}
