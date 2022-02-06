#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 512
#define MAX_CMD_LN_CHRS 2048
#define MAX_PROCESSES 200
#define PROMPT ":"
#define VAR_EXPR "$$"
#define HOME "HOME"
#define DEV_NULL "/dev/null"

// used for SIGTSTP
static volatile sig_atomic_t flag;
// used for keeping track of background processes
static int numBackgroundTotal;
static int numBackgroundCurrent;
static int processList[MAX_PROCESSES];
static bool processExited[MAX_PROCESSES];

/*
	Parses input into the different categories of 
	a command mentioned per assignment.
	@param char* - cmd: main command to run
	@param char* - argList[]: holds up to MAX_ARGS of allowable command arguments
	@param int - argListSize: alloted arguments in argList
	@param char* - inputFile: file for input redirection
	@param char* - ouputFile: file for output redirection
	@param bool - isBackgroundProc: does the command end in &

*/
struct command {
	char*	cmd;
	char*	argList[MAX_ARGS];
	int		argListSize;
	char*	inputFile;
	char*	outputFile;
	bool	isBackgroundProc;	
};

/*
*	@brief Prepares the information in the command
*   struct in a manner that allows for exec to be
*	called. Also handles redirection of input/output
*   to appropriate locations before executing. Sets
*   exit status 1 on failure.
*   
*	@param struct command* - user input as struct
*/
void ChildFork(struct command*);

/*
*	@brief Cycles through the static array
*	holding the current background processes.
*	If a child process is complete, it prints
*	the termination message and status.
*	@param void - no parameters
*/
void CheckChildrenStatus(void);

/*
*	@brief Checks to see if the command entered
*	is a comment or blank line that should be ignored.
*	Returns true if it is.
*	@param char* - user input
*   @return bool
*/
bool CheckForCommentLine(char*);

int CheckForVariableExpression(char*);


void CDCommand(struct command*);
void Destructor(struct command*);
void ExitCommand(void);
void ExpandVariableExpression(int, char*, char**);
void GetCommandInput(char**);
void GetPidString(char**);
void SIGTSTP_Off(int);
void SIGTSTP_On(int);
void ProcessCommandLine(char*, struct command**);
void RunCommand(char*, struct command*, int*);
int StatusCommand(int);
void OtherCommand(int*, struct command*);
void VerifyInputRedirection(char*, int*);
void VerifyOutputRedirection(char*, int*);
#endif