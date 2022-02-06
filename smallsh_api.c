#include "smallsh_api.h"

void ChildFork(struct command* commandStruct) {
    int sourceFD = 0, targetFD = 0, sourceResult = 0, outResult = 0;
    // create array for command + arguments
    int newArgSize = commandStruct->argListSize + 2;
    char* newargv[newArgSize];
    newargv[newArgSize - 1] = NULL;
    newargv[0] = commandStruct->cmd;
    for (int i = 0; i < commandStruct->argListSize; ++i) {
        newargv[i + 1] = commandStruct->argList[i];
    }
    // if files are involved
    // case: command is a bg process and SIGTSTP is off
    if (flag == 0 && commandStruct->isBackgroundProc) {
        // must redirect input/output to /dev/null
        if (commandStruct->inputFile == NULL) {
            VerifyInputRedirection(DEV_NULL, &sourceFD);
            sourceResult = dup2(sourceFD, 0);
            if (sourceResult < 0) {
                perror("source dup2()");
                exit(2);
            }
            fcntl(sourceFD, F_SETFD, FD_CLOEXEC);
        }
        if (commandStruct->outputFile == NULL) {
            VerifyOutputRedirection(DEV_NULL, &targetFD);
            sourceResult = dup2(targetFD, 0);
            if (sourceResult < 0) {
                perror("target dup2()");
                exit(2);
            }
            fcntl(targetFD, F_SETFD, FD_CLOEXEC);
        }
    } 

    // case: command is a fg process and SIGTSTP is on
    // SIGTSTP forces command to run in fg
    if (commandStruct->inputFile != NULL) {
        VerifyInputRedirection(commandStruct->inputFile, &sourceFD);
        sourceResult = dup2(sourceFD, 0);
        if (sourceResult < 0) {
            perror("source dup2()");
            exit(2);
        }
        fcntl(sourceFD, F_SETFD, FD_CLOEXEC);
    }
    if (commandStruct->outputFile != NULL) {
        VerifyOutputRedirection(commandStruct->outputFile, &targetFD);
        outResult = dup2(targetFD, 1);
        if (outResult < 0) {
            perror("target dup2()");
            exit(2);
        }
        fcntl(targetFD, F_SETFD, FD_CLOEXEC);
    }

    // attempt to execute other command
    execvp(commandStruct->cmd, newargv);
    // beyond this point, if command fails print error and set exit 1
    // exit 1 used in status 
    perror(commandStruct->cmd);
    exit(1);
    return;
}

void CheckChildrenStatus(void) {
    int childStatus;
    // check background processes before returning to command line
    for (int i = 0; i < numBackgroundTotal; ++i) {
        if (waitpid(processList[i], &childStatus, WNOHANG) > 0) {
            if (!processExited[i]) {
                processExited[i] = true;
                printf("background pid %d is done: ", processList[i]);
                fflush(stdout);
                StatusCommand(childStatus);
                --numBackgroundCurrent;
            }
        }
    }
    return;
}

bool CheckForCommentLine(char* token) {
    // check to see if we ignore the entered input
    return (
            (token == NULL) ||
            ((token != NULL) && (token[0] == '\0')) ||  
            ((token != NULL) && (token[0] == '\n')) ||
            ((token != NULL) && (token[0] == '#')) ||
            (strcmp(token, "#") == 0)
        );
}

int CheckForVariableExpression(char* token) {
    int exprCount = 0;
    // counts how many times a variable expression appears in the command
    while ((token = strstr(token, VAR_EXPR))) {
        ++exprCount;
        token += strlen(VAR_EXPR);
    }
    return exprCount;
}

void CDCommand(struct command* commandStruct) {
    // attempt to change directory
    int chdirResult = (commandStruct->argListSize == 0) ? chdir(getenv(HOME)) :
        chdir(commandStruct->argList[0]);   
    if (chdirResult == -1) {
        // errno set to 2 on failure
        int errsv = errno;
        if (errsv == 2) perror("chdir");
    }
    return;
}

void Destructor(struct command* commandStruct) {
    // free up anything that is initialized
    if (commandStruct->cmd != NULL) {
        free(commandStruct->cmd);
        commandStruct->cmd = NULL;
    }
    if (commandStruct->inputFile != NULL) {
        free(commandStruct->inputFile);
        commandStruct->inputFile = NULL;
    }
    if (commandStruct->outputFile != NULL) {
        free(commandStruct->outputFile);
        commandStruct->outputFile = NULL;
    }
    
    int i = 0;
    while (i < commandStruct->argListSize) {
        free(commandStruct->argList[i]);
        commandStruct->argList[i] = NULL;
        ++i;
    }
    free(commandStruct);
    return;
}

