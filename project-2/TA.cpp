

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

/*TODO
 
//Declaration of Semaphores and Mutex Lock.
//Semaphores used:
//A semaphore to signal and wait TA's sleep.
//An array of 3 semaphores to signal and wait chair to wait for the TA.
//A semaphore to signal and wait for TA's next student.
 
 //Mutex Lock used:
//To lock and unlock variable ChairsCount to increment and decrement its value.
 
 //hint: use sem_t and pthread_mutex_t 
 */
// declaration semaphore TA
sem_t sem_TA;
// declaration semaphore student
sem_t sem_student;
// declaration semaphore chairs
sem_t sem_chairs[3];
// declaration mutex Lock TA chair and hallway chairs
pthread_mutex_t chair;

//Declared Functions
void *TA_Activity(void*);
void *Student_Activity(void *threadID);

int main(int argc, char* argv[])
{
	int number_of_students;		//a variable taken from the user to create student threads.	Default is 5 student threads.
	int id;
	srand(time(NULL));
	
    	//TODO
	//Initializing Mutex Lock and Semaphores.
	sem_init(&sem_TA, 0, 0);
	printf("sem_TA initiated.\n");		//TODO test
	
	sem_init(&sem_student, 0, 0);
	printf("sem_student initiated.\n");		//TODO test
	for (int i = 0; i < 3; i++) {	
		sem_init(&sem_chairs[i], 0, 0);
		printf("sem_chairs[%d] initiated with value.\n", i);		//TODO test
	}

	pthread_mutex_init(&chair, NULL);
	printf("mutex \"chair\" initiated.\n");		//TODO test
	
	//hint: use sem_init() and pthread_mutex_init()
	////TODO done
	
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
		
	//Allocate memory for Students
	Students = (pthread_t*) malloc(sizeof(pthread_t)*number_of_students);

	//TODO
	//Creating one TA thread and N Student threads.
		//hint: use pthread_create
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&TA, &attr, TA_Activity, NULL);
//	pthread_create(&TA, NULL, TA_Activity, NULL);
	printf("TA thread created.\n");		//TODO test

	for (int i = 0; i < number_of_students; i++) {
		pthread_create(&Students[i], &attr, Student_Activity, (void*) (long)i);
//		pthread_create(&Students[i], NULL, Student_Activity, NULL);
		printf("Student thread %d created.\n", i);		//TODO test

	}

	//Waiting for TA thread and N Student threads.
		//hint: use pthread_join
	pthread_join(TA, NULL);
	printf("TA joined.\n");		//TODO test

	for (int i = 0; i < number_of_students; i++) {
		pthread_join(Students[i], NULL);
		printf("Student thread %d joined.\n", i);		//TODO test
	}
	//TODO done

	//Free allocated memory
	free(Students); 
	return 0;
}

void *TA_Activity(void*)
{
	//TODO
	while (1) {
		//TA is currently sleeping.
		sem_wait(&sem_TA);
		printf("A student comes in and wakes the TA.\n");
		while (1) {
			//lock
			pthread_mutex_lock(&chair);
			//if chairs are empty, break the loop.
			if (ChairsCount == 0) {
				pthread_mutex_unlock(&chair);
				printf("If chairs are empty, break the loop.\n");
				break;
			}
			//TA gets next student on chair.
			sem_post(&sem_chairs[CurrentIndex]);
			ChairsCount--;
			printf("A student leaves a hall way chair, then goes into TA's room. # of occupied chair = %d.\n", ChairsCount);
			CurrentIndex = (CurrentIndex + 1) % 3;
			//unlock TA's chair
			pthread_mutex_unlock(&chair);
			
			//TA is currently helping the student
			printf("TA is currently helping the student.\n");
			sleep(5);
			sem_post(&sem_student);
			usleep(100);
		}
	}
	//TODO done
}

void *Student_Activity(void *threadID) 
{
	//TODO
	int StudyingTime;
	printf("I am student %ld.\n", (long)threadID);

	while (1) {
		//Student  needs help from the TA
		printf("I(%ld) am studying.\n", (long)threadID);
		StudyingTime = rand() % 10;
		sleep(StudyingTime);

		printf("I(%ld) need TA's help.\n", (long)threadID);
		
		pthread_mutex_lock(&chair);
		int cc = ChairsCount;
		pthread_mutex_unlock(&chair);
		//Student tried to sit on a chair.
		//if student sits, wake up the TA.
		if (cc < 3) {
			if (cc == 0) {
				sem_post(&sem_TA);
//				printf("A student wakes up the TA. # of occupied chairs = %d\n", cc);		//TODO test
			} else {
				printf("I(%ld) am sitting on a hallway chair.\n", (long)threadID);
			}	
			// lock
			pthread_mutex_lock(&chair);
			int id = (CurrentIndex + ChairsCount) % 3;
			ChairsCount++;
			printf("Remaining chair: %d\n", 3 - ChairsCount);
			// unlock
			pthread_mutex_unlock(&chair);
			//Student leaves his/her chair.
			sem_wait(&sem_chairs[id]);
			printf("TA is helping me(%ld)!!!\n", (long)threadID);

			//Student  is getting help from the TA
			
			//Student waits to go next.
			sem_wait(&sem_student);
			//Student left TA room
			printf("I(%ld) am  leaving the TA's chair.\n", (long)threadID);
		} else {
			//If student didn't find any chair to sit on.Student will return at another time
			printf("Student %ld will return at anothre time.\n", (long)threadID);
		}
	//hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()
	}
	//TODO done
}
