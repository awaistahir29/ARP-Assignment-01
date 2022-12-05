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


int main(int argc, char const *argv[])
{
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize User Interface 
    init_console_ui();

    // Making Named Pipes for Communication
    char *motorX_fifo = "/tmp/motorX_fifo";
    char *motorZ_fifo = "/tmp/motorZ_fifo";
    //char *inspection_fifo = "/tmp/inspection_fifo";
    char *watchdog_fifo = "/tmp/watchdog_fifo";

    int r = mkfifo(motorX_fifo, 0666); 
    if (r == -1){
        if (errno != EEXIST){
            return 1;
        }
        
    }

    mkfifo(motorZ_fifo, 0777);
    //mkfifo(inspection_fifo, 0777);
    mkfifo(watchdog_fifo, 0777);


    int fd_X = open(motorX_fifo, O_RDWR);
    if (fd_X == -1){
        return 2;
    }

    int fd_z = open(motorZ_fifo, O_RDWR);
    //int fd_IN = open(inspection_fifo, O_RDWR);
    int fd_WD = open(watchdog_fifo, O_RDWR);

    


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
                    int a = write(fd_X, &d, sizeof(d));
                    if (a == -1) {
                        printf("Writing Error \n");
                        return 3;
                        }
                }

                // Vx++ button pressed
                else if(check_button_pressed(vx_incr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Horizontal Speed Increased");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    int i = +1;
                    //char buf[80];
                    int a = write(fd_X, &i, sizeof(i));
                    if (a == -1) {
                        printf("Writing Error \n");
                        return 3;
                        }
                }

                // Vx stop button pressed
                else if(check_button_pressed(vx_stp_button, &event)) {
                    mvprintw(LINES - 1, 1, "Horizontal Motor Stopped");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    int s = 0;
                    //char buf[80];
                    int a = write(fd_X, &s, sizeof(s));
                    if (a == -1) {
                        printf("Writing Error \n");
                        return 3;
                    }
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
                    int a = write(fd_z, &dz, sizeof(dz));
                    if (a == -1) {
                        printf("Writing Error \n");
                        return 3;
                    }
                }

                // Vz++ button pressed
                else if(check_button_pressed(vz_incr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Vertical Speed Increased");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    int iz = +1;
                    //char buf[80];
                    int a = write(fd_z, &iz, sizeof(iz));
                    if (a == -1) {
                        printf("Writing Error \n");
                        return 3;
                        }
                }

                // Vz stop button pressed
                else if(check_button_pressed(vz_stp_button, &event)) {
                    mvprintw(LINES - 1, 1, "Vertical Motor Stopped");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    int sz = 0;
                    //char buf[80];
                    int a = write(fd_z, &sz, sizeof(sz));
                    if (a == -1) {
                        printf("Writing Error \n");
                        return 3;
                    }
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

    close(fd_WD);
    unlink(watchdog_fifo);

    close(fd_X);
    unlink(motorX_fifo);

    close(fd_z);
    unlink(motorZ_fifo);

    return 0;
}
