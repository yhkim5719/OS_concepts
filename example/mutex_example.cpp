#include <pthread.h>
#include <stdio.h>

/* This data is shared by the thread(s) */
int count = 0;                         //The counter variable.
pthread_t t1, t2;       //The thread variables.
pthread_mutex_t myMutex; //The mutex


/* the thread */
void *runner(void *param); 
int sum = 0;

int main(int argc, char *argv[] )
{
	pthread_attr_t attr; /* Set of thread    attributes */

	/* Get the default attributes */
	pthread_attr_init(&attr);


	/* Initialize the mutex */
	pthread_mutex_init(&myMutex, NULL);


	/* create the thread */
	pthread_create(&t1, &attr, runner, NULL);
	pthread_create(&t2, &attr, runner, NULL);
	/* wait for the thread to exit */
	pthread_join(t1, NULL);
	pthread_join(t2,NULL);
	
	
	fprintf(stderr, "count = %d\n", count);
} //End of main()

/* The thread will begin control in this function */
void *runner(void *param)
{
	/* Lock the mutex to allow only one thread */
	//pthread_mutex_lock(&myMutex);
	for (int i = 0; i < 10; ++i) {
		count += 1;
		printf("%d, ", count);
	}
	printf("\n");
		/* Unlock the mutex */
	//pthread_mutex_unlock(&myMutex);
	
	
	pthread_exit(0);
}

