#include "smallsh_api.h"


void GetCommandInput(char** userInputAddr) {
    int inputLength = 0;
    *userInputAddr = calloc(MAX_CMD_LN_CHRS, sizeof(char));
    printf("%s", PROMPT);
    fflush(stdout);
    getline(userInputAddr, &inputLength, stdin);
    
    //*(*userInputAddr + strcspn(*userInputAddr, "\n")) = '\0';
    return;
}


void ProcessCommandLine(char* userCommandLine,
    struct command** userStructAddr) {
    // max length of characters for command line is 2048
    if (strlen(userCommandLine) > MAX_CMD_LN_CHRS) {
        return;
    }
    char* linePtr = NULL;
    char* lineToken = strtok_r(userCommandLine, " \r\t\n\v\f", &linePtr);
    char** commandArray = NULL;
    // check if line is empty
    if (lineToken == NULL) return; //Skip blank line

    return;
}

bool CheckForCommentLine(char* token) {
    return ((strcmp(token, "") == 0) ||
        (strcmp(token, "#") == 0));
}

