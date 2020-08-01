#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>
#include <signal.h>
#include <sys/mman.h>
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
#define PATTERN_OF_EXIT "^\s{0,}0"
#define REGEX_SUCCESS 0
#define PRINT_STRING_ERROR 0
#define PRINT_STRING_SUCCESS 1
#define PROCESS_REQUEST_FAILURE -1
#define MAKE_TABLE_FAILURE 0
#define MAKE_TABLE_SUCCESS 1
#define TIME_WAIT 5
#define ALARM 1
#define EMPTY_STRING 1
#define PROCESS_UI_FAILURE 0
#define PROCESS_UI_SUCCESS 1
#define MUNMAP_FAILURE -1
#define FINISH_MUNMAP_FAILURE 0
#define FINISH_MUNMAP_SUCCESS 1

typedef struct Table
{
	off_t offset;
	int length;
}Table;

int Signal = NULL;
char *mappedFile = NULL;
size_t fileLength = 0;

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

int makeTable(Table* table, int* tableIndex)
{
	int code;

	*tableIndex = 0;
	size_t iter;
	for (iter = 0; iter < fileLength; iter++)
	{
		if (mappedFile[iter] != END_OF_STRING)
		{
			++table[*tableIndex].length;
			continue;
		}
		++*tableIndex;
		if (*tableIndex > MAX_COUNT_OF_STRINGS)
		{
			fprintf(stderr, "File is too big. Prog got first 100 strings.\n");
			return MAKE_TABLE_SUCCESS;
		}
		table[*tableIndex].offset = table[*tableIndex - 1].offset + table[*tableIndex - 1].length + 1;
	}
	if (code == READ_FAILURUE)
	{
		perror("Reading file error");
		return MAKE_TABLE_FAILURE;
	}
	return MAKE_TABLE_SUCCESS;
}

void printString(int stringNumb, Table* table)
{
	char symb;
	off_t iter;
	for (iter = table[stringNumb - 1].offset; iter < table[stringNumb - 1].offset + table[stringNumb - 1].length; iter++)
	{
		printf("%c", mappedFile[iter]);
	}
	printf("\n");
}

int processRequest(Table* table, int stringsCount, char* request)
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
	printString(stringNumb, table);
	return CONTINUE_WORK;
}

int printAllStrings()
{
	printf("Too long to wait. All file: \n");
	char symb;
	off_t iter;
	for (iter = 0; iter < fileLength; iter++)
	{
		printf("%c", mappedFile[iter]);
	}
}

void endWork()
{
	Signal = ALARM;
}

void *mapFile(int *fileDesc)
{
	off_t filesize = lseek(*fileDesc, 0, SEEK_END);
	if (filesize == LSEEK_FAILURE)
	{
		perror("Reading file error");
		return NULL;
	}

	fileLength = (size_t)filesize;
	char *mappedfile = mmap(NULL, fileLength, PROT_READ, MAP_PRIVATE, *fileDesc, 0);
	if (mappedFile == MAP_FAILED)
	{
		perror("Mapping error");
		return NULL;
	}

	close(*fileDesc);

	return mappedfile;
}

int processUserInput(Table* table, int stringsCount)
{
	char request[MAX_SIZE_OF_REQUEST];
	int code;
	code = CONTINUE_WORK;
	if (signal(SIGALRM, endWork) == SIG_ERR)
	{
		perror("Signal seting error");
		return PROCESS_UI_FAILURE;
	}
	while (code != END_OF_WORKING)
	{
		printf("Print the number of string in 5 seconds(0 for exit)\n");
		alarm(TIME_WAIT);
		if (fgets(request, MAX_SIZE_OF_REQUEST, stdin) == EOF)
		{
			fprintf(stderr, "Reading request error\nTry again\n");
			continue;
		}
		if (Signal == ALARM)
		{
			break;
		}
		if (strlen(request) == EMPTY_STRING)
		{
			continue;
		}
		request[strlen(request) - 1] = '\0';
		code = processRequest(table, stringsCount, request);
		if (code == PROCESS_REQUEST_FAILURE)
		{
			perror("Processing request error");
			return PROCESS_UI_FAILURE;
		}
	}
	if (Signal == ALARM)
	{
		if (printAllStrings() == PRINT_STRING_ERROR)
		{
			fprintf(stderr, "Printing all strings error\n");
			return PROCESS_UI_FAILURE;
		}
	}
	return PROCESS_UI_SUCCESS;
}

int finishmap() 
{
	int code;
	if (code = munmap(mappedFile, fileLength) == MUNMAP_FAILURE) 
	{
		perror("Releasing resources error");
		return FINISH_MUNMAP_FAILURE;
	}
	return FINISH_MUNMAP_SUCCESS;
}

int main(int argc, char *argv[])
{
	int fileDesc;
	checkArgs(argc, argv[1], &fileDesc);

	mappedFile = mapFile(&fileDesc);
	if (mappedFile == NULL)
	{
		return EXIT_FAILURE;
	}

	Table table[MAX_COUNT_OF_STRINGS] = { 0, 0 };
	int stringsCount;
	int code;
	code = makeTable(table, &stringsCount);
	if (code == MAKE_TABLE_FAILURE)
	{
		return EXIT_FAILURE;
	}

	if (processUserInput(table, stringsCount) == PROCESS_UI_FAILURE)
	{
		fprintf(stderr, "ProcessUserInput error\n");
		finishmap();//обработка ошибки
		return EXIT_FAILURE;
	}

	if (code = finishmap() == FINISH_MUNMAP_FAILURE)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}