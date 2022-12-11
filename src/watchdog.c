#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/resource.h>

#define SIZE 80

#define TIMEDELTA 60

//pointer to log file
FILE *logfile;

int fd_starter;
char buffer[SIZE];

int pid_motX;
int pid_motZ;
int pid_inspection;
int pid_command;

time_t time_check;

int check(int retval)
{
    if (retval == -1)
    {
        fprintf(logfile, "\nERROR (" __FILE__ ":%d) -- %s\n", __LINE__, strerror(errno));
        fflush(logfile);
        fclose(logfile);
        printf("\tAn error has been reported on log file.\n");
        fflush(stdout);
        exit(-1);
    }
    return retval;
}

void signal_handler(int sig)
{
    time_check = time(NULL);
}

int main(int argc, char const *argv[])
{


    return 0;
}