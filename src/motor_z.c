#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{

    // Path to the named pipe
    char *motorX_fifo = "/tmp/motorX_fifo";
    char *motorZ_fifo = "/tmp/motorZ_fifo";
    
    printf("Opening\n");

    printf("Opening\n");
    int fd_z = open(motorZ_fifo, O_RDONLY);
    if (fd_z == -1){
        printf("Error in Opening\n");
        return 1;
    }

    printf("Opened\n");
    
    while(TRUE) {
        int z;
        int x = read(fd_z, &z, sizeof(x));
        if (x == -1){
            printf("Error in reading from pipe\n");
            return 2;
        }

        switch(z){
            case 0:
                printf("Stopping the MotorX\n");
                break;
            case -1:
                printf("Decreasing the speed of MotorX\n");
                break;
            case 1:
                printf("Increasing the speed of MotorX\n");
                break;
        }
}
    
    // Closing all opened file descripters and unlinking the fifo files

    close(fd_z);
    unlink(motorZ_fifo);

    return 0;
}