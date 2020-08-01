#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

void * printLine(void *param)
{
	int iter;
	for (iter = 0; iter < 10; iter++) 
	{
		printf("Child: %d\n", iter);
	}
}

int main(int argc, char *argv[]) 
{
	pthread_t thread;
	int code;
	code = pthread_create(&thread, NULL, printLine, NULL);
	if (code != 0)
	{
		char errorname[256];
		strerror_r(code, errorname, 256);
		fprintf(stderr, "%s: creating thread: %s\n", argv[0], errorname);
		exit(EXIT_FAILURE);
	}
	pthread_join(thread, NULL);

	int iter;
	for (iter = 0; iter < 10; iter++)
	{
		printf("Parent: %d\n", iter);
	}
	
	return 0;
}
