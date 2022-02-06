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
// Picked insanely high number because I don't clear out the
// actual array, did not have time to implement a nice
// background process structure(aka Linked List or circular
// array to reuse old spots, SIGCHLD), we don't need these many indices
#define MAX_PROCESSES 1000
#define PROMPT ":"
#define VAR_EXPR "$$"
#define HOME "HOME"
#define DEV_NULL "/dev/null"

// used for SIGTSTP -- TY Prof. Gambord
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
*/
bool CheckForCommentLine(char*);

/*
*	@brief Finds the number of times the variable
*	expansion parameter appears in the command.
*	Returns the count.
*	@param char* - user input
*/
int CheckForVariableExpression(char*);

/*
*	@brief Executes the built in cd command 
*   for smallsh. Changes directory to HOME if
*	no argument is passed. Else, changes the 
*	current directory to a valid location
*	given.
*	@param struct command* - built command struct
*/
void CDCommand(struct command*);

/*
*	@brief Frees all the dynamically allocated
*   data for the command struct.
*	@param struct command* - built command struct
*/
void Destructor(struct command*);

/*
*	@brief Executes the built in exit command
*   for smallsh. Terminates the program and kills
*   any running background processes.
*	@param void - no parameters
*/
void ExitCommand(void);

/*
*	@brief Exapands any instance of the pattern
*	$$ into the PID of the current running program
*   for smallsh.
*	@param int - count of variable expressions
* *	@param char* - user command input
* *	@param char** - result of expansion
*/
void ExpandVariableExpression(int, char*, char**);

/*
*	@brief Gets the user input from the command line.
* *	@param char** - user input from getline
*/
void GetCommandInput(char**);

/*
*	@brief Gets the PID string of the current smallsh process used
*	in variable expansion.
* *	@param char** - pid_t turned to string
*/
void GetPidString(char**);

/*
*	@brief Executes the non-built in commands
*   for smallsh. Uses a child process to accomplish this.
*	@param int* - used to store last foreground status
*	@param struct command* - built command struct
*/
void OtherCommand(int*, struct command*);

/*
*	@brief Forms a struct for the user input by 
*	tokenizing the string into components. Validates
*	the command as well. Expands if necessary.
*	@param char* - user input from getline
*	@param struct command* - built command struct
*/
void ProcessCommandLine(char*, struct command**);

/*
*	@brief Redirects input to the correct commands
*	if either built in or other. 
*	tokenizing the string into components. Validates
*	the command as well. Expands if necessary.
*	@param char* - user input from getline
*	@param struct command* - built command struct
*/
void RunCommand(char*, struct command*, int*);

/*
*	@brief Register handler for SIGTSTP off process.
*	When registered and triggered it will bring the program
*	out of foreground only mode.
*	@param int - signal number
*/
void SIGTSTP_Off(int);

/*
*	@brief Register handler for SIGTSTP on process.
*	When registered and triggered it will bring the program
*	into foreground only mode.
*	@param int - signal number
*/
void SIGTSTP_On(int);

/*
*	@brief Executes the built in status command
*   for smallsh. Displays either the exit value
*	or terminating signal of the last foreground process.
*	Return exit 0 if ran before any foreground process that
*	is not the built in commands.
*	@param int - status value to evaluate
*/
int StatusCommand(int);

/*
*	@brief Verifies if the input location is valid to open
*	and use for input redirection. Prints an error and sets 
*	exit status 1 if failure.
*   @param char* - input file location
*	@param int - file descriptor to save
*/
void VerifyInputRedirection(char*, int*);

/*
*	@brief Verifies if the output location is valid to open
*	and use for output redirection. Prints an error and sets
*	exit status 1 if failure.
*   @param char* - output file location
*	@param int - file descriptor to save
*/
void VerifyOutputRedirection(char*, int*);
#endif