#include "smallsh_api.h"

bool CheckForCommentLine(char* token) {
    // check if token is empty, comment line, or null
    if ((token != NULL) && (token[0] == '\0')) {
        printf("token is empty\n");
    }
    if (token == NULL) printf("token is null\n");   
    return (
            (token == NULL) ||
            ((token != NULL) && (token[0] == '\0')) ||  
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
    printf("%s occurs %d times \n", VAR_EXPR, exprCount);
    return exprCount;
}

void Destructor(struct command* commandStruct) {
    if (commandStruct->cmd != NULL) free(commandStruct->cmd);
    if (commandStruct->inputFile != NULL) free(commandStruct->inputFile);
    if (commandStruct->outputFile != NULL) free(commandStruct->outputFile);
    
    int i = 0;
    while (i < commandStruct->argListSize) {
        free(commandStruct->argList[i]);
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
                expTokenIndex++;
                tokenIndex++;
                charsRead++;
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
                expTokenIndex++;
                tokenIndex++;
                charsRead++;
            }
        }
        charsRead = 0;
    }
    free(pidString);
    return;
}

void GetCommandInput(char** userInputAddr) {
    char* input = NULL;
    size_t inputLength = 0;
    // freed in main
    *userInputAddr = calloc(MAX_CMD_LN_CHRS + 1, sizeof(char));
    printf("%s ", PROMPT);
    fflush(stdout);
    getline(&input, &inputLength, stdin);
    input[strcspn(input, "\n")] = '\0';
    strcpy(*userInputAddr, input);
    free(input);
    return;
}

void GetPidString(char** pidStringAddr) {
    pid_t processID = getpid();
    // get the number of bytes needed for pid string
    int procIDLength = snprintf(NULL, 0, "%u", processID);
    // freed in expand variable
    *pidStringAddr = calloc(procIDLength + 1, sizeof(char));
    sprintf(*pidStringAddr, "%u", processID);
    printf("The pid string is %s\n", *pidStringAddr);
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
    if (CheckForCommentLine(lineToken)) return;
    // freed in main, as well as all parts that are dynamic
    *userStructAddr = calloc(1, sizeof(struct command));
    (*userStructAddr)->argListSize = 0;

    while (lineToken != NULL) {
        printf("Before expansion(if needed): %s\n", lineToken);
        expansionCount = CheckForVariableExpression(lineToken);
        if (expansionCount > 0) {
            ExpandVariableExpression(expansionCount, lineToken, &expToken);
            lineToken = expToken;
            printf("After expansion(if needed): %s\n", lineToken);
        }
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
            // string coped over in while loop
            free(expToken);
            expToken = NULL;
        }
        // continue parsing
        lineToken = strtok_r(NULL, " \r\t\n\v\f", &linePtr);
    }
    return;
}

