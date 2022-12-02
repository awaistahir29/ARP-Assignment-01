gcc src/motor_x.c -o bin/motorX

gcc src/inspection_console.c -lncurses -lm -o bin/inspection
gcc src/command_console.c -lncurses -o bin/command
gcc src/motor_z.c -o bin/motorZ
gcc src/watchdog.c -o bin/watchdog

gcc src/master.c -o bin/master

./bin/master
