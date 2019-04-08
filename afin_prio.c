#define _GNU_SOURCE
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#define NUM_THREADS 5
#define TASK_EXEC_TIME 2

// -lrt

void *threadFunction(void *threadId)
{
	long tid;
	tid = (long)threadId;

	clockid_t cpuId;
	struct timespec timeStart, timeStop;	

	if( pthread_getcpuclockid(pthread_self(), &cpuId) != 0 )
	{
      perror( "pthread get cpu lock id error\n" );
      exit(-1);
    }
	
	if( clock_gettime( cpuId, &timeStart ) != 0 )
	{
      perror( "clock gettime error\n" );
      exit(-1);
    }
	if( clock_gettime( cpuId, &timeStop ) != 0 )
	{
      perror( "clock gettime error\n" );
      exit(-1);
    }
	printf("Thread %c here, I started on %lf\n", (char)(tid+65), (double)timeStart.tv_sec);
	while( timeStop.tv_sec - timeStart.tv_sec != (double)TASK_EXEC_TIME )
	{
		//printf("Thread %c here, and I still need %ld seconds.\n", (char)(tid+65), (float)timeStop.tv_sec - (float)timeStart.tv_sec);
		clock_gettime( cpuId, &timeStop );
	}
	
	getchar();

	pthread_exit(NULL);
}

int main()
{

	pthread_t threads[NUM_THREADS];
	struct sched_param param;
	int i, rc, policy, prio_min, prio_max;
	long tid;

	for(tid=0; tid<NUM_THREADS; tid++)
		rc = pthread_create( &threads[tid], NULL, threadFunction, (void *)tid );

	// Obtem parametros da thread
	for(i=0; i<NUM_THREADS; i++)
	{
		if (pthread_getschedparam(threads[i], &policy, &param) != 0)
		{
			printf("Erro pthread_getschedparam!\n");
			return 0;
		}
		printf("Escalonador = %d, Prioridade = %d\n", policy, param.sched_priority);
	}
	
	// Prioridade minimas e maximas para escalonador FIFO
	printf("Prioridade minima FIFO: %d\n", prio_min = sched_get_priority_min(SCHED_FIFO));
	printf("Prioridade maxima FIFO: %d\n", prio_max = sched_get_priority_max(SCHED_FIFO));
	
	// Seta prioridade da thread
	param.sched_priority = (int)prio_max/2;
	for(i=0; i<NUM_THREADS; i++)
	{
		if (pthread_setschedparam(threads[i], SCHED_FIFO, &param) != 0)
		{
			printf("Erro pthread_setschedparam!\n");
			return 0;
		}
	}
	
	// Obtem parametros da thread
	for(i=0; i<NUM_THREADS; i++)
	{
		if (pthread_getschedparam(threads[i], &policy, &param) != 0)
		{
			printf("Erro pthread_getschedparam!\n");
			return 0;
		}
	}
	printf("Escalonador = %d, Prioridade = %d\n", policy, param.sched_priority);
	
	for(i=0; i<NUM_THREADS; i++)
		pthread_join(threads[i], NULL);
	
	getchar();
	
	return 0;
}
