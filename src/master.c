#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


int spawn(const char * program, char * arg_list[]) {

  pid_t child_pid = fork();

  if(child_pid < 0) {
    perror("Error while forking...");
    return 1;
  }

  else if(child_pid != 0) {
    return child_pid;
  }

  else {
    if(execvp (program, arg_list) == 0);
    perror("Exec failed");
    return 1;
  }
}

int main() {

  char * arg_list_command[] = { "/usr/bin/konsole", "-e", "./bin/command", NULL };
  char * arg_list_inspection[] = { "/usr/bin/konsole", "-e", "./bin/inspection", NULL };
  char * arg_list_motorX[] = {"/usr/bin/konsole", "-e", "./bin/motorX", "", NULL};
  char * arg_list_motorZ[] = {"/usr/bin/konsole", "-e", "./bin/motorZ", "", NULL};
  char * arg_list_watchdog[] = {"/usr/bin/konsole", "-e","./bin/watchdog", "", NULL};

  pid_t motor_x = spawn("/usr/bin/konsole", arg_list_motorX);

  pid_t pid_insp = spawn("/usr/bin/konsole", arg_list_inspection);
  
  pid_t motor_z = spawn("/usr/bin/konsole", arg_list_motorZ);
  pid_t watchdog = spawn("/usr/bin/konsole", arg_list_watchdog);
  pid_t pid_cmd = spawn("/usr/bin/konsole", arg_list_command);

  int status;
  waitpid(pid_cmd, &status, 0);
  waitpid(pid_insp, &status, 0);
  waitpid(motor_x, &status, 0);
  waitpid(motor_z, &status, 0);
  waitpid(watchdog, &status, 0);
  
  printf ("Main program exiting with status %d\n", status);
  return 0;
}

