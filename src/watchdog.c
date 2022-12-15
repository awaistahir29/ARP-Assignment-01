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

#define TIMEDELTA 20

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
    //open Log file
    logfile = fopen("Watchdog.txt", "a");
    if (logfile == NULL)
    {
        printf("an error occured while creating Watchdog's log File\n");
        return 0;
    }
    fprintf(logfile, "******log file created******\n");
    fflush(logfile);


    signal(SIGUSR1, signal_handler);

    char *fifo_watchdog_pid_motX = "/tmp/watchdog_pid_x";

    mkfifo(fifo_watchdog_pid_motX, 0666);

    int fd_watchdog_pid_motX = check(open(fifo_watchdog_pid_motX, O_WRONLY));

    check(write(fd_watchdog_pid_motX, buffer, strlen(buffer) + 1));
    sprintf(buffer, "%d", (int)getpid());

    check(close(fd_watchdog_pid_motX));
    unlink(fifo_watchdog_pid_motX);

    time_check = time(NULL);

    while (1)
    {
        sleep(1);
        ////Writing in log file
        fprintf(logfile, "p - Sleeping\n");
        fflush(logfile);
        if (difftime(time(NULL), time_check) >= TIMEDELTA)
        {   
            printf("Session Expired \n");
            fprintf(logfile, "-------------Time Out------------\n");

            // kill(pid_motX, SIGUSR2);

            ////Writing in log file
            fprintf(logfile, "p - MotorX process has killed\n");
            fflush(logfile);

            // kill(pid_motZ, SIGUSR2);

            //Writing in log file
            fprintf(logfile, "p - MotorZ process has killed\n");
            fflush(logfile);

            time_check = time(NULL);
        }
    }
    return 0;
}