#include <stdlib.h>
#include <stdio.h>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>

#define NUM_THREADS 10

int global = 0;

// thread function
void *threadFunction(void *threadid)
{
	long tid;
	tid = (long)threadid;
	while(global < 1000)
	{
		global++;
		printf("Thread %c, iterating global to %d.\n", (char)tid, global);
	}
	printf("Thread %c can't iterate anymore, shutting down.\n", (char)tid, global);
	pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
	pthread_t threads[NUM_THREADS];
	int rc;
	long t;
	for(t=65; t<(NUM_THREADS+65); t++)
	{
		printf("In main: creating thread %c\n", (char)t);
		rc = pthread_create(&threads[t-65], NULL, threadFunction, (void *)t);
		if (rc)
		{
			printf("ERROR; return code from pthread_create() is %d\n", rc); 
			exit(-1); 
		}
	}
	for(t=65; t<NUM_THREADS+65; t++)
	{
		pthread_join(threads[t-65], NULL);
	}
	
	return 0;
}
