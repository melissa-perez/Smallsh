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
    Main entry point of the movies_by_year program.
    Functions in movies_by_year_api.c and documentation
    for functions in movies_by_year_api.h.

    @param int, argc
    @param const char*, argv
    @return int
*/
int main() {
	char* userCommandInput = NULL;
    struct command* commandStruct = NULL;

    GetCommandInput(&userCommandInput);
    ProcessCommandLine(userCommandInput, &commandStruct);


    free(userCommandInput);
    free(commandStruct);
	return 0;
}