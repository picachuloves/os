#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

#define PHILS_COUNT 5
#define FOOD_COUNT 10
#define DELAY_LENGTH 500000
#define PTHREAD_SUCCESS 0

pthread_t philosophers[PHILS_COUNT];
pthread_mutex_t forks[PHILS_COUNT];
pthread_mutex_t food_lock;

int food = 0;

void* phil_control(void* num) {
	int id;
	int first_fork, second_fork;
	id = (int)num;
	first_fork = id;
	second_fork = (id + 1) % PHILS_COUNT;
	
	while(food < FOOD_COUNT) {
		usleep(DELAY_LENGTH); //thinking
		
		pthread_mutex_lock(&food_lock);
		if(food >= FOOD_COUNT) {
			pthread_mutex_unlock(&food_lock);
			break;
		}
		printf("Philosopher #%d: got dish #%d\n", id, food);
		food++;
		pthread_mutex_unlock(&food_lock);
		
		pthread_mutex_lock(forks + first_fork);
		printf("Philosopher #%d: got fork #%d\n", id, first_fork);
		pthread_mutex_lock(forks + second_fork);
		printf("Philosopher #%d: got fork #%d\n", id, second_fork);
		usleep(DELAY_LENGTH);
		pthread_mutex_unlock(forks + second_fork);
		pthread_mutex_unlock(forks + first_fork);
	}
	printf("Philosopher #%d: done eating\n");
}

int main() {
	int i, result;
	pthread_mutex_init (&food_lock, NULL);
	for(i = 0; i < PHILS_COUNT; i++) {
		result = pthread_mutex_init(&forks[i], NULL);
		if(result != PTHREAD_SUCCESS) {
			errno = result;
			perror("Error initing mutex");
			return EXIT_FAILURE;
		}
	}
	for(i = 0; i < PHILS_COUNT; i++) {
		result = pthread_create(&philosophers[i], NULL, phil_control, (void *)i);
		if(result != PTHREAD_SUCCESS) {
			errno = result;
			perror("Error creating thread");
			return EXIT_FAILURE;
		}
	}
	for(i = 0; i < PHILS_COUNT; i++) {
		result = pthread_join(philosophers[i], NULL);
		if(result != PTHREAD_SUCCESS) {
			errno = result;
			perror("Error joining thread");
			return EXIT_FAILURE;
		}
	}
	for(i = 0; i < PHILS_COUNT; i++) {
		result = pthread_mutex_destroy(&forks[i]);
		if(result != PTHREAD_SUCCESS) {
			errno = result;
			perror("Error destroying mutex");
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}