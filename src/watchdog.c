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

#define TIMEDELTA 10

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
    printf("Received Signal\n");
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

    printf("Ready to receive Receive Signal\n");

    signal(SIGUSR1, signal_handler);

    char *fifo_watchdog_pid_motX = "/tmp/watchdog_pid_x";
    char *fifo_watchdog_pid_motZ = "/tmp/watchdog_pid_z";
    char *fifo_motX_pid = "/tmp/pid_x";
    char *fifo_motZ_pid = "/tmp/pid_z";

    mkfifo(fifo_watchdog_pid_motZ, 0666);
    mkfifo(fifo_watchdog_pid_motX, 0666);
    mkfifo(fifo_motX_pid, 0666);
    mkfifo(fifo_motZ_pid, 0666);

    int fd_watchdog_pid_motX = check(open(fifo_watchdog_pid_motX, O_WRONLY));
    sprintf(buffer, "%d", (int)getpid());
    check(write(fd_watchdog_pid_motX, buffer, strlen(buffer) + 1));
    check(close(fd_watchdog_pid_motX));
    unlink(fifo_watchdog_pid_motX);

    int fd_watchdog_pid_motZ = check(open(fifo_watchdog_pid_motZ, O_WRONLY));
    sprintf(buffer, "%d", (int)getpid());
    check(write(fd_watchdog_pid_motZ, buffer, strlen(buffer) + 1));
    check(close(fd_watchdog_pid_motZ));
    unlink(fifo_watchdog_pid_motZ);


    int fd_pid_motX = check(open(fifo_motX_pid, O_RDONLY));
    check(read(fd_pid_motX,buffer,SIZE));
    pid_motX=atoi(buffer);

    int fd_pid_motZ = check(open(fifo_motZ_pid, O_RDONLY));
    check(read(fd_pid_motZ,buffer,SIZE));
    pid_motZ=atoi(buffer);

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

            kill(pid_motX, SIGUSR1);

            ////Writing in log file
            fprintf(logfile, "p - MotorX process has killed\n");
            fflush(logfile);

            kill(pid_motZ, SIGUSR1);

            //Writing in log file
            fprintf(logfile, "p - MotorZ process has killed\n");
            fflush(logfile);

            time_check = time(NULL);
        }
    }
    return 0;
}