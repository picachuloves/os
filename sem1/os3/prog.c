#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	printf("The real user ID: %ld\n", getuid());
	printf("The effective user ID: %ld\n", geteuid());
	
	FILE *file;
	file = fopen("file.txt", "r");
	if (file == NULL) {
		perror("Couldn't open the file");
	}
	else {
		printf("File was openned successfully\n");
		fclose(file);
	}

	//chmod UGO ls -l 1-x 2-w 4-r 

	if (setuid(getuid()))
		perror("Error setting the ID");
	printf("------------------------------\n");
	printf("After setting the user ID\n");
	printf("The real user ID: %ld\n", getuid());
	printf("The effective user ID: %ld\n", geteuid());
	file = fopen("file.txt", "r");
	if (file == NULL) {
		perror("Couldn't open the file");
	}
	else {
		printf("File was openned successfully\n");
		fclose(file);
	}
	return 0;
}
