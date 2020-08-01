#include <stdio.h> 
#include <pthread.h> 
#include <stdint.h> 
#include <stdlib.h> 
#include <unistd.h> 

#define STEPS 200000000


struct threadData{
	int threadRank;
	double partitialSum;
	int threadsCount;
};

void* calculatePi(void* data) {
	struct threadData* threaddata = (struct threadData*)data;
	int i;

	for (i = threaddata->threadRank; i < STEPS; i += threaddata->threadsCount) {
		threaddata->partitialSum += 1.0 / (i * 4.0 + 1.0);
		threaddata->partitialSum -= 1.0 / (i * 4.0 + 3.0);
	}
	pthread_exit(threaddata);
}



int main(int argc, char* argv[]) {
	int i;
	if (argc != 2) {
		fprintf(stderr, "Wrong number of arguments\n");
		exit(EXIT_FAILURE);
	}
	char errorFlag;
	int threadCount;
	if (sscanf(argv[1], "%d%c\n", &threadCount, &errorFlag) != 1) {
		fprintf(stderr, "Wrong threads count \n");
		exit(EXIT_FAILURE);
	}

	if (threadCount <= 0)
	{
		fprintf(stderr, "Wrong threads count \n");
		exit(EXIT_FAILURE);
	}
	double partialSums[threadCount];

	struct threadData* threadDatas = (struct threadData*)malloc(sizeof(struct threadData) * threadCount);
	pthread_t threads[threadCount];
	for (i = 00; i < threadCount; i++) {
		threadDatas[i].threadsCount = threadCount;
		threadDatas[i].threadRank = i;
		threadDatas[i].partitialSum = 0;
		if(pthread_create(&threads[i], NULL, calculatePi, &threadDatas[i]))
		{
			perror("Error while creating a thread");
			exit(EXIT_FAILURE);
		}
	}


	for (i = 0; i < threadCount; ++i) {
		if (pthread_join(threads[i], (void**)&threadDatas)) {
			perror("Error waiting thread");
			exit(EXIT_FAILURE);
		}

		partialSums[i] = threadDatas->partitialSum;
	}

	double pi = 0;

	for (i = 0; i < threadCount; ++i) {
		pi += partialSums[i];
	}
	pi *= 4;

	printf("PI: %.15f\n", pi);
	free(threadDatas);
	return 0;
}
