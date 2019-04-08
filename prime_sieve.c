#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>

#define START 2
#define UPTO 100000
#define NUM_THREADS 10
#define TRUE 0
#define FALSE 1

// 2 to UPTO array of UPTO-1 positions
int primes[UPTO-1] = { TRUE };
int rangeStart[NUM_THREADS], rangeEnd[NUM_THREADS];

//thread function
void *sieveSegment(void *threadid)
{
	long t;
	t = (long)threadid;
	
	int i, j, k, l;
	k = rangeStart[(int)(t-65)];
	l = rangeEnd[(int)(t-65)];
	for(i=k; i<=l; i++)
	{
		for(j=START; j*j<=i; j++)
		{
			if( i % j == 0)	primes[i-2] = FALSE;
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	int i, j, k;
	
	pthread_t threads[NUM_THREADS];
	int rc;
	long t;
	for(t=65; t<(NUM_THREADS+65); t++)
	{
		printf("In main: creating thread %c\n", (char)t);
		
		if(t == 65)	rangeStart[t-65] = START;
		else rangeStart[t-65] = (UPTO/10)*((t-65)%10);
		
		if(t == (NUM_THREADS+64)) rangeEnd[t-65] = UPTO;
		else rangeEnd[t-65] = ((UPTO/10)*((t-64)%10) - 1);
		
		rc = pthread_create(&threads[t-65], NULL, sieveSegment, (void *)t);
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
	
	j = k = 0;
	for(i=START; i<=UPTO; i++)
	{
		if(primes[i-2] == TRUE)
		{
			k++;
			if(j < 9)
			{
				j++;
				printf("%d\t", i);
			}
			else
			{
				j = 0;
				printf("%d\n", i);
			}
		}
	}
	printf("\nThere are %d primes between %d and %d.", k, START, UPTO);
	
	return 0;
}