#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include "msg.h"    /* For the message struct */

using namespace std;

/* The size of the shared memory segment */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr = NULL;


/**
 * The function for receiving the name of the file
 * @return - the name of the file received from the sender
 */
string recvFileName()
{
	/* The file name received from the sender */
	string fileName;
        
	/* TODO: declare an instance of the fileNameMsg struct to be
	 * used for holding the message received from the sender.
         */
	fileNameMsg rcvFile;
	rcvFile.mtype = 3;

        /* TODO: Receive the file name using msgrcv() */
	if (msgrcv(msqid, &rcvFile, sizeof(rcvFile) - sizeof(long), 3, 0) < 0) {
		printf("in recv, line 41\n");
		perror("msgrcv");
		exit(-1);
	}

	/* TODO: return the received file name */
	fileName = rcvFile.fileName;

        return fileName;
}
 /**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */
void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	
//	FILE* f;
//	f = fopen("keyfile.txt", "w");
//	if (f != NULL) {
//		fprintf(f, "Hello world");
//	}

	key_t key = ftok("keyfile.txt", 'a');
	if (key == -1) {
		perror("ftok");
	}
	/* Allocate a shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE. */
	if ((shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, IPC_CREAT | 0666)) == -1) {
		perror("shmget");
	}

	/* Attach to the shared memory */
	sharedMemPtr = shmat(shmid, NULL, 0);

	/* Create a message queue */
	msqid = msgget(key, IPC_CREAT | 0666);
	
}
 

/**
 * The main loop
 * @param fileName - the name of the file received from the sender.
 * @return - the number of bytes received
 */
unsigned long mainLoop(const char* fileName)
{
	/* The size of the message received from the sender */
	int msgSize = -1;
	
	/* The number of bytes received */
	int numBytesRecv = 0;
	
	/* The string representing the file name received from the sender */
	string recvFileNameStr = fileName;
	
	/* append __recv to the end of file name */
	recvFileNameStr.append("__recv");

	/* Open the file for writing */
	FILE* fp = fopen(recvFileNameStr.c_str(), "w");
			
	/* Error checks */
	if(!fp)
	{
		perror("fopen");	
		exit(-1);
	}
		

	/* Keep receiving until the sender sets the size to 0, indicating that
 	 * there is no more data to send.
 	 */	
	message sndMsg;		
	sndMsg.mtype = 1;
	ackMessage rcvMsg;
	rcvMsg.mtype = 2;

	while(msgSize != 0)
	{	

		/* Receive the message and get the value of the size field. The message will be of 
		 * of type SENDER_DATA_TYPE. That is, a message that is an instance of the message struct with 
		 * mtype field set to SENDER_DATA_TYPE (the macro SENDER_DATA_TYPE is defined in 
		 * msg.h).  If the size field of the message is not 0, then we copy that many bytes from 
		 * the shared memory segment to the file. Otherwise, if 0, then we close the file 
		 * and exit.
		 */
		if (msgrcv(msqid, &sndMsg, sizeof(sndMsg) - sizeof(long), 1, 0) == -1) {	
			perror("msgrcv");
			exit(1);
		}
		msgSize = sndMsg.size;

		/* If the sender is not telling us that we are done, then get to work */
		
		if(msgSize != 0)
		{
			/* count the number of bytes received */
			numBytesRecv += msgSize;

			/* Save the shared memory to file */
			if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) < 0)
			{
				perror("fwrite");
			}
			
			/* Tell the sender that we are ready for the next set of bytes. 
 			 */
			if(msgsnd(msqid, &rcvMsg, sizeof(rcvMsg) - sizeof(long), 0) == -1) {
				perror("msgsnd");
				exit(1);
			}

		}
		/* We are done */
		else
		{
			/* Close the file */
			fclose(fp);
		}
	
	}
	
	return numBytesRecv;
}



/**
 * Performs cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */
void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* Detach from shared memory */
	if ( shmctl(shmid, IPC_RMID, 0) < 0 ) {
		perror("shmctl");
		exit(-1);
	}

	/* Deallocate the shared memory segment */
	if ( shmdt(sharedMemPtr) < 0 ) {
		perror("shmdt");
		exit(-1);
	}

	/* Deallocate the message queue */
	if ( msgctl(msqid, IPC_RMID, 0) < 0 ) {
		perror("msgctl");
		exit(-1);
	}
}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */
void ctrlCSignal(int signal)
{
	/* Free system V resources */
	fprintf(stderr, "cleaned the message queue and the shared memory segment.");
	cleanUp(shmid, msqid, sharedMemPtr);
}

int main(int argc, char** argv)
{
	
	/* Install a signal handler (see signaldemo.cpp sample file).
 	 * If user presses Ctrl-c, your program should delete the message
 	 * queue and the shared memory segment before exiting. You may add 
	 * the cleaning functionality in ctrlCSignal().
 	 */
	signal(SIGINT, ctrlCSignal);

	/* Initialize */
	init(shmid, msqid, sharedMemPtr);
	
	/* Receive the file name from the sender */
	string fileName = recvFileName();
	
	/* Go to the main loop */
	fprintf(stderr, "The number of bytes received is: %lu\n", mainLoop(fileName.c_str()));

	/* Detach from shared memory segment, and deallocate shared memory 
	 * and message queue (i.e. call cleanup) 
	 */
	cleanUp(shmid, msqid, sharedMemPtr);

	return 0;
}
