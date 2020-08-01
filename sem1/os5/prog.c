#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>

#define CORRECT_NUMBER_OF_ARGUMENTS 2
#define MAX_COUNT_OF_STRINGS 100
#define MAX_SIZE_OF_FILENAME 100
#define MAX_SIZE_OF_REQUEST 100
#define OPEN_FAILURE -1
#define CLOSE_FAILURE -1 
#define WRITE_FAILURE -1
#define READ_FAILURUE -1
#define LSEEK_FAILURE -1
#define READ_CONTINUES 0
#define END_OF_WORKING 0
#define CONTINUE_WORK 1
#define END_OF_STRING '\n'
#define PATTERN_OF_REQUEST "^[1-9][0-9]{0,}$"
#define PATTERN_OF_EXIT "^\s{0,}0"
#define REGEX_SUCCESS 0

typedef struct Table
{
	off_t offset;
	int length;
}Table;

void checkArgs(int argc, char* argv, int* fileDesc)
{
	char filename[MAX_SIZE_OF_FILENAME];
	if (argc != CORRECT_NUMBER_OF_ARGUMENTS)
	{
		printf("Print filename!\n");
		scanf("%s", filename);
		argv = filename;
	}
	*fileDesc = open(argv, O_RDONLY);
	while (*fileDesc == OPEN_FAILURE)
	{
		printf("Incorrect name of file!\nPlease, print the filename:\n");
		scanf("%s", filename);
		*fileDesc = open(filename, O_RDONLY);
	}
}

void makeTable(int *fileDesc, Table* table, int* currentString)
{
	char currentChar;
	int code;
	*currentString = 0;
	while (code = read(*fileDesc, &currentChar, sizeof(char)) > READ_CONTINUES)
	{
		if (currentChar != END_OF_STRING)
		{
			++table[*currentString].length;
			continue;
		}
		++*currentString;
		table[*currentString].offset = table[*currentString - 1].offset + table[*currentString - 1].length + 1;
		if (*currentString > MAX_COUNT_OF_STRINGS)
		{
			printf("File is too big. Prog got first 100 strings.\n");
			return;
		}
	}
	if (code == READ_FAILURUE)
	{
		perror("Reading file error");
		exit(EXIT_FAILURE);
	}
}

void printString(int *fileDesc, int stringNumb, Table* table)
{
	if (lseek(*fileDesc, table[stringNumb - 1].offset, SEEK_SET) == LSEEK_FAILURE)
	{
		perror("Reading file error");
		exit(EXIT_FAILURE);
	}
	char symb;
	int iter;
	for (iter = 0; iter < table[stringNumb - 1].length; iter++)
	{
		if (read(*fileDesc, &symb, sizeof(char)) == READ_FAILURUE)
		{
			perror("Reading file error");
			exit(EXIT_FAILURE);
		}
		if (write(1, &symb, sizeof(char)) == WRITE_FAILURE)
		{
			perror("Writing error");
			exit(EXIT_FAILURE);
		}
	}
	printf("\n");
}

int processRequest(int *fileDesc, Table* table, int stringsCount, char* request)
{
	regex_t regexexit, regexrequest;
	int code;
	code = regcomp(&regexexit, PATTERN_OF_EXIT, REG_EXTENDED);
	if (code != REGEX_SUCCESS) {
		perror("regex error");
		char error[512];
		regerror(code, &regexexit, error, sizeof(error));
		printf("%s", error);
		exit(EXIT_FAILURE);
	}
	code = regcomp(&regexrequest, PATTERN_OF_REQUEST, REG_EXTENDED);
	if (code != REGEX_SUCCESS) {
		perror("regex error");
		char error[512];
		regerror(code, &regexrequest, error, sizeof(error));
		printf("%s", error);
		exit(EXIT_FAILURE);
	}

	regmatch_t pm;
	code = regexec(&regexexit, request, 0, &pm, 0);
	if (code == REGEX_SUCCESS)
	{
		return END_OF_WORKING;
	}
	code = regexec(&regexrequest, request, 0, &pm, 0);
	if (code == REGEX_SUCCESS)
	{
		int stringNumb = atoi(request);
		if (stringNumb > stringsCount)
		{
			printf("File has only %d strings!\n", stringsCount);
			printf("Try again\n");
			return CONTINUE_WORK;
		}
		printString(fileDesc, stringNumb, table);
		return CONTINUE_WORK;
	}
	printf("Incorrect request!\n");
	return CONTINUE_WORK;
}

int main(int argc, char *argv[]) 
{
	int fileDesc;
	checkArgs(argc, argv[1], &fileDesc);

	Table table[MAX_COUNT_OF_STRINGS] = { 0, 0 };
	int stringsCount;
	makeTable(&fileDesc, table, &stringsCount);
	
	char request[MAX_SIZE_OF_REQUEST];
	int code = 1;
	while (code != END_OF_WORKING)
	{
		printf("Print the number of string(0 for exit)\n");
		scanf("%s", request);
		code = processRequest(&fileDesc, table, stringsCount, request);
	} 

	if (close(fileDesc) == CLOSE_FAILURE)
	{
		perror("Closing error");
		exit(EXIT_FAILURE);
	}
	return 0;
}