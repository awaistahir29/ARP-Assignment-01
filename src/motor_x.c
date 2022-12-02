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

    // Making Named Pipes for Communication
    char *inspection_fifo_X = "/tmp/inspection_fifo_X";
    
    mkfifo(inspection_fifo_X, 0777);
    

    int fd_X = open(motorX_fifo, O_RDONLY);
    if (fd_X == -1){
        return 2;
    }
    int fd_IN = open(inspection_fifo_X, O_WRONLY);

    
    char count[80];
    int d = read(fd_X, count, sizeof(count));
        if (d == -1){
            printf("Error in reading from pipe\n");
            return 1;
        }
        printf("Read from Command Console");

    // Closing all opened file descripters and unlinking the fifo files
    close(fd_IN);
    unlink(inspection_fifo_X);

    close(fd_X);
    unlink(motorX_fifo);

    return 0;
}