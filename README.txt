Assignment 3: Smallsh
Student: Melissa Perez

To compile use make or run the command from 2:
1. 
make
- MAKEFILE is provided.

2. 
gcc -std=gnu99 -g -Wall -o smallsh smallsh.c smallsh_api.c smallsh_api.h
- Same instruction inside MAKEFILE

To run program:
./smallsh 

Debug is available, but signals do not work with Valgrind.
make debug

To clean the directory for peremeli_program3:
make clean
