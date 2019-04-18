#define _GNU_SOURCE
#include <sched.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define NUM_THREADS 5
#define BUFFER_SIZE NUM_THREADS*3

char buffer[BUFFER_SIZE] = { 0 };
unsigned int bufferPos = 0;

// mutexes
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t auxMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// threads
void *threadProducer(void *threadId)
{
	long tid;
	tid = (long)threadId;
	
	pthread_mutex_lock(&auxMutex);
	pthread_mutex_unlock(&auxMutex);
	while(1)
	{
		pthread_mutex_lock(&mutex);
			while(bufferPos == BUFFER_SIZE)
				pthread_cond_wait(&cond, &mutex);
			buffer[bufferPos] = (char)(tid + 65);
			printf("Pos: %d\n", bufferPos);
			bufferPos++;
			printf("BUFFER: ");
			for(int i = 0; i < BUFFER_SIZE; i++)
				printf("[%c] ", buffer[i]);
			printf("\n");
		pthread_mutex_unlock(&mutex);
		usleep(1);

		pthread_cond_broadcast(&cond);
	}
}

void *threadConsumer(void *threadId)
{
	long tid;
	tid = (long)threadId;
	
	pthread_mutex_lock(&auxMutex);
	pthread_mutex_unlock(&auxMutex);
	while(1)
	{
		pthread_mutex_lock(&mutex);
			while(bufferPos == 0)
				pthread_cond_wait(&cond, &mutex);
			bufferPos--;
			printf("Pos: %d\n", bufferPos);
			buffer[bufferPos] = ' ';
			printf("BUFFER: ");
			for(int i = 0; i < BUFFER_SIZE; i++)
				printf("[%c] ", buffer[i]);
			printf("\n");
		pthread_mutex_unlock(&mutex);
		usleep(1);

		pthread_cond_broadcast(&cond);
	}
}

int main()
{
	pthread_t producers[NUM_THREADS];
	pthread_t consumers[NUM_THREADS];
	int i, s;
	long tid;
	
	// create
	pthread_mutex_lock(&auxMutex);
	for(tid=0; tid<NUM_THREADS; tid++)
	{
		s = pthread_create(&producers[tid], NULL, threadProducer, (void *)tid);
			if (s != 0)
				handle_error_en(s, "func: pthread_create");
		s = pthread_create(&consumers[tid], NULL, threadConsumer, (void *)tid);
			if (s != 0)
				handle_error_en(s, "func: pthread_create");
	}
	pthread_mutex_unlock(&auxMutex);

	for(tid=0; tid<NUM_THREADS; tid++)
	{
		pthread_join(producers[tid], NULL);
		pthread_join(consumers[tid], NULL);
	}
	return 0;
}
