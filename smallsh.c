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
int main() {
    char* userCommandInput = NULL;
    struct command* commandStruct = NULL;
    int lastChildStatus = EXIT_SUCCESS;
    //s//truct sigaction SIGTSTP_Action = { {0} };
    //struct sigaction SIGCHLD_Action = { {0} };
    //struct sigaction ignore_action = { {0} };


    //backgroundProcessesCount = 0;

   while (true) {
        GetCommandInput(&userCommandInput);
        ProcessCommandLine(userCommandInput, &commandStruct);
        if (commandStruct != NULL) {
            RunCommand(userCommandInput, commandStruct, &lastChildStatus);
            Destructor(commandStruct);
            commandStruct = NULL;
        }       
        free(userCommandInput);
        userCommandInput = NULL;
   }
    return EXIT_SUCCESS;
}