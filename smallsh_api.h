#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>


#define MAX_ARGS 512
#define MAX_CMD_LN_CHRS 2048
#define PROMPT ":"
#define VAR_EXPR "$$"
#define HOME "HOME"
#define MAX_PROC 25
#define MAX_PROCESSES 200
 
static volatile sig_atomic_t flag;
static int numBackgroundTotal;
static int numBackgroundCurrent;
static int processList[MAX_PROCESSES];
static bool processExited[MAX_PROCESSES];

struct command {
	char*	cmd;
	char*	argList[MAX_ARGS];
	int		argListSize;
	char*	inputFile;
	char*	outputFile;
	bool	isBackgroundProc;	
};

bool CheckForCommentLine(char*);
int CheckForVariableExpression(char*);
void ChildFork(struct command*);
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