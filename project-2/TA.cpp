

#include <pthread.h>		//Create POSIX threads.
#include <time.h>			//Wait for a random time.
#include <unistd.h>			//Thread calls sleep for specified number of seconds.
#include <semaphore.h>		//To create semaphores
#include <stdlib.h>			
#include <stdio.h>			//Input Output

pthread_t *Students;		//N threads running as Students.
pthread_t TA;				//Separate Thread for TA.

int ChairsCount = 0;
int CurrentIndex = 0;
 
sem_t sem_TA;
sem_t sem_student;
sem_t sem_chairs[3];
pthread_mutex_t chair;

void *TA_Activity(void*);
void *Student_Activity(void *threadID);

int main(int argc, char* argv[])
{
	int number_of_students;		//a variable taken from the user to create student threads.	Default is 5 student threads.
	int id;
	srand(time(NULL));
	
	sem_init(&sem_TA, 0, 0);
	printf("sem_TA is initiated.\n");		//TODO test
	
	sem_init(&sem_student, 0, 0);
	printf("sem_student is initiated.\n");		//TODO test
	for (int i = 0; i < 3; i++) {	
		sem_init(&sem_chairs[i], 0, 0);
		printf("sem_chairs[%d] is initiated.\n", i);		//TODO test
	}

	pthread_mutex_init(&chair, NULL);
	printf("mutex \"chair\" initiated.\n");		//TODO test
	
	if(argc<2)
	{
		printf("Number of Students not specified. Using default (5) students.\n");
		number_of_students = 5;
	}
	else
	{
		printf("Number of Students specified. Creating %d threads.\n", number_of_students);
		number_of_students = atoi(argv[1]);
	}
		
	Students = (pthread_t*) malloc(sizeof(pthread_t)*number_of_students);

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&TA, &attr, TA_Activity, NULL);
	printf("TA starts working.\n");		//TODO test

	for (int i = 0; i < number_of_students; i++) {
		pthread_create(&Students[i], &attr, Student_Activity, (void*) (long)i);
		printf("Student %d starts studying.\n", i);		//TODO test

	}

	pthread_join(TA, NULL);
	printf("TA finished today's job.\n");		//TODO test

	for (int i = 0; i < number_of_students; i++) {
		pthread_join(Students[i], NULL);
		printf("Student %d finished studying.\n", i);		//TODO test
	}

	free(Students); 
	return 0;
}

void *TA_Activity(void*)
{
	while (1) {
		sem_wait(&sem_TA);
		printf("\tTA is sleeping now.\n");
		while (1) {
			pthread_mutex_lock(&chair);
			if (ChairsCount == 0) {
				pthread_mutex_unlock(&chair);
				printf("\tDone! I(TA) can sleep!!\n");
				break;
			}
			sem_post(&sem_chairs[CurrentIndex]);
			ChairsCount--;
			printf("\tStill %d students waiting.\n", ChairsCount);
			CurrentIndex = (CurrentIndex + 1) % 3;
			pthread_mutex_unlock(&chair);
			
			printf("\tI(TA) am busy helping the student.\n");
			sem_post(&sem_student);
		}
	}
}

void *Student_Activity(void *threadID) 
{
	int StudyingTime;
	printf("I am student %ld.\n", (long)threadID);

	while (1) {
		printf("I(%ld) am studying.\n", (long)threadID);
		StudyingTime = rand() % 10;
		sleep(StudyingTime);

		printf("I(%ld) need TA's help.\n", (long)threadID);
		
		pthread_mutex_lock(&chair);
		int cc = ChairsCount;
		pthread_mutex_unlock(&chair);
		if (cc < 3) {
			if (cc == 0) {
				sem_post(&sem_TA);
				printf("TA is sleeping. I gotta wake up TA!\n");		//TODO test
			} else {
				printf("I(%ld) am sitting on a hallway chair.\n", (long)threadID);
			}	
			pthread_mutex_lock(&chair);
			int id = (CurrentIndex + ChairsCount) % 3;
			ChairsCount++;
			pthread_mutex_unlock(&chair);
			sem_wait(&sem_chairs[id]);
			printf("TA is helping me(%ld)!!!\n", (long)threadID);
			sem_wait(&sem_student);
			printf("I(%ld) am done with this TA.\n", (long)threadID);
		} else {
			printf("Student %ld will return at anothre time.\n", (long)threadID);
		}
	}
}
