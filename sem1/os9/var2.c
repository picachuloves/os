#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#define MSGSIZE 256
#define PIPE_FAILURE -1
#define FORK_FAILURE -1
#define CHILD 0
#define READ 0 
#define WRITE 1
#define WRITE_FAILURE -1
#define READ_FAILURE -1
#define SEND_FAILURE -1
#define SEND_SUCCESS 0
#define RECIEVE_SUCCESS 0
#define RECIEVE_FAILURE -1
#define END_OF_MSG '\n'
#define CHILD_PROC_SUCCESS 0
#define CHILD_PROC_FAILURE -1
#define WAIT_FAILURE -1
#define CLOSE_FAILURE -1
#define CLOSING_FAILURE -1
#define CLOSING_SUCCESS 0
#define MSG "HelLow, IT's me, abcdefg!"

int sendmsg(int fileDescs[])
{
	pid_t pid = fork();
	if (pid == FORK_FAILURE)
	{
		perror("Making new process error");
		return SEND_FAILURE;
	}
	int childProcCode;
	if (pid == CHILD)
	{
		printf("Your msg: \n");

		if (close(fileDescs[READ]) == CLOSE_FAILURE)
		{
			perror("Closing filedesc error");
			childProcCode = CHILD_PROC_FAILURE;
			exit(EXIT_FAILURE);
		}

		if (write(fileDescs[WRITE], MSG, sizeof(char) * MSGSIZE) == WRITE_FAILURE)
		{
			perror("Sending msg error");
			childProcCode = CHILD_PROC_FAILURE;
			exit(EXIT_FAILURE);
		}

		if (close(fileDescs[WRITE]) == CLOSE_FAILURE)
		{
			perror("Closing filedesc error");
			childProcCode = CHILD_PROC_FAILURE;
			exit(EXIT_FAILURE);
		}
		childProcCode = CHILD_PROC_SUCCESS;
		exit(EXIT_SUCCESS);
	}
	if (childProcCode == CHILD_PROC_FAILURE)
	{
		return SEND_FAILURE;
	}
	return SEND_SUCCESS;
	
}

int recievemsg(int fileDescs[])
{
	pid_t pid = fork();
	if (pid == FORK_FAILURE)
	{
		perror("Making new process error");
		return RECIEVE_FAILURE;
	}
	int childProcCode;
	if (pid == CHILD)
	{
		printf("Your msg: \n");

		ssize_t code;
		char buffer;
		if (close(fileDescs[WRITE]) == CLOSE_FAILURE)
		{
			perror("Closing filedesc error");
			childProcCode = CHILD_PROC_FAILURE;
			exit(EXIT_FAILURE);
		}

		char msg[MSGSIZE];
		int msgindex = 0;
		while ((code = read(fileDescs[READ], &buffer, sizeof(char))) > 0) 
		{
			buffer = (char)toupper(buffer);
			msg[msgindex] = buffer;
			msgindex++;
		}

		printf("%s\n", msg);

		if (code == READ_FAILURE)
		{
			perror("Reading msg error");
			childProcCode = CHILD_PROC_FAILURE;
			exit(EXIT_FAILURE);
		}

		if (close(fileDescs[READ]) == CLOSE_FAILURE)
		{
			perror("Closing filedesc error");
			childProcCode = CHILD_PROC_FAILURE;
			exit(EXIT_FAILURE);
		}
		childProcCode = CHILD_PROC_SUCCESS;
		exit(EXIT_SUCCESS);
	}
	if (childProcCode == CHILD_PROC_FAILURE)
	{
		return RECIEVE_FAILURE;
	}
	return RECIEVE_SUCCESS;
}

int closeAll(int* fileDescs)
{
	if (close(fileDescs[READ]) == CLOSE_FAILURE)
	{
		perror("close failed");
		return CLOSING_FAILURE;
	}
	if (close(fileDescs[WRITE]) == CLOSE_FAILURE)
	{
		perror("close failed");
		return CLOSING_FAILURE;
	}
	return CLOSING_SUCCESS;
}


int main(int argc, char *argv[])
{
	int fileDescs[2];
	if (pipe(fileDescs) == PIPE_FAILURE) 
	{
		perror("Creating pipe error");
		return EXIT_FAILURE;
	}

	if (sendmsg(fileDescs) == SEND_FAILURE)
	{
		return EXIT_FAILURE;
	}

	if (recievemsg(fileDescs) == RECIEVE_FAILURE)
	{
		return EXIT_FAILURE;
	}

	if (closeAll(fileDescs) == CLOSING_FAILURE)
	{
		return EXIT_FAILURE;
	}

	int status;
	while (wait(&status) != WAIT_FAILURE) 
	{
		if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS || WIFSIGNALED(status)) 
		{
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}