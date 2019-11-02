/**
 * This program illustrates how basic pipes work.
 * The parent creates a pipe, forks off a child, and
 * sends the child a string "Hello World"
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

/* The array of pipe file descriptors */
int fd[2];

/**
 * The function to be called by the parent
 */
void parent()
{
	/* Parent will not be reading; close the read 
 	 * end of the pipe.
 	 */
	close(fd[0]);
	
	/* Send "Hello world" to the child */
	if(write(fd[1], "Hello world", 11) < 0)
	{
		perror("write");
		exit(-1);
	}
	
	/* We are done writing. No longer need the write end */
	close(fd[1]);
}

/**
 * The function called by the child
 */
void child()
{
	/* The message buffer */
	char msgBuff[100];
	
	/* Fill the buffer with 0's */
	memset(msgBuff, 0, sizeof(msgBuff));
	
	/* Child will not be writing; close the write
 	 * end of the pipe
 	 */
	close(fd[1]);
	
	/* Read 100 bytes or until the other process closes it's write end */
	if(read(fd[0], msgBuff, sizeof(msgBuff)) < 0)
	{
		perror("read");
		exit(-1);
	} 
	
	/* Print what you read */
	fprintf(stderr, "Got string: %s from the parent\n", msgBuff);
		
	/* We are done reading. Close the read end */
	close(fd[0]);
}

int main()
{	
	/* Create a pipe */
	if(pipe(fd) < 0)
	{
		perror("pipe");
		exit(-1);
	}
	
	/* Create a child */
	pid_t pid = fork();
	
	/* Fork failed */
	if(pid < 0)
	{
		perror("fork");
		exit(-1);
	}
	/* We are the parent */
	else if(pid > 0) 
	{ 
		/* Do parent stuff */
		parent(); 
		
		/* Wait for the child */
		if(wait(NULL) < 0)
		{
			perror("wait");
			exit(-1);
		}
	}
	
	/* We are a child */
	else { child();  }	

	
	return 0;	
}
