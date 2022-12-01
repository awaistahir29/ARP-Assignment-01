gcc src/inspection_console.c -lncurses -lm -o bin/inspection
gcc src/command_console.c -lncurses -o bin/command
gcc src/master.c -o bin/master

./bin/master
