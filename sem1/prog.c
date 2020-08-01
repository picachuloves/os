#define _CRT_SECURE_NO_WARNINGS

#include <math.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

int main() {

int data = 0;

int q = 0;

typedef struct queue {

char *arr;

struct queue *next;

} queue;

queue *head = NULL;

queue *pi = NULL;

queue *pred = NULL;

queue *pdel = NULL;

char arr[1000][1000];

char *rstr;

printf("START : \n");

int length = 0;

int Mlength = 0;

int i = 0;

char el[10000];

for (i = 0; i < 10000; i++)

{

rstr = gets(el);

if ((rstr != NULL) && (rstr[0] != '.'))

{

length = strlen(rstr);

queue *tmp = (queue*)malloc(sizeof(queue));

tmp->arr = (char*)malloc(length + 1);

strcpy(tmp->arr, el);

tmp->next = NULL;

if (head == NULL)

{

head = tmp;

pred = head;

}

else

{

pred->next = tmp;

pred = tmp;

}

}

else

break;

}

printf("%\n");

pi = head;

while (pi != NULL)

{

pdel = pi;

printf("%s\n", pi->arr);

pi = pi->next;

free(pdel->arr);

free(pdel);

}

length = 0;

return 0;

}
