#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define RING '\a'

int count = 0;

void processSignal(int Signal)
{
	if (Signal == SIGQUIT)
	{
		printf("Count of DELETE tapping: %d\n", count);
		exit(EXIT_SUCCESS);
	}

	printf("%c\n", RING);
	count++;
}
int main()
{
	if (sigset(SIGINT, processSignal) == SIG_ERR)
	{
		perror("Signal SIGINT setting error");
		exit(EXIT_FAILURE);
	}
	if (signal(SIGQUIT, processSignal) == SIG_ERR)
	{
		perror("Signal SIGQUIT setting error");
		exit(EXIT_FAILURE);
	}
	for (;;);
}