#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <semaphore.h>

#define PTHREAD_SUCCESS 0
#define LINES_COUNT 10
#define SEMAPHORE_COUNT 2
#define PARENT_INDEX 0
#define CHILD_INDEX 1

sem_t semaphore[SEMAPHORE_COUNT];

char* stringForParentThread = "Parent string\n";
char* stringForChildThread = "Child string\n";

int init_semaphores() {
	int i = 0, result;
	for(; i < SEMAPHORE_COUNT; i++) {
		result = sem_init(&semaphore[i], 0, 1 - i);
		if(result != PTHREAD_SUCCESS) return errno;
	}
	return PTHREAD_SUCCESS;
}

int destroy_semaphores() {
	int i = 0, result;
	for(; i < SEMAPHORE_COUNT; i++) {
		result = sem_destroy(&semaphore[i]);
		if(result != PTHREAD_SUCCESS) return errno;
	}
	return PTHREAD_SUCCESS;
}

int print_lines(char* line, int count, int thread_index) {
	int i = 0, result;
	for(; i < count; i++) {
		result = sem_wait(&semaphore[thread_index]);
		if(result != PTHREAD_SUCCESS) return errno;
		printf("%s", line);
		result = sem_post(&semaphore[1 - thread_index]);
		if(result != PTHREAD_SUCCESS) return errno;
	}
	return PTHREAD_SUCCESS;
}

void* thread_control(void* arg) {
	int result = print_lines(stringForChildThread, LINES_COUNT, CHILD_INDEX);
	if(result != PTHREAD_SUCCESS) {
		errno = result;
		perror("Error in child thread");
		exit(EXIT_FAILURE);
	}
	return NULL;
}

int main() {
	int result;
	pthread_t child;
	init_semaphores();
	result = pthread_create(&child, NULL, thread_control, NULL);
	if(result != PTHREAD_SUCCESS) {
		errno = result;
		perror("Error while creating a thread");
		return EXIT_FAILURE;
	}
	result = print_lines(stringForParentThread, LINES_COUNT, PARENT_INDEX);
	if(result != PTHREAD_SUCCESS) {
		errno = result;
		perror("Error in parent thread");
		return EXIT_FAILURE;
	}
	destroy_semaphores();
	pthread_exit(NULL);
}
