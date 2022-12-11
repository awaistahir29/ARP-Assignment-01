#include <bits/types/struct_timeval.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int x;

int main(int argc, char const *argv[])
{
    float movement;
    float position = 0.0;
    float y_max = 9.00;
    float movement_distance = 1.0;
    int movement_time = 1;

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

    printf("Opening\n");
    int fd_z = open(motorZ_fifo, O_RDONLY);
    if (fd_z == -1){
        printf("Error in Opening\n");
        return 1;
    }

    int fd_insp_z = open(inspection_fifoZ, O_RDWR);
    if(fd_insp_z == -1){
        printf("Error Occured\n");
        return 5;
    }

    fd_set read_fd;
    struct timeval timeout;

    FD_ZERO(&read_fd);

    FD_SET(fd_z, &read_fd);
    printf("Opened\n");
    
    while(TRUE) {

        FD_ZERO(&read_fd);

        FD_SET(fd_z, &read_fd);
        FD_SET(fd_insp_z, &read_fd);

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
                        int aa = write(fd_insp_z, &position, sizeof(float));
                        if (aa == -1){
                            printf("Error Occured writing on insp");
                            return 5;
                            }
                        sleep(movement_time);
                        break;
                    case 1:
                        printf("Increasing the speed of MotorZ\n");
                        movement = movement_distance;
                        if(position + movement > y_max){
                            position = y_max;
                            int b = write(fd_insp_z, &position, sizeof(float));
                            if (b == -1){
                                printf("Error Occured writing on insp");
                                return 5;
                            }
                        sleep(movement_time);
                        }
                        else{
                        position += movement;
                        int ab = write(fd_insp_z, &position, sizeof(float));
                        if (ab == -1){
                            printf("Error Occured writing on insp");
                            return 5;
                            }
                        }
                        sleep(movement_time);
                        break;
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