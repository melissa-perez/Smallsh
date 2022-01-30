#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h> // for waitpid
#include <time.h>
#include <unistd.h>


#define MAX_ARGS 512
#define MAX_CMD_LN_CHRS 2048
#define PROMPT ":"
#define VAR_EXPR "$$"


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
void Destructor(struct command*);
void ExpandVariableExpression(int, char*, char**);
void GetCommandInput(char**);
void GetPidString(char**);
void ProcessCommandLine(char*, struct command**);

#endif