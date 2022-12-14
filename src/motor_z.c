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

//pointer to log file
FILE *logfile;

int x;

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

int main(int argc, char const *argv[])
{
    float movement;
    float position = 0.0;
    float y_max = 9.00;
    float movement_distance = 0.25;
    float movement_time = 1;

    float random_error;

    // Path to the named pipe
    char *motorX_fifo = "/tmp/motorX_fifo";
    char *motorZ_fifo = "/tmp/motorZ_fifo";
    char *inspection_fifoZ = "/tmp/insp_fifoZ";
    
    int r = mkfifo(inspection_fifoZ, 0666);
    if (r == -1){
        if (errno != EEXIST){
            return 1;
        }
        
    }

    int fd_z = check(open(motorZ_fifo, O_RDWR));
    int fd_insp_z = check(open(inspection_fifoZ, O_RDWR));

    fd_set read_fd;
    struct timeval timeout;

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
                        int a = write(fd_insp_z, &position, sizeof(float));
                        if (a == -1){
                            printf("Error Occured writing on insp");
                            return 5;
                            }
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
            printf("Error Occured\n");
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

    return 0;
}