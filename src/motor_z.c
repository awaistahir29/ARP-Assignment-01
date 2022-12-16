#include <bits/types/struct_timeval.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define SIZE 80

//pointer to log file
FILE *logfile;

int x;

int pid_watchdog;
char buffer[SIZE];

float position = 0.0;
int fd_insp_z;

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

void sigusr1_handler(int sig)
{
    position = 0;
    check(write(fd_insp_z, &position, sizeof(float)));
}

int main(int argc, char const *argv[])
{
    logfile = fopen("MotorZ.txt", "a");
    if (logfile == NULL)
    {
        printf("an error occured while creating MotorZ's log File\n");
        return 0;
    }
    fprintf(logfile, "***log file created***\n");
    fflush(logfile);

    //Writing in log file
    fprintf(logfile, "p - position of the Z motor\n");
    fprintf(logfile, "p - maximum position of Z motor\n");
    fprintf(logfile, "p - the amount of movement made after receiving a command\n");
    fprintf(logfile, "p - the amount of seconds needed to do the movement\n");
    fflush(logfile);

    //randomizing seed for random error generator
    //srand(time(NULL));
    fflush(stdout);

    //Writing in log file
    fprintf(logfile, "randomizing seed for random error generator\n");
    fflush(logfile);
    float movement;
    
    float y_max = 9.00;
    float movement_distance = 0.25;
    float movement_time = 1;

    float random_error;

    signal(SIGUSR1, sigusr1_handler);

    // Path to the named pipe
    char *motorX_fifo = "/tmp/motorX_fifo";
    char *motorZ_fifo = "/tmp/motorZ_fifo";
    char *inspection_fifoZ = "/tmp/insp_fifoZ";
    char *fifo_motZ_pid = "/tmp/pid_z";

    mkfifo(fifo_motZ_pid, 0666);
    
    int r = mkfifo(inspection_fifoZ, 0666);
    //Writing in log file
    fprintf(logfile, "p - FIFO connections have been established\n");
    fflush(logfile);
    if (r == -1){
        if (errno != EEXIST){
            return 1;
        }
        
    }

    int fd_z = check(open(motorZ_fifo, O_RDWR));
    fd_insp_z = check(open(inspection_fifoZ, O_RDWR));

    fd_set read_fd;
    struct timeval timeout;

    char *fifo_watchdog_pid = "/tmp/watchdog_pid_z";
    mkfifo(fifo_watchdog_pid, 0666);

    //getting watchdog pid
    int fd_watchdog_pid = check(open(fifo_watchdog_pid, O_RDONLY));
    check(read(fd_watchdog_pid, buffer, SIZE));
    pid_watchdog = atoi(buffer);
    check(close(fd_watchdog_pid));

    //writing own pid
    int fd_motZ_pid = check(open(fifo_motZ_pid, O_WRONLY));
    sprintf(buffer, "%d", (int)getpid());
    check(write(fd_motZ_pid, buffer, SIZE));
    check(close(fd_motZ_pid));

    FD_ZERO(&read_fd);

    FD_SET(fd_z, &read_fd);
    
    while(TRUE) {

        FD_ZERO(&read_fd);

        FD_SET(fd_z, &read_fd);
        FD_SET(fd_insp_z, &read_fd);

        //generating a small random error between -0.02 and 0.02
        random_error = (float)(-20 + rand() % 40) / 1000;

        switch(select(FD_SETSIZE + 1, &read_fd, NULL, NULL, &timeout)){
            case 0:
                switch(x){
                    case 0:
                        printf("Stopping the MotorZ\n");
                        check(write(fd_insp_z, &position, sizeof(float)));
                        kill(pid_watchdog, SIGUSR1);
                        sleep(movement_time);
                        break;
                    case -1:
                        printf("Decreasing the speed of MotorZ\n");
                        position -= movement;
                        check(write(fd_insp_z, &position, sizeof(float)));
                        sleep(movement_time);
                        break;
                    case 1:
                        printf("Increasing the speed of MotorZ\n");
                        movement = movement_distance + random_error;
                        if(position + movement > y_max){
                            position = y_max;
                            check(write(fd_insp_z, &position, sizeof(float)));
                        sleep(movement_time);
                        }
                        else{
                        position += movement;
                        check(write(fd_insp_z, &position, sizeof(float)));
                        }
                        sleep(movement_time);
                        break;
                    case 2:
                        printf("Stopped From Inspection Occured\n");
                        check(write(fd_insp_z, &position, sizeof(float)));
                        sleep(movement_time);
                        break;
                    case 3:
                        printf("Reset Command Occured\n");
                        position = 0;
                        check(write(fd_insp_z, &position, sizeof(float)));
                        sleep(movement_time);
                        }
                        break;    
            case -1:
            printf("No Command Received\n");
            break;

            default:
            if (FD_ISSET(fd_z, &read_fd))
            {

                read(fd_z, &x, sizeof(x));
            }
            if (FD_ISSET(fd_insp_z, &read_fd))
            {

                read(fd_insp_z, &x, sizeof(x));
            }
            break;
            
    }
}
    
    // Closing all opened file descripters and unlinking the fifo files

    close(fd_z);
    unlink(motorZ_fifo);

    close(fd_insp_z);
    unlink(inspection_fifoZ);

    //Writing in log file
    fprintf(logfile, "p - Files have closed and unlinked\n");
    fflush(logfile);

    return 0;
}