#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/wait.h>

#define MIN_NUMBER_OF_ARGS 2
#define CHILD 0
#define FORK_FAILURE -1
#define WAIT_FAILURE -1
#define PROCESS_FAILURE 0


int main(int argc, char **argv)
{
	if (argc < MIN_NUMBER_OF_ARGS)
	{
		fprintf(stderr, "Please, try like this:\n./a.out prog_name arg1 arg2 ...\n");
		exit(EXIT_FAILURE);
	}
	pid_t pid;
	pid = fork();
	if (pid == FORK_FAILURE)
	{
		perror("Making new process error");
		exit(EXIT_FAILURE);
	}

	if (pid == CHILD)
	{
		execvp(argv[1], &argv[1]);
		perror("Executing new prog error");
		exit(EXIT_FAILURE);
	}

	int status;
	pid = wait(&status);
	if (pid == WAIT_FAILURE)
	{
		perror("Waiting error");
		exit(EXIT_FAILURE);
	}

	if (WIFEXITED(status) == PROCESS_FAILURE)
	{
		fprintf(stderr, "Child process was not terminated normally.\n");
		exit(EXIT_FAILURE);
	}

	printf("Child process exit code: %d\n", WEXITSTATUS(status));

	exit(EXIT_SUCCESS);
}