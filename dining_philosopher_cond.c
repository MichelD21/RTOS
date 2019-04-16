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
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
//pthread_mutex_t cutlery[NUM_THREADS] = PTHREAD_MUTEX_INITIALIZER;
int cutlery[NUM_THREADS] = { 0 };

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
		printf("Philosopher %d is thinking.\n", (int)philoNum);
		sleep(1);

		pthread_mutex_lock(&mutex);

			while( cutlery[(int)philoNum] == 1 )
				pthread_cond_wait(&cond, &mutex);
			cutlery[philoNum] = 1;
			usleep(5);
			printf("Philosopher %d has grabbed cutlery #%d.\n", (int)philoNum, (int)philoNum);

			while( cutlery[((int)(philoNum+1) % NUM_THREADS)] == 1 )
				pthread_cond_wait(&cond, &mutex);
			cutlery[((int)(philoNum+1) % NUM_THREADS)] = 1;
			usleep(5);
			printf("Philosopher %d has grabbed cutlery #%d.\n", (int)philoNum, ((int)(philoNum+1) % NUM_THREADS));
			
			printf("Philosopher %d is dining.\n", (int)philoNum);
			sleep(1);

			cutlery[philoNum] = 0;
			printf("Philosopher %d has released cutlery #%d.\n", (int)philoNum, (int)philoNum);
			pthread_cond_broadcast(&cond);
			cutlery[((int)(philoNum+1) % NUM_THREADS)] = 0;
			printf("Philosopher %d has released cutlery #%d.\n", (int)philoNum, ((int)(philoNum+1) % NUM_THREADS));
			pthread_cond_broadcast(&cond);
	
		pthread_mutex_unlock(&mutex);
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
