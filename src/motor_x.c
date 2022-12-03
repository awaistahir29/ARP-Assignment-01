#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char const *argv[])
{
    
    float x_min = 00.00;
    float x_max = 38.00;

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
        int x;
        float c;
        int d = read(fd_X, &x, sizeof(x));
        if (d == -1){
            printf("Error in reading from pipe\n");
            return 2;
        }

        switch(x){
            case 0:
                printf("Stopping the MotorX\n");
                break;
            case -1:
                printf("Decreasing the speed of MotorX\n");
                /*
                for(int i = x_min; i == 0; i--){
                    x_min--;
                    printf("The Value is: %.2f\n", x_min);
                    
                    int wr = write(fd_insp, &x_min, sizeof(float));
                    if(wr == -1){
                        printf("Error Writing in the fifo\n");
                        return 7;
                    }
                }
                */
                break;
            case 1:
                printf("Increasing the speed of MotorX\n");
                for(int i = x_min; i <= x_max; i++){
                    x_min++;
                    //n = n + 1;
                    printf("The Value is: %.2f\n", x_min);
                    
                    int wr = write(fd_insp, &x_min, sizeof(float));
                    if(wr == -1){
                        printf("Error Writing in the fifo\n");
                        return 7;
                    }
                    sleep(1);
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
