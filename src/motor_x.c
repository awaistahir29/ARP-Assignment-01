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

int fd_insp;
float position = 0;  //position of the X motor

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
    check(write(fd_insp, &position, sizeof(float)));
}

int main(int argc, char const *argv[])
{
    //open Log file
    logfile = fopen("MotorX.txt", "a");
    if (logfile == NULL)
    {
        printf("an error occured while creating Unnamed_pipe's log File\n");
        return 0;
    }
    fprintf(logfile, "***log file created***\n");
    fflush(logfile);

    //Writing in log file
    fprintf(logfile, "p - position of the X motor\n");
    fprintf(logfile, "p - mazimum position of X motor\n");
    fprintf(logfile, "p - the amount of movement made after receiving a command\n");
    fprintf(logfile, "p - the amount of seconds needed to do the movement\n");
    fflush(logfile);
    float random_error;
    float x_min = 00.00;
    float x_max = 39.00;


    float max_x = 1;                //maximum position of X motor
    float movement_distance = 1.00; //the amount of movement made after receiving a command
    float movement_time = 1;        //the amount of seconds needed to do the movement

    float movement;

    signal(SIGUSR1, sigusr1_handler);

    fd_set readfds;
    struct timeval timeout;

    char *fifo_watchdog_pid = "/tmp/watchdog_pid_x";
    char *fifo_motX_pid = "/tmp/pid_x";

    mkfifo(fifo_watchdog_pid, 0666);
    mkfifo(fifo_motX_pid, 0666);
    
    fprintf(logfile, "p - FIFO FILES HAVE MADE AND CONNECTED\n");
    fflush(logfile);

    //getting watchdog pid
    int fd_watchdog_pid = check(open(fifo_watchdog_pid, O_RDONLY));
    check(read(fd_watchdog_pid, buffer, SIZE));
    pid_watchdog = atoi(buffer);
    check(close(fd_watchdog_pid));

    //writing own pid
    int fd_motX_pid = check(open(fifo_motX_pid, O_WRONLY));
    sprintf(buffer, "%d", (int)getpid());
    check(write(fd_motX_pid, buffer, SIZE));
    check(close(fd_motX_pid));

    // Path to the named pipe
    char *motorX_fifo = "/tmp/motorX_fifo";
    char *inspection_fifo = "/tmp/insp_fifo";

    int r = mkfifo(inspection_fifo, 0666);
    if (r == -1){
        if (errno != EEXIST){
            return 1;
        }
        
    }

    int fd_X = check(open(motorX_fifo, O_RDWR));
    fd_insp = check(open(inspection_fifo, O_RDWR));
    
    while(TRUE) {
        
        FD_ZERO(&readfds);
    
        FD_SET(fd_X, &readfds);
        FD_SET(fd_insp, &readfds);

        //generating a small random error between -0.02 and 0.02
        random_error = (float)(-20 + rand() % 40) / 1000;

        switch(select(FD_SETSIZE + 1, &readfds, NULL, NULL, &timeout)){
            case 0:
                switch(x){
                    case 0:
                        printf("StoppedMotorX\n");
                        check(write(fd_insp, &position, sizeof(float)));
                        kill(pid_watchdog, SIGUSR1);
                        sleep(movement_time);
                        break;
                    case -1:
                        printf("Decreasing the speed of MotorX\n");
                        position -= movement;
                        check(write(fd_insp, &position, sizeof(float)));
                        kill(pid_watchdog, SIGUSR1);
                        sleep(movement_time);
                        break;
                    case 1:
                        printf("Increasing the speed of MotorX\n");
                        movement = movement_distance + random_error;
                        if(position + movement > x_max){
                            position = x_max;
                            int b = check(write(fd_insp, &position, sizeof(float)));
                        //kill(pid_watchdog, SIGUSR1);
                        sleep(movement_time);
                        }
                        else{
                        position += movement;
                        check(write(fd_insp, &position, sizeof(float)));
                        kill(pid_watchdog, SIGUSR1);}
                        sleep(movement_time);
                        break;
                    case 2:
                        printf("Stopped From Inspection Occured\n");
                        check(write(fd_insp, &position, sizeof(float)));
                        kill(pid_watchdog, SIGUSR1);
                        sleep(movement_time);
                        break;
                    case 3:
                        printf("Reset Command Occured\n");
                        position = 0;
                        check(write(fd_insp, &position, sizeof(float)));
                        kill(pid_watchdog, SIGUSR1);
                        sleep(movement_time);
                        }
                        break;    
            case -1:
                printf("Error Occured");
                break;
            default:
                if (FD_ISSET(fd_X, &readfds))
            {

                read(fd_X, &x, sizeof(x));
            }
            if (FD_ISSET(fd_insp, &readfds))
            {
                read(fd_insp, &x, sizeof(x));
                //strcpy(last_input_command, "");
                }
            break; 
        }
    }
    
    // Closing all opened file descripters and unlinking the fifo files
    check(close(fd_X));
    unlink(motorX_fifo);

    check(close(fd_insp));
    unlink(inspection_fifo);

    return 0;
}
