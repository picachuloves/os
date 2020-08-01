#include <stdio.h> 
#include <time.h> 
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

extern char *tzname[];
extern char **environ;
int main() {
	if (putenv("TZ=:America/Los_Angeles") != 0) {
		perror("Seting TZ error");
		return 1;
	}
	time_t rawtime;
	struct tm *sp;
	int i = 0;
	if (time(&rawtime) <= 0)
	{
		perror("Calendar time is not available");
		return 1;
	}
	

	printf("Time and date in California: %s", ctime(&rawtime));
	
	sp = localtime(&rawtime);

	printf("%d/%d/%02d %d:%02d %s\n",
		sp->tm_mon + 1, sp->tm_mday,
		sp->tm_year + 1900, sp->tm_hour, sp->tm_min,
		tzname[sp->tm_isdst]);
	
	while(environ[i] != NULL) {
		printf("%s\n", environ[i]);
		i++;
	}
	//локальные, пользоватльские - файлы конфигурации .profile  при входе пользователя, системные при старте системы /etc/environment /etc/profile 
	// getenv putenv
	//tm_sec	int	seconds after the minute
	//tm_min	int	minutes after the hour	
	//tm_hour	int	hours since midnight
	//tm_mday	int	day of the month	
	//tm_mon	int	months since January
	//tm_year	int	years since 1900	
	//tm_wday	int	days since Sunday
	//tm_yday	int	days since January 1	
	//tm_isdst	int	Daylight Saving Time flag		
	exit(0);
}