void ExpandVariableExpression(int expCount, char* token, char** expTokenAddr) {
    char* pidString = NULL;
    GetPidString(&pidString);
    
    // need to calculate space for new string, keep in mind occurences and pid length
    int newLength = strlen(token) - (strlen(VAR_EXPR) * expCount)
        + (strlen(pidString) * expCount);
    
    int oldLength = strlen(token);
    *(expTokenAddr) = calloc(newLength + 1,
        sizeof(char));

    int tokenIndex = 0, expTokenIndex = 0, charsRead = 0;
    // iterating through token and indices
    while (tokenIndex < oldLength) {
        // while still processing $$ occr
        if (expCount > 0) {
            // copy each character up until first occurence in current
            // token pointer
            while (charsRead < strcspn(token, VAR_EXPR)) {
                (*expTokenAddr)[expTokenIndex] = token[charsRead];
                ++expTokenIndex;
                ++tokenIndex;
                ++charsRead;
            }
            // if this if proced, then there is an expression
            // add the pid in its place
            tokenIndex += strlen(VAR_EXPR);
            strcat(*expTokenAddr, pidString);
            expTokenIndex += strlen(pidString);
            token += tokenIndex;
            expCount -= 1;
        }
        else {
            // no more occrs in the string, copy anything that remains
            while (tokenIndex < oldLength) {
                (*expTokenAddr)[expTokenIndex] = token[charsRead];
                ++expTokenIndex;
                ++tokenIndex;
                ++charsRead;
            }
        }
        charsRead = 0;
    }
    free(pidString);
    pidString = NULL;
    return;
}

// Pseudocode credit to Ed Heal and Emile Cormier on StackOverflow
// https://stackoverflow.com/questions/14110738/how-to-terminate-a-child-process-which-is-running-another-program-by-doing-exec
void ExitCommand(void) {
    // no pids -- program ends normally
    if (numBackgroundCurrent == 0) exit(EXIT_SUCCESS);
    int currChildStatus;
    // go through array and clear out pids forcefully
    for (int i = 0; i < numBackgroundTotal; ++i) {
        if (!WIFEXITED(waitpid(processList[i], &currChildStatus, WNOHANG))) {
            // attempt to kill pid normally
            kill(processList[i], SIGTERM);
            // reduced time in order to terminate quicker
            sleep(1);
            waitpid(processList[i], &currChildStatus, WNOHANG);
            // if it did not work, force kill
            if (currChildStatus == 0) {
                kill(processList[i], SIGKILL);
                waitpid(processList[i], &currChildStatus, 0);
            }
            processList[i] = 0;
        }
    }
    // killing running pids is abnormal
    exit(EXIT_FAILURE);
}

void GetCommandInput(char** userInputAddr) {
    char* input = NULL;
    int charsRead;
    size_t inputLength = 2048;
    printf("%s ", PROMPT);
    fflush(stdout);
    // fix getline in case of SIGTSTP
    if ((charsRead = getline(&input, &inputLength, stdin)) != -1) {
        // valid input
        *userInputAddr = calloc(MAX_CMD_LN_CHRS + 1, sizeof(char));
        input[charsRead - 1] = '\0';
        strcpy(*userInputAddr, input);
    }
    else clearerr(stdin);
    free(input);
    input = NULL;
    return;
}

void GetPidString(char** pidStringAddr) {
    pid_t processID = getpid();
    // get the number of bytes needed for pid string
    int procIDLength = snprintf(NULL, 0, "%u", processID);
    // freed in expand variable
    *pidStringAddr = calloc(procIDLength + 1, sizeof(char));
    sprintf(*pidStringAddr, "%u", processID);
    return;
}

void ProcessCommandLine(char* userCommandLine,
    struct command** userStructAddr) {
    // max length of characters for command line is 2048
    if (strlen(userCommandLine) > MAX_CMD_LN_CHRS) return;

    char* linePtr = NULL;
    char* lineToken = strtok_r(userCommandLine, " \r\t\n\v\f", &linePtr);
    bool firstCommand = true;
    int expansionCount = 0;
    // freed end of while loop if allocated
    char* expToken = NULL;
    // check to ignore comment/blank lines
    if (CheckForCommentLine(lineToken)) {
        // check to see if any children are finished
        CheckChildrenStatus();
        return;
    }
    
    // freed in main, as well as all parts that are dynamic
    *userStructAddr = calloc(1, sizeof(struct command));
    (*userStructAddr)->argListSize = 0;

    while (lineToken != NULL) {
        //printf("Before expansion(if needed): %s\n", lineToken);
        expansionCount = CheckForVariableExpression(lineToken);
        if (expansionCount > 0) {
            ExpandVariableExpression(expansionCount, lineToken, &expToken);
            lineToken = expToken;
            //printf("After expansion(if needed): %s\n", lineToken);
        }
        //fflush(stdout);
        if (firstCommand) {
            // at this point we should have a command
            (*userStructAddr)->cmd = calloc(strlen(lineToken) + 1,
                sizeof(char));
            strcpy((*userStructAddr)->cmd, lineToken);
            firstCommand = false;
        }
        else if (strcmp(lineToken, ">") == 0) {
            lineToken = strtok_r(NULL, " \r\t\n\v\f", &linePtr);
            (*userStructAddr)->outputFile = calloc(strlen(lineToken) + 1,
                sizeof(char));
            strcpy((*userStructAddr)->outputFile, lineToken);
        }
        else if (strcmp(lineToken, "<") == 0) {
            lineToken = strtok_r(NULL, " \r\t\n\v\f", &linePtr);
            (*userStructAddr)->inputFile = calloc(strlen(lineToken) + 1,
                sizeof(char));
            strcpy((*userStructAddr)->inputFile, lineToken);
        }
        else if ((strcmp(lineToken, "&") == 0) &&
            (strcmp(linePtr, "") == 0)) {
            (*userStructAddr)->isBackgroundProc = true;
        }
        else {
            if ((*userStructAddr)->argListSize < MAX_ARGS) {
                (*userStructAddr)->argList[(*userStructAddr)->argListSize] = calloc(strlen(lineToken) + 1,
                    sizeof(char));
                strcpy((*userStructAddr)->argList[(*userStructAddr)->argListSize], lineToken);
                (*userStructAddr)->argListSize++;
            }
        }
        if (expToken != NULL) {
            // string copied over in while loop
            free(expToken);
            expToken = NULL;
        }
        // continue parsing
        lineToken = strtok_r(NULL, " \r\t\n\v\f", &linePtr);
    }
    return;
}

