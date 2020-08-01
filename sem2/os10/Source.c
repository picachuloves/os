#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define MUTEX_COUNT 3

pthread_mutex_t mutexes[MUTEX_COUNT];

void destroyMutexes(int count) {
	int i;
	for (i = 0; i < count; ++i) {
		if (pthread_mutex_destroy(&mutexes[i]) < 0) {
			perror("Error destroying mutex");
			exit(EXIT_FAILURE);
		}
	}
}

void exitFunc(char* errorMsg) {
	destroyMutexes(MUTEX_COUNT);
	perror(errorMsg);
	exit(EXIT_FAILURE);
}

void lockMutex(int num) {
	if (pthread_mutex_lock(&mutexes[num])) {
		exitFunc("Error locking mutex");
	}
}

void unlockMutex(int num) {
	if (pthread_mutex_unlock(&mutexes[num])) {
		exitFunc("Error unlocking mutex");
	}
}

void initMutexes() {
	int i;
	for (i = 0; i < MUTEX_COUNT; ++i) {
		if (pthread_mutex_init(&mutexes[i], NULL)) {
			destroyMutexes(i);
			perror("Error creating mutex");
			exit(EXIT_FAILURE);
		}
	}
}

void* secondPrint(void* data) {
	lockMutex(2);
	int i;
	for (i = 0; i < 10; ++i) {
		lockMutex((2 * i + 1) % 3);
		if (printf("Second: %d\n", i) < 0) {
			exitFunc("Error printing : 2 tread");
		}
		unlockMutex((2 * i + 2) % 3);
	}

	unlockMutex(2);
	return NULL;
}

void firstPrint() {
	int i;
	for (i = 0; i < 10; ++i) {
		lockMutex((2 * i) % 3);
		if (printf("First: %d\n", i) < 0) {
			exitFunc("Error printing : 1 tread");
		}
		unlockMutex((2 * i + 1) % 3);
	}
	unlockMutex(1);
}



int main() {
	pthread_t thread;

	initMutexes();
	lockMutex(1);

	if (pthread_create(&thread, NULL, secondPrint, NULL)) {
		exitFunc("Error creating thread");
	}

	if (sleep(1)) {
		exitFunc("Sleep was interrupted");
	}

	firstPrint();

	if (pthread_join(thread, NULL)) {
		exitFunc("Error waiting thread");
	}

	destroyMutexes(MUTEX_COUNT);
	return EXIT_SUCCESS;
}

