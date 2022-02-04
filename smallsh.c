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
    int lastChildStatus = EXIT_SUCCESS;
    //backgroundProcessesCount = 0;

   while (true) {
        GetCommandInput(&userCommandInput);
        ProcessCommandLine(userCommandInput, &commandStruct);
        if (commandStruct != NULL) {
            RunCommand(userCommandInput, commandStruct, &lastChildStatus);
            Destructor(commandStruct);
        }
        free(userCommandInput);
        userCommandInput = NULL;
   }
    return EXIT_SUCCESS;
}