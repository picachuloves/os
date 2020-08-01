#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void *exitfunc(void *param)
{
	printf("\nStop\n");
}

void *printLines(void *param) {
	pthread_cleanup_push(exitfunc, NULL);
	while(1){
		write(STDOUT_FILENO,"BZZZZZZZ ", 10);
   	}
	pthread_cleanup_pop(0);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t thread;
	if (pthread_create(&thread, NULL, printLines, NULL)) {
		perror("Creating thread failure");
		exit(EXIT_FAILURE);
	}

	sleep(2);

	if (pthread_cancel(thread)) {
		perror("Canceling thread failure");
		exit(EXIT_FAILURE);
	}
	if (pthread_join(thread, NULL)) {
        	perror("Joining thread failure");
        	exit(EXIT_FAILURE);
    	}

	pthread_exit(NULL);

}
