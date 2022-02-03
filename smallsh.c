/*
    Assignment 3: smallsh
    Author: Melissa Perez
    OSU ID: 934-326-989
    OSU email address: peremeli@oregonstate.edu
    Last modified: 01/27/2022

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
int main() {
    char* userCommandInput = NULL;
    struct command* commandStruct = NULL;
    int lastFGStatus = -2;
    //backgroundProcessesCount = 0;

    GetCommandInput(&userCommandInput);
    ProcessCommandLine(userCommandInput, &commandStruct);
    RunCommand(userCommandInput, commandStruct, &lastFGStatus);

    free(userCommandInput);
    Destructor(commandStruct);
    return EXIT_SUCCESS;
}