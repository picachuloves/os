#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

pthread_mutex_t mutex;
pthread_cond_t cond;

void destroy(){
	if(pthread_mutex_destroy(&mutex)){
		perror("Error destroying mutex");
		exit(EXIT_FAILURE);
	}
	if(pthread_cond_destroy(&cond)){
		perror("Error destroying cond");
		exit(EXIT_FAILURE);
	}
}

void exitFunc(char* errorMsg) {
	destroy();
	perror(errorMsg);
	exit(EXIT_FAILURE);
}

void init(){
	if(pthread_mutex_init(&mutex,NULL)){
		perror("Error initing mutex");
		exit(EXIT_FAILURE);
	}
    	if(pthread_cond_init(&cond,NULL)){
		perror("Error initing cond");
		exit(EXIT_FAILURE);
	}
}

void *printLines(void *arg){
	if(pthread_mutex_lock(&mutex)){
		exitFunc("Error locking mutex");
	}
	char* msg = (char*)malloc(sizeof(char)*10);
	if((int)arg==1){
		strcpy(msg, "First:");
	}
	else {
		strcpy(msg, "Second:");
		pthread_cond_signal(&cond);
	}    
	int i=0;
	
   	for(i=0;i<10;i++){
        	pthread_cond_wait(&cond,&mutex);
		printf("%s %d\n",msg,i);
		pthread_cond_signal(&cond);
    	}
	pthread_mutex_unlock(&mutex);
   	free(msg);
    	pthread_exit(0);
}


int main (int argc, char** argv)
{
    pthread_t pthread;
    
    init();
    
    if(pthread_create(&pthread,NULL,printLines,(void*)2)){
		exitFunc("Error creating thread");
	}
    
    printLines((void*)1);
    
    destroy();
    
    pthread_exit(0);
}
