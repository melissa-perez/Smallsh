/*
    Assignment 3: smallsh
    Author: Melissa Perez
    OSU ID: 934-326-989
    OSU email address: peremeli@oregonstate.edu
    Last modified: 02/05/2022

    Description:

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