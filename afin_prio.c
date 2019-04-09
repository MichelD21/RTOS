#define _GNU_SOURCE
#include <sched.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 5
#define THREAD_EXEC_TIME 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *threadFunction(void *threadId)
{
	long tid;
	tid = (long)threadId;

	clockid_t cpuId;
	struct timespec timeStart, timeStop;

	long int isItDiff;

	pthread_mutex_lock(&mutex);
	pthread_mutex_unlock(&mutex);

	pthread_getcpuclockid( pthread_self(), &cpuId );

	clock_gettime( cpuId, &timeStart );
	clock_gettime( cpuId, &timeStop );
	
	isItDiff = 0;
	while( timeStop.tv_sec < timeStart.tv_sec + (long int)THREAD_EXEC_TIME )
	{
		clock_gettime( cpuId, &timeStop );
		if( isItDiff != ( 2 - timeStop.tv_sec) )
		{
			isItDiff = ( 2 - timeStop.tv_sec );
			printf("Thread %c here, and I need %ld more second(s).\n", (char)(tid+65), isItDiff);
		}

	}

	printf("Thread %c here, I am finished.\n", (char)(tid+65) );

	pthread_exit(NULL);
}

int main()
{

	pthread_t threads[NUM_THREADS];
	struct sched_param param;
	int i, rc, policy, prio_min_fifo, prio_max_fifo, prio_min_rr, prio_max_rr;
	long tid;

	prio_min_fifo = sched_get_priority_min(SCHED_FIFO);
	prio_max_fifo = sched_get_priority_max(SCHED_FIFO);
	
	prio_min_rr = sched_get_priority_min(SCHED_RR);
	prio_max_rr = sched_get_priority_max(SCHED_RR);

	cpu_set_t cpuSet;
	printf("Available Processors: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));

	printf("Prioridade FIFO = %d, RR = %d, OTHER = %d\n\n", SCHED_FIFO, SCHED_RR, SCHED_OTHER);

	// First item on the exercise:
	CPU_ZERO(&cpuSet);
	CPU_SET(0, &cpuSet);
	param.sched_priority = (int)prio_max_fifo/2;

	pthread_mutex_lock(&mutex);
	printf("Same affinity, same priority, FIFO:\n");
	for(tid=0; tid<NUM_THREADS; tid++)
	{
		rc = pthread_create( &threads[tid], NULL, threadFunction, (void *)tid );
		pthread_setaffinity_np( threads[tid], sizeof(cpuSet), &cpuSet);
		pthread_setschedparam(threads[tid], SCHED_FIFO, &param);
		printf("Created thread %c, Prio: %d, Afin: ", (char)(tid+65), param.sched_priority );
		CPU_ZERO(&cpuSet);
		pthread_getaffinity_np(threads[tid], sizeof(cpuSet), &cpuSet);
		for (i = 0; i < sysconf(_SC_NPROCESSORS_ONLN); i++)
		{
			if ( CPU_ISSET(i, &cpuSet) )
				printf("%d ", i);
		}
		printf("\n");
	}
	pthread_mutex_unlock(&mutex);
	
	for(tid=0; tid<NUM_THREADS; tid++)
		pthread_join(threads[tid], NULL);
	
	printf("\n");

	// Second item on the exercise:
	CPU_ZERO(&cpuSet);
	CPU_SET(0, &cpuSet);
	param.sched_priority = (int)prio_max_rr/2;

	pthread_mutex_lock(&mutex);
	printf("Same affinity, same priority, RR:\n");
	for(tid=0; tid<NUM_THREADS; tid++)
	{
		rc = pthread_create( &threads[tid], NULL, threadFunction, (void *)tid );
		pthread_setaffinity_np( threads[tid], sizeof(cpuSet), &cpuSet);
		pthread_setschedparam(threads[tid], SCHED_RR, &param);
		printf("Created thread %c, Prio: %d, Afin: ", (char)(tid+65), param.sched_priority );
		CPU_ZERO(&cpuSet);
		pthread_getaffinity_np(threads[tid], sizeof(cpuSet), &cpuSet);
		for (i = 0; i < sysconf(_SC_NPROCESSORS_ONLN); i++)
		{
			if ( CPU_ISSET(i, &cpuSet) )
				printf("%d ", i);
		}
		printf("\n");
	}
	pthread_mutex_unlock(&mutex);
	
	for(tid=0; tid<NUM_THREADS; tid++)
		pthread_join(threads[tid], NULL);

	printf("\n");

	// Third item on the exercise:
	CPU_ZERO(&cpuSet);
	CPU_SET(0, &cpuSet);

	pthread_mutex_lock(&mutex);
	printf("Same affinity, different priority, FIFO:\n");
	for(tid=0; tid<NUM_THREADS; tid++)
	{
		rc = pthread_create( &threads[tid], NULL, threadFunction, (void *)tid );
		pthread_setaffinity_np( threads[tid], sizeof(cpuSet), &cpuSet);
		param.sched_priority = (int)(prio_min_fifo + prio_max_fifo*tid/10);
		pthread_setschedparam(threads[tid], SCHED_FIFO, &param);
		printf("Created thread %c, Prio: %d, Afin: ", (char)(tid+65), param.sched_priority );
		CPU_ZERO(&cpuSet);
		pthread_getaffinity_np(threads[tid], sizeof(cpuSet), &cpuSet);
		for (i = 0; i < sysconf(_SC_NPROCESSORS_ONLN); i++)
		{
			if ( CPU_ISSET(i, &cpuSet) )
				printf("%d ", i);
		}
		printf("\n");
	}
	pthread_mutex_unlock(&mutex);
	
	for(tid=0; tid<NUM_THREADS; tid++)
		pthread_join(threads[tid], NULL);

	printf("\n");

	// Fourth item on the exercise:
	CPU_ZERO(&cpuSet);
	CPU_SET(0, &cpuSet);

	pthread_mutex_lock(&mutex);
	printf("Same affinity, different priority, RR:\n");
	for(tid=0; tid<NUM_THREADS; tid++)
	{
		rc = pthread_create( &threads[tid], NULL, threadFunction, (void *)tid );
		pthread_setaffinity_np( threads[tid], sizeof(cpuSet), &cpuSet);
		param.sched_priority = (int)(prio_min_rr + prio_max_rr*tid/10);
		pthread_setschedparam(threads[tid], SCHED_RR, &param);
		printf("Created thread %c, Prio: %d, Afin: ", (char)(tid+65), param.sched_priority );
		CPU_ZERO(&cpuSet);
		pthread_getaffinity_np(threads[tid], sizeof(cpuSet), &cpuSet);
		for (i = 0; i < sysconf(_SC_NPROCESSORS_ONLN); i++)
		{
			if ( CPU_ISSET(i, &cpuSet) )
				printf("%d ", i);
		}
		printf("\n");
	}
	pthread_mutex_unlock(&mutex);
	
	for(tid=0; tid<NUM_THREADS; tid++)
		pthread_join(threads[tid], NULL);

	printf("\n");

	// Fifth item on the exercise:
	param.sched_priority = (int)prio_max_fifo/2;

	pthread_mutex_lock(&mutex);
	printf("Different affinity, same priority, FIFO:\n");
	for(tid=0; tid<NUM_THREADS; tid++)
	{
		CPU_ZERO(&cpuSet);
		CPU_SET( (int)tid % sysconf(_SC_NPROCESSORS_ONLN), &cpuSet);
		rc = pthread_create( &threads[tid], NULL, threadFunction, (void *)tid );
		pthread_setaffinity_np( threads[tid], sizeof(cpuSet), &cpuSet);
		pthread_setschedparam(threads[tid], SCHED_FIFO, &param);
		printf("Created thread %c, Prio: %d, Afin: ", (char)(tid+65), param.sched_priority );
		CPU_ZERO(&cpuSet);
		pthread_getaffinity_np(threads[tid], sizeof(cpuSet), &cpuSet);
		for (i = 0; i < sysconf(_SC_NPROCESSORS_ONLN); i++)
		{
			if ( CPU_ISSET(i, &cpuSet) )
				printf("%d ", i);
		}
		printf("\n");
	}
	pthread_mutex_unlock(&mutex);
	
	for(tid=0; tid<NUM_THREADS; tid++)
		pthread_join(threads[tid], NULL);

	printf("\n");

	// Sixth item on the exercise:
	param.sched_priority = (int)prio_max_rr/2;

	pthread_mutex_lock(&mutex);
	printf("Different affinity, same priority, RR:\n");
	for(tid=0; tid<NUM_THREADS; tid++)
	{
		CPU_ZERO(&cpuSet);
		CPU_SET( (int)tid % sysconf(_SC_NPROCESSORS_ONLN), &cpuSet);
		rc = pthread_create( &threads[tid], NULL, threadFunction, (void *)tid );
		pthread_setaffinity_np( threads[tid], sizeof(cpuSet), &cpuSet);
		pthread_setschedparam(threads[tid], SCHED_RR, &param);
		printf("Created thread %c, Prio: %d, Afin: ", (char)(tid+65), param.sched_priority );
		CPU_ZERO(&cpuSet);
		pthread_getaffinity_np(threads[tid], sizeof(cpuSet), &cpuSet);
		for (i = 0; i < sysconf(_SC_NPROCESSORS_ONLN); i++)
		{
			if ( CPU_ISSET(i, &cpuSet) )
				printf("%d ", i);
		}
		printf("\n");
	}
	pthread_mutex_unlock(&mutex);
	
	for(tid=0; tid<NUM_THREADS; tid++)
		pthread_join(threads[tid], NULL);
	
	//getchar();
	
	return 0;
}
