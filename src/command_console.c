#include "./../include/command_utilities.h"

#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/resource.h>

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
    //open Log file
    logfile = fopen("Command_Comsole.txt", "a");
    if (logfile == NULL)
    {
        printf("an error occured while creating Command Console's log File\n");
        return 0;
    }
    fprintf(logfile, "***log file created***\n");
    fflush(logfile);


    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize User Interface 
    init_console_ui();

    // Making Named Pipes for Communication
    char *motorX_fifo = "/tmp/motorX_fifo";
    char *motorZ_fifo = "/tmp/motorZ_fifo";
    char *watchdog_fifo = "/tmp/watchdog_fifo";

    int r = mkfifo(motorX_fifo, 0666); 
    if (r == -1){
        if (errno != EEXIST){
            return 1;
        }
        
    }

    mkfifo(motorZ_fifo, 0777);
    mkfifo(watchdog_fifo, 0777);
    fprintf(logfile, "p - fifo files have created and connection has been established\n");

    fflush(logfile);


    int fd_X = check(open(motorX_fifo, O_RDWR));
    fprintf(logfile, "Opened fifo file for motorX\n");
    fflush(logfile);
    int fd_z = check(open(motorZ_fifo, O_RDWR));
    fprintf(logfile, "Opened fifo file for MotorZ\n");
    fflush(logfile);
    int fd_WD = check(open(watchdog_fifo, O_RDWR));

    // Infinite loop
    while(TRUE)
	{	
        // Get mouse/resize commands in non-blocking mode...
        int cmd = getch();

        // If user resizes screen, re-draw UI
        if(cmd == KEY_RESIZE) {
            if(first_resize) {
                first_resize = FALSE;
            }
            else {
                reset_console_ui();
            }
        }
        // Else if mouse has been pressed
        else if(cmd == KEY_MOUSE) {

            // Check which button has been pressed...
            if(getmouse(&event) == OK) {

                // Vx-- button pressed
                if(check_button_pressed(vx_decr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Horizontal Speed Decreased");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                        }
                    int d = -1;
                    //char buf[80];
                    check(write(fd_X, &d, sizeof(int)));
                }

                // Vx++ button pressed
                else if(check_button_pressed(vx_incr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Horizontal Speed Increased");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    int p = 1;
                    //char buf[80];
                    check(write(fd_X, &p, sizeof(int)));
                }

                // Vx stop button pressed
                else if(check_button_pressed(vx_stp_button, &event)) {
                    mvprintw(LINES - 1, 1, "Horizontal Motor Stopped");
                    refresh();
                    //printf(logfile, "p - STOP MOTOR_X\n");
                    fprintf(logfile, "p - Wrote on fifo file\n");
                    fflush(logfile);
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    int s = 0;
                    //char buf[80];
                    check(write(fd_X, &s, sizeof(int)));
                }

                // Vz-- button pressed
                else if(check_button_pressed(vz_decr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Vertical Speed Decreased");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    int dz = -1;
                    //char buf[80];
                    check(write(fd_z, &dz, sizeof(dz)));
                }

                // Vz++ button pressed
                else if(check_button_pressed(vz_incr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Vertical Speed Increased");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    int iz = 1;
                    //char buf[80];
                    check(write(fd_z, &iz, sizeof(iz)));
                }

                // Vz stop button pressed
                else if(check_button_pressed(vz_stp_button, &event)) {
                    mvprintw(LINES - 1, 1, "Vertical Motor Stopped");
                    refresh();
                    fprintf(logfile, "p - STOP MOTOR_X\n");
                    fprintf(logfile, "p - Wrote on fifo file\n");
                    fflush(logfile);
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    int sz = 0;
                    //char buf[80];
                    check(write(fd_z, &sz, sizeof(sz)));
                }
            }
        }

        refresh();
	}

    // Terminate
    endwin();

    // Closing all opened file descripters and unlinking the fifo files
    //close(fd_IN);
    //unlink(inspection_fifo);

    check(close(fd_WD));
    unlink(watchdog_fifo);

    check(close(fd_X));
    unlink(motorX_fifo);
    fprintf(logfile, "p - UNLINKED PIPE FO MOTOR X\n");

    check(close(fd_z));
    unlink(motorZ_fifo);
    fprintf(logfile, "p - UNLINKED PIPE FOR MOTOR Z\n");

    return 0;
}
