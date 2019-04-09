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
#define THREAD_EXEC_TIME 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *threadFunction(void *threadId)
{
	long tid, isItDiff;
	tid = (long)threadId;
	int s;

	clockid_t cpuId;
	struct timespec timeStart, timeStop;

	pthread_mutex_lock(&mutex);
	pthread_mutex_unlock(&mutex);

	s = pthread_getcpuclockid(pthread_self(), &cpuId);
		if (s != 0)
			handle_error_en(s, "thread: pthread_getcpuclockid");
	s = clock_gettime(cpuId, &timeStart);
		if (s != 0)
			handle_error_en(s, "thread: clock_gettime");
	s = clock_gettime(cpuId, &timeStop);
		if (s != 0)
			handle_error_en(s, "thread: clock_gettime");
	
	isItDiff = 0;
	while( timeStop.tv_sec < timeStart.tv_sec + (long int)THREAD_EXEC_TIME )
	{
		s = clock_gettime(cpuId, &timeStop);
			if (s != 0)
				handle_error_en(s, "thread: clock_gettime");
		if( isItDiff != ( 2 - timeStop.tv_sec) )
		{
			isItDiff = ( 2 - timeStop.tv_sec );
			if( isItDiff == 0 )
				printf("Thread %c here, I am finished.\n", (char)(tid+65));
			else
				printf("Thread %c here, and I need %ld more second(s).\n", (char)(tid+65), isItDiff);
		}

	}

	pthread_exit(NULL);
}

void createThreadFunc(int affin, int prio, int policy)
{
	pthread_t threads[NUM_THREADS];
	struct sched_param param;
	cpu_set_t cpuSet;
	int i, s, prioMinFIFO, prioMaxFIFO, prioMinRR, prioMaxRR;
	long tid;
	
	// Setup priorities
	prioMinFIFO = sched_get_priority_min(SCHED_FIFO);
	prioMaxFIFO = sched_get_priority_max(SCHED_FIFO);

	prioMinRR = sched_get_priority_min(SCHED_RR);
	prioMaxRR = sched_get_priority_max(SCHED_RR);

	pthread_mutex_lock(&mutex);
	for(tid=0; tid<NUM_THREADS; tid++)
	{
		// create
		s = pthread_create(&threads[tid], NULL, threadFunction, (void *)tid);
			if (s != 0)
				handle_error_en(s, "func: pthread_create");
		// scheduling and priority
		if( policy == SCHED_FIFO )
		{
			if( prio )
				param.sched_priority = (int)(prioMinFIFO + tid);
			else
				param.sched_priority = (int)prioMaxFIFO/2;
		}
		else
		{
			if( prio )
				param.sched_priority = (int)(prioMinRR + tid);
			else
				param.sched_priority = (int)prioMaxRR/2;
		}
		s = pthread_setschedparam(threads[tid], policy, &param);
			if (s != 0)
				handle_error_en(s, "func: pthread_setschedparam");
		// allocate
		CPU_ZERO(&cpuSet);
		if( affin )
			CPU_SET(0, &cpuSet);
		else
			CPU_SET( (int)tid % sysconf(_SC_NPROCESSORS_ONLN), &cpuSet);
		s = pthread_setaffinity_np(threads[tid], sizeof(cpuSet), &cpuSet);
			if (s != 0)
				handle_error_en(s, "func: pthread_setaffinity_np");
		
		// show info
		printf("Created thread %c, Prio: %d, Affin: ", (char)(tid+65), param.sched_priority );
		CPU_ZERO(&cpuSet);
		s = pthread_getaffinity_np(threads[tid], sizeof(cpuSet), &cpuSet);
			if (s != 0)
				handle_error_en(s, "func: pthread_getaffinity_np");
		for (i = 0; i < sysconf(_SC_NPROCESSORS_ONLN); i++)
			if (CPU_ISSET(i, &cpuSet))
				printf("%d ", i);
		printf("\n");
	}
	pthread_mutex_unlock(&mutex);

	for(tid=0; tid<NUM_THREADS; tid++)
		pthread_join(threads[tid], NULL);
}

int main()
{
	int i, s, rc, policy, prio_min_fifo, prio_max_fifo, prio_min_rr, prio_max_rr;
	long tid;
	struct sched_param param;

	// Give parent process priority over threads
	param.sched_priority = sched_get_priority_max(SCHED_FIFO);
	s = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
		if (s != 0)
			handle_error_en(s, "main: pthread_setschedparam");
	printf("Parent process has maximum FIFO priority (%d)\n", param.sched_priority);

	// Show CPUs
	printf("Available CPUs: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));

	// Show Policies Code Number
	printf("FIFO = %d, RR = %d, OTHER = %d\n\n", SCHED_FIFO, SCHED_RR, SCHED_OTHER);

	// Show results
	printf("Same affinity, same priority, FIFO:\n");
	createThreadFunc(1, 0, SCHED_FIFO);
	printf("\n");
	printf("Same affinity, same priority, RR:\n");
	createThreadFunc(1, 0, SCHED_RR);
	printf("\n");
	printf("Same affinity, different priority, FIFO:\n");
	createThreadFunc(1, 1, SCHED_FIFO);
	printf("\n");
	printf("Same affinity, different priority, RR:\n");
	createThreadFunc(1, 1, SCHED_RR);
	printf("\n");
	printf("Different affinity, same priority, FIFO:\n");
	createThreadFunc(0, 0, SCHED_FIFO);
	printf("\n");
	printf("Different affinity, same priority, RR:\n");
	createThreadFunc(0, 0, SCHED_RR);
	printf("\n");
	
	//getchar();
	
	return 0;
}
