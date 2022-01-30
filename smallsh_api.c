#include "smallsh_api.h"


void GetCommandInput(char** userInputAddr) {
    size_t inputLength = 0;
    *userInputAddr = calloc(MAX_CMD_LN_CHRS + 1, sizeof(char));
    printf("%s ", PROMPT);
    fflush(stdout);
    getline(userInputAddr, &inputLength, stdin);    
    *(*userInputAddr + strcspn(*userInputAddr, "\n")) = '\0';
    return;
}

void ProcessCommandLine(char* userCommandLine,
    struct command** userStructAddr) {
    // max length of characters for command line is 2048
    if (strlen(userCommandLine) > MAX_CMD_LN_CHRS) return;
    
    char* linePtr = NULL;
    char* lineToken = strtok_r(userCommandLine, " \r\t\n\v\f", &linePtr);
    bool firstCommand = true;
    // check to ignore comment/blank lines
    if (CheckForCommentLine(lineToken)) return;

    *userStructAddr = calloc(1, sizeof(struct command));
    (*userStructAddr)->argListSize = 0;

    while (lineToken != NULL) {
        printf("%s\n", lineToken);
        CheckForVariableExpression(lineToken);
        
        if (firstCommand) {
            // at this point we should have a command
            (*userStructAddr)->cmd = calloc(strlen(lineToken) + 1,
                sizeof(char));
            strcpy((*userStructAddr)->cmd, lineToken);
            firstCommand = false;
        }             
        else if (strcmp(lineToken, ">") == 0) {
            lineToken = strtok_r(NULL, " ", &linePtr);
            (*userStructAddr)->outputFile = calloc(strlen(lineToken) + 1,
                sizeof(char));
            strcpy((*userStructAddr)->outputFile, lineToken);
        }
        else if (strcmp(lineToken, "<") == 0) {
            lineToken = strtok_r(NULL, " ", &linePtr);
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
        lineToken = strtok_r(NULL, " \r\t\n\v\f", &linePtr);
    }
    return;
}

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
    while ((token = strstr(token, VAR_EXPR))) {
        exprCount++;
        token += strlen(VAR_EXPR);
    }
    printf("%s occurs %d times \n", VAR_EXPR, exprCount);
    return exprCount;
}