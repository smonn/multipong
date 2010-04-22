/*
 *		  File: jobs.h
 *		Author: simoni
 * Description: This module contains functions for creating a thread
 *				which processes a dynamically allocated queue. It also
 *				contains functions for interacting with said queue.
 *	   Created: April 20th 2009, 20:58
 */

#ifndef JOBS_H_
#define JOBS_H_

/*
 * Queue length that will be allocated. Will be multiplied by the
 * number of active players to allocate enough memory for the queue.
 *
 * For example, for 3 players, 3 * QUEUE_LENGTH * sizeof (t_node)
 * will be allocated.
 */
#define QUEUE_LENGTH 10

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

/*
 * This node equals one job for one client.
 */
typedef struct t_node {
	/* Function to call, must have two arguments, the first needs to be 
	 * converted to a void pointer, the second must be an integer.*/
	void (*callback)(void*, int);
	/* Arguments that is passed to the callback function. */
	void *argument;
	int playerID;

	/* Do we need more stuff here? */
} t_node;

/*
 * Indexes for reading and writing to a queue.
 */
typedef struct position {
	int head;	/* At which index data should be stored. */
	int tail;	/* At which index data should be read. */
} t_position;

/*
 * This will be used when calling most of the functions in this module.
 */
typedef struct jobthread {
	pthread_t threadID;		/* Pointer to a thread ID. */
	int nplayers;			/* Number of players. */
	t_position pos;			/* An initialized position. */
	t_node *queue;			/* An array with cnodes. */
	sem_t *sem;				/* Semaphore to keep track... */
} t_jobthread;

/*
 * Allocates memory and creates a QUEUE big enough for NPLAYERS clients.
 * Should be called before CreateJobThread.
 *
 * Calls abort() on failure, returns a pointer to a queue on success.
 * The abort() call makes error checking unnecessary.
 */
t_node* InitializeQueue(int nplayers);

/*
 * Frees the allocated memory for QUEUE and POS.
 *
 * Needs to be explicitly called when the job thread is done.
 */
void ClearQueue(t_node *queue);

/*
 * Wrapper for sem_open() - we're using named semaphores.
 *
 * Calls abort() on failure, returns a pointer to a semaphore on success.
 * The abort() call makes error checking unnecessary.
 */
sem_t* InitializeSemaphore();

/*
 * Closes the semaphore and unlinks it.
 * 
 * Wrapper for sem_close() and sem_unlink().
 */
void ClearSemaphore(sem_t *sem);

/*
 * Save a new JOB in queue, if there's space left.
 *
 * Returns -1 on failure, 0 on success.
 */
int PushJob(t_jobthread *data, t_node job);

/*
 * Process the next job on the QUEUE.
 */
void ProcessJob(t_node job);

/*
 * Creates the job thread (wrapper for pthread_create).
 *
 * Returns -1 on failure, 0 on success.
 */
int CreateJobThread(t_jobthread *data);

/*
 * Cancels the job thread.
 */
void CancelJobThread(pthread_t threadID);

/*
 * Pick up the job thread.
 */
void JoinJobThread(pthread_t threadID);

/*
 * The main function for the job thread.
 *
 * Returns NULL.
 */
void* MainJobThread(void* arg);

/*
 * Returns the current size of the queue.
 */
size_t getQueueSize();

#endif /* JOBS_H_ */
