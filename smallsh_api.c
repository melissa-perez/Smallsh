#include "smallsh_api.h"


void GetCommandInput(char** userInputAddr) {
    size_t inputLength = 0;
    printf("%s", PROMPT);
    fflush(stdout);
    *userInputAddr = calloc(MAX_CMD_LN_CHRS, sizeof(char));
    getline(userInputAddr, &inputLength, stdin);
    *(*userInputAddr + strcspn(*userInputAddr, "\n")) = '\0';
    return;
}


void ProcessCommandLine(char* userCommandLine,
    struct command** userStructAddr) {
    *userStructAddr = calloc(1, sizeof(struct command));
    char* linePtr = NULL;
    char* lineToken = strtok_r(userCommandLine, " ", &linePtr);

    while (lineToken != NULL) {
        printf("\nParsed: %s\n", lineToken);

        if ((strcmp(lineToken, "&") == 0) &&
            (strcmp(linePtr, "") == 0)) {
            (*userStructAddr)->isBackgroundProc = true;
        }
        lineToken = strtok_r(NULL, " ", &linePtr);
    }
    return;
}



/*// For movie title, allocate space -- length unknown
currMovie->title = calloc(strlen(lineToken) + 1, sizeof(char));
strcpy(currMovie->title, lineToken);

// Advance token to movie year, 4 digit between 1900 and 2021
lineToken = strtok_r(NULL, ",", &linePtr);
currMovie->year = atoi(lineToken);

// Advance token to movie languages, concurrently process langs
// Always enclosed in [], separated by ;
// Max number: 5, max length: 20, case-sensitive
lineToken = strtok_r(NULL, ",", &linePtr);
langTokens = calloc(strlen(lineToken) + 1, sizeof(char));
strcpy(langTokens, lineToken);
lineToken = strtok_r(langTokens, "[];", &langPtr);

while (lineToken != NULL && index < MAX_LANGS) {
    strcpy((currMovie->languages)[index], lineToken);
    index += 1;
    lineToken = strtok_r(NULL, "[];", &langPtr);
}

// Advance back to processing currLine, movie ratings, between 1 and 10
// Can be decimal
lineToken = strtok_r(NULL, "\n", &linePtr);
currMovie->rating = strtod(lineToken, NULL);

// Assign next movie to null, free up langTokens(copied over)
currMovie->next = NULL;
free(langTokens);

return currMovie;*/