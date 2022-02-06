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
    char* userCommandInput = NULL;
    struct command* commandStruct = NULL;
    int lastChildStatus = EXIT_SUCCESS;
    numBackgroundTotal = 0;
    numBackgroundCurrent = 0;
    memset(processList, 0, MAX_PROCESSES);
    memset(processExited, false, MAX_PROCESSES);
    signal(SIGTSTP, &SIGTSTP_On);
    signal(SIGINT, SIG_IGN);

    while (true) {
        GetCommandInput(&userCommandInput);
        if (userCommandInput != NULL) {
            ProcessCommandLine(userCommandInput, &commandStruct);
            free(userCommandInput);
            userCommandInput = NULL;
        }
        if (commandStruct != NULL) {
            RunCommand(userCommandInput, commandStruct, &lastChildStatus);
            Destructor(commandStruct);
            commandStruct = NULL;
        }
    }
        return EXIT_SUCCESS;
}