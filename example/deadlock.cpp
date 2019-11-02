#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* thread_one runs in this function */
pthread_mutex_t first_mutex;
pthread_mutex_t second_mutex;
pthread_t t1,t2;
pthread_attr_t attr;

/* t1 runs in this function */
void *do_work_one(void* param)
{
	pthread_mutex_lock(&first_mutex);
	pthread_mutex_lock(&second_mutex);
	
	fprintf(stderr, "Thread one running\n");	
	
	/** Do some work **/
	pthread_mutex_unlock(&second_mutex);
	pthread_mutex_unlock(&first_mutex);
	pthread_exit(0);
}

/* t2 runs in this function */
void *do_work_two(void *param)
{
	pthread_mutex_lock(&second_mutex);
	pthread_mutex_lock(&first_mutex);
	
	/** Do some work */
	fprintf(stderr, "Thread two running\n");	
	
	pthread_mutex_unlock(&first_mutex);
	pthread_mutex_unlock(&second_mutex);
}


int main()
{
	
	fprintf(stderr, "Starting...\n\n\n");	
	
	/* Initialize the mutex locks */
	pthread_mutex_init(&first_mutex, NULL);
	pthread_mutex_init(&second_mutex, NULL);
	/* Get the default attributes */
	pthread_attr_init(&attr);
	/* create the thread */
	pthread_create(&t1, &attr, do_work_one, NULL);
	pthread_create(&t2, &attr, do_work_two, NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);	
	
	fprintf(stderr, "Exiting...\n\n\n");	
		
	return 0;
}
