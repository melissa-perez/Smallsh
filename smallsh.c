/*
    Assignment 3: smallsh
    Author: Melissa Perez
    OSU ID: 934-326-989
    OSU email address: peremeli@oregonstate.edu
    Last modified: 02/05/2022

    Description: The smallsh.c program is a command
    line application reacreating aspects from command
    shell programs. The main program loops indefinitely.
    Three built in commands are supported: cd, status, and exit.
    The exit command terminates the program. Other commands are
    also evaluated and support input and output redirection.
    Commands can run as either foreground or background processes.
    Variable expansion is provided for $$, resulting in the current 
    program's pid. Lastly, signal support for SIGINT and SIGTSTP
    is available.
*/

#include "smallsh_api.h"


/**
    Main entry point of the smallsh program.
    Functions in smallsh.c and documentation
    for functions in smallsh.h.

    @param int, argc
    @param const char*, argv
    @return int
*/
int main()
{
    // create the program variables needed
    char* userCommandInput = NULL;
    struct command* commandStruct = NULL;
    int lastChildStatus = EXIT_SUCCESS;
    // initialize static variables
    numBackgroundTotal = 0;
    numBackgroundCurrent = 0;
    memset(processList, 0, MAX_PROCESSES);
    memset(processExited, false, MAX_PROCESSES);
    // assign signal handlers
    signal(SIGTSTP, &SIGTSTP_On);
    signal(SIGINT, SIG_IGN);

    // infinite loop, terminates on exit command
    while (true) {
        // get user input
        GetCommandInput(&userCommandInput);
        // valid input was received
        if (userCommandInput != NULL) {
            // process the input and free the allocation of getline
            ProcessCommandLine(userCommandInput, &commandStruct);
            free(userCommandInput);
            userCommandInput = NULL;
        }
        // valid command was received and struct constructed
        if (commandStruct != NULL) {
            // run command, destroy the dynamic allocation and reset to NULL 
            // for next iteration
            RunCommand(userCommandInput, commandStruct, &lastChildStatus);
            Destructor(commandStruct);
            commandStruct = NULL;
        }
    }
    // never reaches this point, could change while true loop to a boolean and return bool
    // from exit command instead of terminating there
    return EXIT_SUCCESS;
}