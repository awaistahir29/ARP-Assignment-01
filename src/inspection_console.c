#include "./../include/inspection_utilities.h"

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

//pointer to log file
FILE *logfile;



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
    char *inspection_fifo = "/tmp/insp_fifo";

    char *inspection_fifoZ = "/tmp/insp_fifoZ";

    char *motorX_fifo = "/tmp/motorX_fifo";
    
    int fd_motor_X = check(open(motorX_fifo, O_WRONLY));

    int fd_insp = open(inspection_fifo, O_RDONLY);
    if(fd_insp == -1){
        printf("Error Opening inspection fifo\n");
        return 6;
    }

    int fd_insp_z = open(inspection_fifoZ, O_RDONLY);
    if(fd_insp_z == -1){
        printf("Error Opening inspection fifo\n");
        return 7;
    }
    printf("Opened FIle Z\n");

    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // End-effector coordinates
    float ee_x, ee_y;

    // Initialize User Interface 
    init_console_ui();

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

                // STOP button pressed
                if(check_button_pressed(stp_button, &event)) {
                    mvprintw(LINES - 1, 1, "STP button pressed");
                    int sp = 2;
                    check(write(fd_motor_X, &sp, sizeof(int)));
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // RESET button pressed
                else if(check_button_pressed(rst_button, &event)) {
                    mvprintw(LINES - 1, 1, "RST button pressed");
                    int sp = 3;
                    check(write(fd_motor_X, &sp, sizeof(int)));
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    mvprintw(LINES - 1, 1, "RST button pressed");
                }
            }
        }
        
        //float x;
        int d = read(fd_insp, &ee_x, sizeof(float));
        if (d == -1){
            printf("Error in reading from pipe\n");
            return 2;
        }

        int e = read(fd_insp_z, &ee_y, sizeof(float));
        if (e == -1){
            printf("Error in reading from pipe\n");
            return 3;
        }
        /*
        
        // To be commented in final version...
        switch (cmd)
        {
            case KEY_LEFT:
                //ee_x--;
                break;
            case KEY_RIGHT:
                //ee_x++;
                break;
                
            case KEY_UP:
                ee_y--;
                break;
            case KEY_DOWN:
                ee_y++;
                break;
            default:
                break;
        }
        */
        
        // Update UI
        update_console_ui(&ee_x, &ee_y);
	}

    // Terminate
    endwin();

    close(fd_insp);
    unlink(inspection_fifo);

     close(fd_insp_z);
    unlink(inspection_fifoZ);

    close(fd_motor_X);
    unlink(motorX_fifo);

    return 0;
}
