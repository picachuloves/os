#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>

#define INIT_BUFFER_SIZE 256
#define BUFFER_INCREASER 2
#define ALLOCATION_FAILURE NULL
#define END_OF_INPUT '.'
#define END_OF_STRING '\n'

typedef struct List
{
	char* string;
	struct List *next;
} mylist;
mylist *head;

typedef struct Buffer 
{
	char *stirng;
	unsigned long long currentPos;
	unsigned long long size;
} Buffer;

void addSymb(Buffer *buffer, char symb) 
{
	if (buffer->currentPos == buffer->size)
	{
		buffer->size *= BUFFER_INCREASER;
		buffer->stirng = (char*)realloc(buffer->stirng, buffer->size*sizeof(char));
		if (buffer->stirng == ALLOCATION_FAILURE)
		{
			perror("Error of increasing buffer");
			exit(EXIT_FAILURE);
		}
	}
	buffer->stirng[buffer->currentPos] = symb;
	buffer->currentPos++;
}

void push(Buffer *buffer) 
{
	mylist *ptr = head;

	mylist *newnode = (mylist*)malloc(sizeof(mylist));
	if (newnode == NULL)
	{
		perror("Allocation for node failed");
		exit(EXIT_FAILURE);
	}
	newnode->string = (char*)malloc(buffer->size * sizeof(char));
	if (newnode->string == NULL)
	{
		perror("Allocation for string failed");
		exit(EXIT_FAILURE);
	}
	strcpy(newnode->string, buffer->stirng);
	newnode->next = NULL;
	if (!head)
	{
		head = newnode;
	}
	else
	{
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
		}
		ptr->next = newnode;
	}
}

void clearlist() 
{
	mylist *ptr = head;
	while ( head!= NULL) 
	{
		head = head->next;
		free(ptr->string);
		free(ptr);
		ptr = head;
	}
}

void printlist() 
{
	printf("Strings:\n");
	mylist *ptr;
	for (ptr = head; ptr != NULL; ptr = ptr->next)
	{
		if (ptr->string[0] != END_OF_INPUT)
		{
			printf("%s", ptr->string);
		}
	}
}

void getString(Buffer *buffer) 
{
	char currentSymbol = '\0';
	while (currentSymbol != END_OF_STRING) 
	{
		currentSymbol = fgetc(stdin);
		if (currentSymbol == EOF)
		{
			perror("Error of reading string");
			exit(EXIT_FAILURE);
		}
		addSymb(buffer, currentSymbol);
	}
	addSymb(buffer, '\0');
}

void initBuffer(Buffer *buffer) 
{
	buffer->stirng = (char*)malloc(INIT_BUFFER_SIZE);
	if (buffer->stirng == ALLOCATION_FAILURE) 
	{
		perror("Error of buffer allocation");
		exit(EXIT_FAILURE);
	}
	buffer->currentPos = 0;
	buffer->size = INIT_BUFFER_SIZE;
}

int main(int argc, char *argv[]) 
{
	Buffer buffer;
	initBuffer(&buffer);

	printf("Start.\n");

	do
	{
		getString(&buffer);
		push(&buffer);
		buffer.currentPos = 0;
	} while (buffer.stirng[0] != END_OF_INPUT);

	printlist();
	clearlist();
	free(buffer.stirng);
	return EXIT_SUCCESS;
}