void RunCommand(char* userCommandInput,
    struct command* commandStruct,
    int* lastStatus) {
    // determine how to handle the first command given
    // cd, exit, status, or fork to exec
    if ((strcmp(commandStruct->cmd, "exit") == 0)) {
        // can't pass args to exit, killing smallsh, clear data 
        free(userCommandInput);
        Destructor(commandStruct);
        ExitCommand();
    }
    else if ((strcmp(commandStruct->cmd, "status") == 0)) {
        StatusCommand(*lastStatus);     
    }
    else if ((strcmp(commandStruct->cmd, "cd") == 0)) {
        CDCommand(commandStruct);
    }
    else {
        OtherCommand(lastStatus, commandStruct);
    }
    return;
}

// Credit to Prof. Gambord on forums for providing signal handling help
// Both SIGTSTP_On and SIGTSTP_Off
void SIGTSTP_On(int sig) {
    int savedErrNo = errno;
    // set the flag on
    flag = 1;
    char* message = "\nEntering foreground-only mode (& is now ignored)\n: ";
    write(STDOUT_FILENO, message, 53);
    // register for off next
    signal(SIGTSTP, &SIGTSTP_Off);
    errno = savedErrNo;
    return;
}

void SIGTSTP_Off(int sig) {
    int savedErrNo = errno;
    // set the flag off
    flag = 0;
    char* message = "\nExiting foreground-only mode\n: ";
    write(STDOUT_FILENO, message, 33);
    // register for on next
    signal(SIGTSTP, &SIGTSTP_On);
    errno = savedErrNo;
    return;
}

int StatusCommand(int status) {
    // check if termination by signal or exit status
    if (WIFEXITED(status)) printf("exit value %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status)) printf("terminated by signal %d\n", WTERMSIG(status));
    fflush(stdout);
    // if neither, then ran before fg, return 0
    return EXIT_SUCCESS;
}

void OtherCommand(int* resultStatus,
    struct command* commandStruct) {
    // begin fork process
    pid_t childPid = fork();
    int childStatus = 0;

    switch (childPid) {
        case -1:
            perror("fork() failed!");
            break;
        case 0:
            // fork is sucessful, give child spawn command info
            // to execute
            if (flag == 1 || !commandStruct->isBackgroundProc) {
                signal(SIGINT, SIG_DFL);
            }
            // register SIGTSTP on children to ignore.
            signal(SIGTSTP, SIG_IGN);
            ChildFork(commandStruct);
            break;
        default:
            if (commandStruct->isBackgroundProc) {
                // if the flag is on, then the background proc must
                // be ran as a foreground program
                if (flag == 1) {                    
                    childPid = waitpid(childPid, &childStatus, 0);
                }
                else {
                    // return without waiting on the process to end
                    processList[numBackgroundTotal] = childPid;
                    //waitpid(childPid, &childStatus, WNOHANG);
                    // this count is needed for my process list 
                    ++numBackgroundTotal;
                    // this is current count for exit termination
                    ++numBackgroundCurrent;
                    printf("background pid is %d\n", childPid);
                    fflush(stdout);
                }
            }
            else {
                childPid = waitpid(childPid, &childStatus, 0);
            }
    }
    // store last foreground process result for status command
    *resultStatus = childStatus;
    // if signal terminated, print the killing signal
    if (WIFSIGNALED(*resultStatus)) StatusCommand(*resultStatus);
    // point before returning to command line
    CheckChildrenStatus();
    return;
}

void VerifyInputRedirection(char* infile,
    int* fileDescriptor) {
    // attempt to open file location for reading
    int openFile = open(infile, O_RDONLY);
    if (openFile < 0) {
        printf("cannot open %s for input\n", infile);
        fflush(stdout);
        exit(1);
    }
    // save fd for fork use 
    *fileDescriptor = openFile;
    return;
}

void VerifyOutputRedirection(char* outfile,
    int* fileDescriptor) {
    // attempt to open file location for writing
    int openFile = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (openFile < 0) {
        printf("cannot open %s for output\n", outfile);
        fflush(stdout);
        exit(1);
    }
    // save fd for fork use 
    *fileDescriptor = openFile;
    return;
}