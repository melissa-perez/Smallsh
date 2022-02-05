/*
    Assignment 3: smallsh
    Author: Melissa Perez
    OSU ID: 934-326-989
    OSU email address: peremeli@oregonstate.edu
    Last modified: 02/05/2022

    Description:

*/

#include "smallsh_api.h"

// on and off flag for SIGTSP
/*static volatile sig_atomic_t stp_flag = 0;

void on(int sig) {
    int savedErrNo = errno;
    // set the flag on
    flag = 1;
    char* message = "\nEntering foreground-only mode (& is now ignored)\n";
    write(STDOUT_FILENO, message, 52);
    // register for off next
    signal(SIGTSTP, &off);
    errno = savedErrNo;
    return;
}

void off(int sig) {
    int savedErrNo = errno;
    // set the flag off
    flag = 0;
    char* message = "\nExiting foreground-only mode\n";
    write(STDOUT_FILENO, message, 32);
    // register for on next
    signal(SIGTSTP, &on);
    errno = savedErrNo;
    return;
}

*/

/**
    Main entry point of the smallsh program.
    Functions in smallsh.c and documentation
    for functions in smallsh.h.

    @param int, argc
    @param const char*, argv
    @return int
*/
/*
int main() {
    char* userCommandInput = NULL;
    struct command* commandStruct = NULL;
    int lastChildStatus = EXIT_SUCCESS;
    //struct sigaction SIGCHLD_Action = { {0} };
    //backgroundProcessesCount = 0;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTSTP);
    while (true) {
        sigprocmask(SIG_BLOCK, &mask, NULL);
        sleep(1);
        sigprocmask(SIG_UNBLOCK, &mask, NULL);

        //GetCommandInput(&userCommandInput);
        if (userCommandInput != NULL) {
            ProcessCommandLine(userCommandInput, &commandStruct);
        }
        if (commandStruct != NULL) {
            RunCommand(userCommandInput, commandStruct, &lastChildStatus);
            Destructor(commandStruct);
            commandStruct = NULL;
        }       
        //free(userCommandInput);
        //userCommandInput = NULL;
    }
    return EXIT_SUCCESS;
}
*/
volatile sig_atomic_t flag = 0;

void off(int);

void on(int sig)
{
    flag = 1;
    write(2, "on\n", 3);
    signal(SIGTSTP, &off);
}

void off(int sig)
{
    flag = 0;
    write(2, "off\n", 4);
    signal(SIGTSTP, &on);
}

int main()
{
    signal(SIGTSTP, &on);
    int flag_save = flag;

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTSTP);

    for (int i = 0; i < 10; )
    {
        sigprocmask(SIG_BLOCK, &mask, NULL);
        if (flag_save != flag)
        {
            i++;
            flag_save = flag;
        }
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
    }
}