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
    
    float random_error;
    float x_min = 00.00;
    float x_max = 39.00;

    float position = 0;             //position of the X motor
    float max_x = 1;                //maximum position of X motor
    float movement_distance = 1.00; //the amount of movement made after receiving a command
    float movement_time = 1;        //the amount of seconds needed to do the movement

    float movement;

    fd_set readfds;
    struct timeval timeout;

    

    // Path to the named pipe
    char *motorX_fifo = "/tmp/motorX_fifo";
    char *inspection_fifo = "/tmp/insp_fifo";


    int r = mkfifo(inspection_fifo, 0666);
    if (r == -1){
        if (errno != EEXIST){
            return 1;
        }
        
    }
    
    printf("Opening\n");
    int fd_X = open(motorX_fifo, O_RDONLY);
    if (fd_X == -1){
        printf("Error in Opening\n");
        return 1;
    }

    int fd_insp = open(inspection_fifo, O_RDWR);
    if(fd_insp == -1){
        printf("Error Opening inspection fifo\n");
        return 6;
    }

    

    printf("Opened\n");
    
    while(TRUE) {
        
        FD_ZERO(&readfds);
    
        FD_SET(fd_X, &readfds);
        FD_SET(fd_insp, &readfds);

        //generating a small random error between -0.02 and 0.02
        random_error = (float)(-20 + rand() % 40) / 1000;
        /*
        int d = read(fd_X, &x, sizeof(x));
        if (d == -1){
            printf("Error in reading from pipe\n");
            return 2;
        }
        */
        switch(select(FD_SETSIZE + 1, &readfds, NULL, NULL, &timeout)){
            case 0:
                switch(x){
                    case 0:
                        printf("StoppedMotorX\n");
                        int a = write(fd_insp, &position, sizeof(float));
                        if (a == -1){
                            printf("Error Occured writing on insp");
                            return 5;
                            }
                        sleep(movement_time);
                        break;
                    case -1:
                        printf("Decreasing the speed of MotorX\n");
                        position -= movement;
                        int aa = write(fd_insp, &position, sizeof(float));
                        if (aa == -1){
                            printf("Error Occured writing on insp");
                            return 5;
                            }
                        sleep(movement_time);
                        break;
                    case 1:
                        printf("Increasing the speed of MotorX\n");
                        movement = movement_distance;
                        if(position + movement > x_max){
                            position = x_max;
                            int b = write(fd_insp, &position, sizeof(float));
                            if (b == -1){
                                printf("Error Occured writing on insp");
                                return 5;
                            }
                        sleep(movement_time);
                        }
                        else{
                        position += movement;
                        int ab = write(fd_insp, &position, sizeof(float));
                        if (ab == -1){
                            printf("Error Occured writing on insp");
                            return 5;
                            }
                        }
                        sleep(movement_time);
                        break;
                    case 2:
                        printf("Stopped From Inspection Occured\n");
                        int aaa = write(fd_insp, &position, sizeof(float));
                        if (aaa == -1){
                            printf("Error Occured writing on insp");
                            return 5;
                            }
                        }
                        sleep(movement_time);
                        break;
                    case 3:
                        printf("Reset Command Occured\n");
                        position = 0;
                        int rs = write(fd_insp, &position, sizeof(float));
                        if (rs == -1){
                            printf("Error Occured writing on insp");
                            return 6;
                            }
                        sleep(movement_time);
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
    close(fd_X);
    unlink(motorX_fifo);


    close(fd_insp);
    unlink(inspection_fifo);

    return 0;
}
