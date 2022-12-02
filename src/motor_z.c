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
    char * motorZ_fifo = "/tmp/motorZ_fifo";

    // Making Named Pipes for Communication
    char * inspection_fifo_Z = "/tmp/inspection_fifo_Z";

    mkfifo(inspection_fifo_Z, 0777);

    int fd_X = open(motorZ_fifo, O_RDONLY);
    int fd_IN = open(inspection_fifo_Z, O_WRONLY);


    // Closing all opened file descripters and unlinking the fifo files
    close(fd_IN);
    unlink(inspection_fifo_Z);

    close(fd_X);
    unlink(motorZ_fifo);
}