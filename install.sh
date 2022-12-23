# install <pathname>

unzip include.zip
unzip src.zip 
mkdir bin

gcc src/motor_x/motor_x.c -o bin/motorX
gcc src/motor_z/motor_z.c -o bin/motorZ
gcc src/command_console/command_console.c -lncurses -lm -o bin/command
gcc src/inspectionConsole/inspection_console.c -lncurses -lm -o bin/inspection

gcc src/starter/master.c -o bin/master
gcc src/watchdog/watchdog.c -o bin/watchdog

mkdir logs

echo install completed
