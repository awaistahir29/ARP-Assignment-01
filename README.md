# ARP-Hoist-Control

## Introduction

Base project structure for the first *Advanced and Robot Programming* (ARP) assignment.
There are two motors.
MotorX, MotorZ 
These motors are moving the along to axis to move the object vertical and horizontal axis.

The movement is bound from 0 to a maximum value of distance, which is set to 1, for each motor. 
![WhatsApp Image 2022-12-23 at 5 16 18 PM](https://user-images.githubusercontent.com/104999107/209366576-e1f2a79e-006d-420c-93fb-7e392ace60d6.jpeg)



In the project there are 6 processes: 

1) starter
2) commandConsole
3) inspectionConsole
4) motorX 
5) motorZ 
6) watchdog 

From the user side the commandConsole and the inspectionConsole acts as an interface to respectively manage the movement and other commands and to view the current position on the axis and to stop or reset the hoist position.

The communication between the processes was managed through the use of pipes, that allow to write or read from a process to another one
through a one to one connection.

The project provides the basic functionalities for the **Command** and **Inspection processes**, both of which are implemented through the *ncurses library* as simple GUIs. In particular, the repository is organized as follows:
- The `src` folder contains the source code for the Command, Inspection, MotorX, MotorZ and Master processes.
- The `include` folder contains all the data structures and methods used within the ncurses framework to build the two GUIs. Unless you want to expand the graphical capabilities of the UIs (which requires understanding how ncurses works), you can ignore the content of this folder, as it already provides you with all the necessary functionalities.
- The `bin` folder is where the executable files are expected to be after compilation
- The `run.sh` is a shell script which complie all of the processes and execute the master and the master process is a parent process, responsible for executing all child processes.



## Ncurses installation
To install the ncurses library, simply open a terminal and type the following command:
```console
sudo apt-get install libncurses-dev
```

## Compiling and running the code
The Command and Inspection processes depend on the ncurses library, which needs to be linked during the compilation step. Furthermore, the Inspection process also uses the mathematical library for some additional computation. Therefore the steps to compile are the following:
1. For the **Installation**:

	 ```console
	./install.sh
	```
This command will install and unzip all the files once you have clone the repository.

2. For the **Run**:
         
	 ```console
	./run.sh
	```
THis command will run the run.sh file which will automatically run all the files the command console, inspection console and the master.

3. For the **Help**:

	```console
	./help.sh
	```
This Command will show the list of all the motors and how to control them.

## Uninstalling

For the **Uninstallation**:

	./uninstall.sh

This command will uninstall all the files once you will excute thi command.

![Command and inspection koncole](https://user-images.githubusercontent.com/104999107/209402215-d7baf132-4db7-43df-8481-ef3827715fca.jpeg)

[How to install.webm](https://user-images.githubusercontent.com/104999107/209402812-d17ae5cc-2071-42d9-b4ea-ac79550ded0f.webm)

[Controlling.webm](https://user-images.githubusercontent.com/104999107/209402971-74c0771d-0e62-41aa-b6ba-aa6315cb9d22.webm)



## Troubleshooting

Should you experience some weird behavior after launching the application (buttons not spawning inside the GUI or graphical assets misaligned) simply try to resize the terminal window, it should solve the bug.

You take other help from the help command.
