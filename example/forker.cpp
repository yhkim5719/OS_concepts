#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


/**
 * The function to be executed by the grand child
 */
void grand_child()
{
	
	/** NOTE: getpid() returns the process id of the caller process **/
	fprintf(stderr, "Hi! I am the grandchild. My pid is %d\n", getpid());
	
	
	fprintf(stderr, "I am about to turn into a Firefox process!\n");
	
	/* Destroy the child code, and replace it with that of Firefox 
	 * executable.
	 */
	if(execlp("/usr/bin/firefox", "mutant", NULL) < 0)
	{
		perror("execlp");
		exit(-1);
	}
	
	/** IF EXECLP SUCCEEDED, THEN WE WILL NEVER GET HERE **/
	fprintf(stderr, "This line will never be executed\n");
}



/**
 * The function to be executed by the child
 */
void child()
{
	/* The process id of the grandchild */
	pid_t grand_child_pid;
	
		
	/** NOTE: getpid() returns the process id of the caller process **/
	fprintf(stderr, "Hi! I am the child. My pid is %d\n", getpid());
	
	/* Create a grandchild. If fork fails, then exit.
	 * Otherwise, save the grandchild's process id
	 */
	if((grand_child_pid = fork()) < 0)
	{
		perror("fork");
		exit(-1);
	}
	
	/* I am the grand child */
	if(grand_child_pid == 0)
	{
		/** NOTE: getpid() returns the process id of the caller process **/
		fprintf(stderr, "Hi! I am the grand child. My pid is %d\n", getpid());
		
		/* Call the grand child function */	
		grand_child();
	}
	/* I am the process that created the grand child */
	else
	{
		/* Wait for the grand child to exit */
		if(wait(NULL) < 0)
		{
			perror("wait");
			exit(-1);
		}	
	}
	
}

int main(int argc, char** argv)
{
	/* The process id of the child */
	pid_t child_pid;
	
	/* If the fork failed, then terminate. Otherwise, save the  
	 * value returned by fork(). 
	 */
	if((child_pid = fork()) < 0)
	{
		perror("fork");
		exit(-1);
	}
	
	/* Are we a child process ? */
	if(child_pid == 0)
	{
		/* Call the child function */
		child();
	}
	/* We are the parent */
	else
	{
		fprintf(stderr, "Hi! I am a parent. Just created a child with pid %d\n", child_pid);
		
		/* Wait for the child to exit */
		if(wait(NULL) < 0)
		{
			perror("wait");
			exit(-1);
		}	
	
	}	
	return 0;
}
