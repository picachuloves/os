#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <time.h>
#include <unistd.h>

#define P2OPEN_FAILURE -1
#define TIME_FAILURE -1
#define PCLOSE_FAILURE -1
#define FCLOSE_FAILURE -1
#define P2CLOSE_FAILURE -1
#define READ 1
#define WRITE 0
#define MAX_VALUE 100
#define NUMS_COUNT 100
#define NUMS_IN_LINE 10
#define FPRINTF_FAILURE 0

void sendNums(FILE* output)
{
	int randNum = 0, printedChars = 0, i;
	for (i = 0; i < NUMS_COUNT; i++)
	{
		randNum = rand() % MAX_VALUE;
		printedChars = fprintf(output, "%d\n", randNum);
		if (printedChars < FPRINTF_FAILURE)
		{
			perror("sending error");
			exit(EXIT_FAILURE);
		}
	}
}

void receiveNums(FILE* input)
{
	int num = 0, i, j;
	for (i = 0; i < NUMS_COUNT; i += NUMS_IN_LINE)
	{
		for (j = 0; j < NUMS_IN_LINE; j++)
		{
			fscanf(input, "%d", &num);
			printf("%2d ", num);
		}
		printf("\n");
	}
}

int main(int argc, char* argv[])
{
	FILE* pipes[2];

	if (p2open("/bin/sort -n", pipes) == P2OPEN_FAILURE)
	{
		perror("p2open error");
		return EXIT_FAILURE;
	}

	time_t current_time = time(NULL);
	if (current_time == TIME_FAILURE)
	{
		perror("time error");
		if (p2close(pipes) == P2CLOSE_FAILURE)
		{
			perror("p2close error");
		}
		return EXIT_FAILURE;
	}
	srand(current_time);

	sendNums(pipes[WRITE]);

	if (fclose(pipes[WRITE]) == FCLOSE_FAILURE)
	{
		perror("fclose error");
		return EXIT_FAILURE;
	}

	receiveNums(pipes[READ]);

	if (pclose(pipes[READ]) == PCLOSE_FAILURE)
	{
		perror("pclose error");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

