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

// mutexes
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t *cutlery;
pthread_mutex_t cutlery[NUM_THREADS] = PTHREAD_MUTEX_INITIALIZER;

// threads
void *threadPhilosopher(void *threadId)
{
	long philoNum;
	philoNum = (long)threadId;
	int s;
	
	pthread_mutex_lock(&mutex);
	pthread_mutex_unlock(&mutex);
	while(1)
	{
		pthread_mutex_lock(&mutex);
		pthread_mutex_lock(&cutlery[(int)philoNum]);
		printf("Philosopher %d has grabbed left cutlery #%d.\n", (int)philoNum, (int)philoNum);
		pthread_mutex_unlock(&mutex);
			pthread_mutex_lock(&mutex);
			pthread_mutex_lock(&cutlery[((int)(philoNum+1) % NUM_THREADS)]);
			printf("Philosopher %d has grabbed right cutlery #%d.\n", (int)philoNum, ((int)(philoNum+1) % NUM_THREADS));
			pthread_mutex_unlock(&mutex);
			//usleep(5);
			printf("Philosopher %d is dining.\n", (int)philoNum);
			sleep(1);
			pthread_mutex_lock(&mutex);
			pthread_mutex_unlock(&cutlery[((int)(philoNum+1) % NUM_THREADS)]);
			printf("Philosopher %d has released right cutlery #%d.\n", (int)philoNum, ((int)(philoNum+1) % NUM_THREADS));
			pthread_mutex_unlock(&mutex);
			//usleep(5);
		pthread_mutex_lock(&mutex);
		pthread_mutex_unlock(&cutlery[(int)philoNum]);
		printf("Philosopher %d has released left cutlery #%d.\n", (int)philoNum, (int)philoNum);
		pthread_mutex_unlock(&mutex);
		//usleep(5);
		printf("Philosopher %d is thinking.\n", (int)philoNum);
		sleep(1);
	}
}

int main()
{
	pthread_t threads[NUM_THREADS];
	int i, s;
	long tid;
	
	// create
	pthread_mutex_lock(&mutex);
	for(tid=0; tid<NUM_THREADS; tid++)
	{
		s = pthread_create(&threads[tid], NULL, threadPhilosopher, (void *)tid);
			if (s != 0)
				handle_error_en(s, "func: pthread_create");
	}
	pthread_mutex_unlock(&mutex);

	for(tid=0; tid<NUM_THREADS; tid++)
		pthread_join(threads[tid], NULL);
	
	return 0;
}
