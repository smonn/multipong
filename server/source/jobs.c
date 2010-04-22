#include "../header/jobs.h"

static pthread_mutex_t jobQueueMutex;
size_t queueCounter;

t_node* InitializeQueue(int nplayers) {
	size_t memsize;
	int length;
	t_node* queue;

	queueCounter = 0;

	length = nplayers * QUEUE_LENGTH;

	/* Allocate memory for QUEUE. */
	memsize = length * sizeof (t_node);
	queue = (t_node*) malloc(memsize);
	if (queue == NULL) {
		perror("JOBS::InitializeQueues - malloc");
		abort();
	}

	return queue;
}

void ClearQueue(t_node *queue) {
	free((void *) queue);
}

sem_t* InitializeSemaphore() {
	sem_t *sem;

	sem = sem_open("/multipongserver", O_CREAT, S_IRWXU, 0);
	if (sem == SEM_FAILED) {
		perror("JOBS::InitializeSemaphore - sem_open");
		abort();
	}

	return sem;
}

void ClearSemaphore(sem_t *sem) {
	if (sem_close(sem) == -1) {
		perror("sem_close");
	}
	if (sem_unlink("/multipongserver") == -1) {
		perror("JOBS::ClearSemaphore - sem_unlink");
	}
}

int PushJob(t_jobthread *data, t_node job) {
	struct timespec req, rem;
	int rval;

	/* Check if there's space left in the queue. */
	if (data->pos.head == data->pos.tail) {
		/* Queue might be empty... */
		if (data->queue[data->pos.head].callback != NULL) {
			/* Queue is full. */
			fprintf(stderr, "JOBS::PushJob - queue is full.\n");
			return -1;
		}
	}

	/* Free spots left, push the job. */
	pthread_mutex_lock(&jobQueueMutex);
	data->queue[data->pos.head++] = job;
	if (data->pos.head == (data->nplayers * QUEUE_LENGTH))
		data->pos.head = 0;
	sem_post(data->sem);
	queueCounter++;
	pthread_mutex_unlock(&jobQueueMutex);

	/* Prevent this function from being called to often. */
	req.tv_sec = 0;
	req.tv_nsec = 1000000;
	rval = nanosleep(&req, &rem);
	if (rval != 0) {
		perror("JOBS::PushJob - nanosleep");
	}

	return 0;
}

void ProcessJob(t_node job) {
	job.callback(job.argument, job.playerID);
	return;
}

int CreateJobThread(t_jobthread *data) {
	if (pthread_create(&(data->threadID), NULL, MainJobThread, data) != 0) {
		perror("JOBS::CreateJobThread - pthread_create");
		return -1;
	}

	return 0;
}

void CancelJobThread(pthread_t threadID) {
	if (pthread_cancel(threadID) != 0) {
		perror("JOBS::CancelJobThread - pthread_cancel");
	}
}

void JoinJobThread(pthread_t threadID) {
	if (pthread_join(threadID, NULL) != 0) {
		perror("JOBS::JoinJobThread - pthread_join");
	}
}

void* MainJobThread(void* arg) {
	t_jobthread *data = (t_jobthread *) arg;
	t_node nullJob;
	nullJob.callback = NULL;
	nullJob.argument = NULL;
	nullJob.playerID = -1;

	while (1) {
		t_node job;
		sem_wait(data->sem);

		pthread_mutex_lock(&jobQueueMutex);
		job = data->queue[data->pos.tail];
		data->queue[data->pos.tail++] = nullJob;
		if (data->pos.tail == (data->nplayers * QUEUE_LENGTH))
			data->pos.tail = 0;
		queueCounter--;
		pthread_mutex_unlock(&jobQueueMutex);

		ProcessJob(job);
	}

	printf("JOBS - job thread ending...\n");

	return NULL;
}

size_t getQueueSize() {
	size_t c;
	pthread_mutex_lock(&jobQueueMutex);
	c = queueCounter;
	pthread_mutex_unlock(&jobQueueMutex);
	return c;
}

