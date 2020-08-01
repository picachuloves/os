#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

#define CHILD 0
#define FORK_FAILURE -1
#define CORRECT_NUM_OF_ARGS 2

int main(int argc, char **argv) 
{
	if (argc != CORRECT_NUM_OF_ARGS)
	{
		fprintf(stderr, "Please, try like this:\n./a.out file_name \n");
		exit(EXIT_FAILURE);
	}

	pid_t pid = fork();

	if (pid == FORK_FAILURE)
	{
		perror("Making new process error");
		exit(EXIT_FAILURE);
	}

	if (pid  == CHILD)
	{
		execl("/bin/cat", "cat", argv[1], (char*)0);
		perror("Executing cat error");
		exit(EXIT_FAILURE);
	}

	printf("PARENT PROCESS IS HERE.\n");
	exit(EXIT_SUCCESS);
}
