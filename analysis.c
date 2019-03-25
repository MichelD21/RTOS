#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define TASKS		10
#define TRUE		1
#define FALSE		0

typedef struct task {
	
	char id;
    int priority;
    int Ctime;
	int Dtime;
	int Ptime;
	
}task;

// create tasks function
void createTasks(task *tasks, long *seed)
{
	int i;
	/* useful seeds:
		1553179206 - 58.98% < 71.77% utilization, response-time schedulable
		1553179287 - 77.06% > 71.77% utilization, response-time schedulable
		1553179396 - 97.22% > 71.77% utilization, response-time unschedulabe
	*/
	*seed = time(NULL);
	srand( *seed );
	for(i=0; i<TASKS; i++)
	{
		tasks[i].id = 65 + i;
		tasks[i].priority = 0;
		tasks[i].Ctime = (int)( ( rand() % 10 ) + 1 );
		tasks[i].Dtime = (int)( ( rand() % 90 ) + tasks[i].Ctime );
		tasks[i].Ptime = tasks[i].Dtime;
	}
}

// processor utilization analysis function
int utilAnalysis(task *tasks, float *util, float *utilBound)
{
	*util = 0;
	int i;
	for(i=0; i<TASKS; i++)
	{
		*util += ( (float)tasks[i].Ctime / (float)tasks[i].Ptime );
	}
	*utilBound = (float)TASKS * ( pow( 2, ( 1 / (float)TASKS ) ) - 1 );
	*util *= 100;
	*utilBound *= 100;
	if( *util < *utilBound ) return TRUE;
	else return FALSE;
}

// response-time analysis function
int responseTimeAnalysis(task *tasks, int *failed)
{
	int prevR, currR, i, j;
	int schedulable = TRUE;
	for(i=0; i<TASKS; i++)
	{
		prevR = 0;
		currR = tasks[i].Ctime;
		
		while( (currR <= tasks[i].Dtime) && (currR != prevR) )
		{
			prevR = currR;
			currR = tasks[i].Ctime;
			for(j=0; j<i; j++)
			{
				currR += ( ( ceil( (float)prevR / (float)tasks[j].Ptime ) ) * tasks[j].Ctime );
			}
		};
		if(currR > tasks[i].Dtime)
		{
			*failed = i;
			return FALSE;
		}
	}
	return TRUE;
}

// sort tasks function
void sortTasks(task *tasks)
{
	int i, sorted = FALSE;
	struct task taskTemp;
	while( sorted == FALSE )
	{
		sorted = TRUE;
		for(i=0; i<(TASKS-1); i++)
		{
			if( tasks[i].Ptime > tasks[i+1].Ptime )
			{
				taskTemp = tasks[i];
				tasks[i] = tasks[i+1];
				tasks[i+1] = taskTemp;
				sorted = FALSE;
			}
		}
	}
	for(i=0; i<TASKS; i++)
	{
		tasks[i].priority = i;
	}
}

// show task function (shortened)
void shortShowTasks(task *tasks)
{
	int i;
	printf("Tasks:      ");
	for(i=0; i<TASKS; i++)
	{
		printf("    %c", tasks[i].id);
	}
	printf("\nPriority:   ");
	for(i=0; i<TASKS; i++)
	{
		printf("    %d", tasks[i].priority);
	}
	printf("\nComputation:");
	for(i=0; i<TASKS; i++)
	{
		if(tasks[i].Ctime / 10 == 1) printf("   %d", tasks[i].Ctime);
		else printf("    %d", tasks[i].Ctime);
	}
	printf("\nDeadline:   ");
	for(i=0; i<TASKS; i++)
	{
		if(tasks[i].Dtime / 10 == 0) printf("    %d", tasks[i].Dtime);
		else if(tasks[i].Dtime / 100 == 0) printf("   %d", tasks[i].Dtime);
		else printf("  %d", tasks[i].Dtime);
	}
	printf("\nPeriod:     ");
	for(i=0; i<TASKS; i++)
	{
		if(tasks[i].Ptime / 10 == 0) printf("    %d", tasks[i].Ptime);
		else if(tasks[i].Ptime / 100 == 0) printf("   %d", tasks[i].Ptime);
		else printf("  %d", tasks[i].Ptime);
	}
	printf("\n\n");
}

// show tasks function
void showTasks(task *tasks)
{
	int i;
	for(i=0; i<TASKS; i++)
	{
		printf("Task        %c\n",tasks[i].id);
		printf("Computation %d\n",tasks[i].Ctime);
		printf("Deadline    %d\n",tasks[i].Dtime);
		printf("Period      %d\n\n",tasks[i].Ptime);
	}
}

int main(void) {
	
	int i, failed;
	float util, utilBound;
	long seed;
	
	printf("\n");
	
	// create task structs
	struct task tasks[TASKS];
	
	// create tasks
	createTasks(tasks, &seed);
	
	// sort tasks
	sortTasks(tasks);
	
	// show tasks (shortened)
	shortShowTasks(tasks);
	
	// show utilization result
	printf("According to Utilization Analysis: ");
	if( utilAnalysis(tasks, &util, &utilBound) ) printf("Schedulable :)\n");
	else printf("Unschedulable :(\n");
	printf("Utilization: %.2f%% - Boundary: %.2f%% \n", util, utilBound);
	
	// show response-time result
	printf("According to Response-time Analysis: ");
	if(responseTimeAnalysis(tasks, &failed)) printf("Schedulable :)\n");
	else
	{
		printf("Unschedulable :(\n");
		if( failed == (TASKS-1) )
		{
			printf("Task %c failed to execute before deadline.\n", tasks[TASKS-1].id);
		}
		else
		{
			printf("Tasks ");
			for(i=failed; i<(TASKS-1); i++)
			{
				printf("%c, ", tasks[i].id);
			}
			printf("%c failed to execute before deadline.\n", tasks[TASKS-1].id);
		}
	}
	printf("Seed used: %ld.\n", seed);
	// getchar();
}