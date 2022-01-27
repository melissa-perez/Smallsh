Assignment 3: SMALLSH
Student: MELISSA PEREZ

To compile use make or run the command from 2:
1. 
make
- MAKEFILE is provided.

2. 
gcc -std=gnu99 -g -Wall -o smallsh smallsh.c smallsh_api.c smallsh_api.h
- Same instruction inside MAKEFILE

To run program:
./smallsh 

To debug, you can use the below command or make debug:
valgrind -s --leak-check=yes --track-origins=yes --show-reachable=yes ./smallsh

To clean the directory for peremeli_program3:
make clean


