#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void* printLines(void* data){
    while(1){
        printf("BZZZZZZZZ ");
    }
}

int main(){
    pthread_t thread;

    if (pthread_create(&thread, NULL, printLines, NULL)){
        perror("create error");
        exit(EXIT_FAILURE);
    }

    sleep(2);

    if (pthread_cancel(thread)){
        perror("cancel error");
        exit(EXIT_FAILURE);
    }
    return 0;
}

