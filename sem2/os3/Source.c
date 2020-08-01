#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

char array[4][15] = { "First Line", "Second Line", "Third Line", "Fourth Line" };

void *printLines(void *param) {
	printf("%s\n", (char*)param);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t thread;
	int code;
	int i;
	for (i = 0; i < 4; i++) {
		code = pthread_create(&thread, NULL, printLines, (void*)array[i]);
		if (code != 0)
		{
			char errorname[256];
			strerror_r(code, errorname, 256);
			fprintf(stderr, "%s: creating thread: %s\n", argv[0], errorname);
			exit(EXIT_FAILURE);
		}
	}
	pthread_exit(NULL);

}