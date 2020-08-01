#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>
#include <signal.h>
#include <string.h>

#define CORRECT_NUMBER_OF_ARGUMENTS 2
#define MAX_COUNT_OF_STRINGS 100
#define MAX_SIZE_OF_FILENAME BUFSIZ
#define MAX_SIZE_OF_REQUEST BUFSIZ
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
#define PATTERN_OF_EXIT "^\s{0,}0\0"
#define REGEX_SUCCESS 0
#define PRINT_STRING_ERROR 0
#define PRINT_STRING_SUCCESS 1
#define PROCESS_REQUEST_FAILURE -1
#define MAKE_TABLE_FAILURE 0
#define MAKE_TABLE_SUCCESS 1
#define TIME_WAIT 5
#define ALARM 1
#define EMPTY_STRING 1

typedef struct Table
{
	off_t offset;
	int length;
}Table;

int Signal = NULL;

void checkArgs(int argc, char* argv, int* fileDesc)
{
	char filename[MAX_SIZE_OF_FILENAME];
	if (argc != CORRECT_NUMBER_OF_ARGUMENTS)
	{
		printf("Please, print the filename:\n");
		scanf("%s", filename);
		argv = filename;
	}
	*fileDesc = open(argv, O_RDONLY);
	while (*fileDesc == OPEN_FAILURE)
	{
		perror("Incorrect name of file");
		printf("Please, print the filename:\n");
		scanf("%s", filename);
		*fileDesc = open(filename, O_RDONLY);
	}
}

int makeTable(int *fileDesc, Table* table, int* tableIndex)
{
	char currentChar;
	int code;

	*tableIndex = 0;
	while (code = read(*fileDesc, &currentChar, sizeof(char)) > READ_CONTINUES)
	{
		if (currentChar != END_OF_STRING)
		{
			++table[*tableIndex].length;
			continue;
		}
		++*tableIndex;
		table[*tableIndex].offset = table[*tableIndex - 1].offset + table[*tableIndex - 1].length + 1;
		if (*tableIndex > MAX_COUNT_OF_STRINGS)
		{
			fprintf(stderr, "File is too big. Prog got first 100 strings.\n");
			return MAKE_TABLE_SUCCESS;
		}
	}
	if (code == READ_FAILURUE)
	{
		perror("Reading file error");
		return MAKE_TABLE_FAILURE;
	}
	return MAKE_TABLE_SUCCESS;
}

int printString(int *fileDesc, int stringNumb, Table* table)
{
	if (lseek(*fileDesc, table[stringNumb - 1].offset, SEEK_SET) == LSEEK_FAILURE)
	{
		perror("Reading file error");
		return PRINT_STRING_ERROR;
	}
	char symb;
	int iter;
	for (iter = 0; iter < table[stringNumb - 1].length; iter++)
	{
		if (read(*fileDesc, &symb, sizeof(char)) == READ_FAILURUE)
		{
			perror("Reading file error");
			return PRINT_STRING_ERROR;
		}
		if (write(1, &symb, sizeof(char)) == WRITE_FAILURE)
		{
			perror("Writing error");
			return PRINT_STRING_ERROR;
		}
	}
	printf("\n");
	return PRINT_STRING_SUCCESS;
}

int processRequest(int *fileDesc, Table* table, int stringsCount, char* request)
{
	regex_t regexexit, regexrequest;
	int code;
	char error[512];
	code = regcomp(&regexexit, PATTERN_OF_EXIT, REG_EXTENDED);
	if (code != REGEX_SUCCESS) {
		perror("regex error");
		regerror(code, &regexexit, error, sizeof(error));
		return PROCESS_REQUEST_FAILURE;
	}
	code = regcomp(&regexrequest, PATTERN_OF_REQUEST, REG_EXTENDED);
	if (code != REGEX_SUCCESS) {
		perror("regex error");
		regerror(code, &regexrequest, error, sizeof(error));
		return PROCESS_REQUEST_FAILURE;
	}

	regmatch_t pm;
	code = regexec(&regexexit, request, 0, &pm, 0);
	if (code == REGEX_SUCCESS)
	{
		return END_OF_WORKING;
	}
	code = regexec(&regexrequest, request, 0, &pm, 0);
	if (code != REGEX_SUCCESS)
	{
		fprintf(stderr, "Incorrect request!\n");
		return CONTINUE_WORK;
	}

	int stringNumb = atoi(request);
	if (stringNumb > stringsCount)
	{
		printf("File has only %d strings!\n", stringsCount);
		printf("Try again\n");
		return CONTINUE_WORK;
	}
	code = printString(fileDesc, stringNumb, table);
	if (code == PRINT_STRING_ERROR)
	{
		fprintf(stderr, "Pritnting string error");
		return PROCESS_REQUEST_FAILURE;
	}
	return CONTINUE_WORK;
}

int printAllStrings(int *fileDesc)
{
	printf("Too long to wait. All file: \n");
	if (lseek(*fileDesc, 0, SEEK_SET) == LSEEK_FAILURE)
	{
		perror("Reading file error");
		return PRINT_STRING_ERROR;
	}
	char symb;
	int code;
	while(code = read(*fileDesc, &symb, sizeof(char)) > READ_CONTINUES)
	{
		if (write(1, &symb, sizeof(char)) == WRITE_FAILURE)
		{
			perror("Writing error");
			return PRINT_STRING_ERROR;
		}
	}
	if (code == READ_FAILURUE)
	{
		perror("Reading file error");
		return PRINT_STRING_ERROR;
	}
	return PRINT_STRING_SUCCESS;
}

void endWork()
{
	Signal = ALARM;
}

int main(int argc, char *argv[])
{
	int fileDesc;
	checkArgs(argc, argv[1], &fileDesc);

	Table table[MAX_COUNT_OF_STRINGS] = { 0, 0 };
	int stringsCount;
	int code;
	code = makeTable(&fileDesc, table, &stringsCount);
	if (code == MAKE_TABLE_FAILURE)
	{
		close(fileDesc);
		return EXIT_FAILURE;
	}
	char request[MAX_SIZE_OF_REQUEST];

	code = CONTINUE_WORK;
	if (signal(SIGALRM, endWork) == SIG_ERR)
	{
		perror("Signal seting error");
		close(fileDesc);
		return EXIT_FAILURE;
	}
	while (code != END_OF_WORKING)
	{
		printf("Print the number of string in 5 seconds(0 for exit)\n");
		alarm(TIME_WAIT);
		fgets(request, MAX_SIZE_OF_REQUEST, stdin);
		if (Signal == ALARM)
		{
			break;
		}
		if (strlen(request) == EMPTY_STRING)
		{
			continue;
		}
		request[strlen(request) - 1] = '\0';
		code = processRequest(&fileDesc, table, stringsCount, request);
		if (code == PROCESS_REQUEST_FAILURE)
		{
			perror("Processing request error");
			close(fileDesc);
			return EXIT_FAILURE;
		}
	}
	if (Signal == ALARM)
	{
		if (printAllStrings(&fileDesc) == PRINT_STRING_ERROR)
		{
			fprintf(stderr, "Printing all strings error\n");
			close(fileDesc);
			return EXIT_FAILURE;
		}
	}

	close(fileDesc);
	return EXIT_SUCCESS;
}