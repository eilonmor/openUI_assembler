#include "dataStructuresFunc.h"
#include "stringFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define maxCharsInRow 80
#define constantNamesLength 28
#define INITIAL_SIZE 30

char **constantNames = NULL;
int constantNamesSize = 0;
int constantNamesCapacity = INITIAL_SIZE;

void initConstantNames() {
    int i;
    const char *initialNames[] = {
        "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
        "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
        "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
        "macr", ".extern", ".entry", "label", ".data", ".string", "macr"
    };
    int initialNamesCount = sizeof(initialNames) / sizeof(initialNames[0]);

    constantNames = (char **)malloc(INITIAL_SIZE * sizeof(char *));
    if (constantNames == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < initialNamesCount; i++) {
        constantNames[i] = strduppp(initialNames[i]);
        if (constantNames[i] == NULL) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
    }
    constantNamesSize = initialNamesCount;
}

/* Add a new constant name to the array */
void addConstantName(char *name) {
    if (name == NULL) {
        fprintf(stderr, "Error: Name cannot be NULL\n");
        return;
    }

    if (constantNamesSize >= constantNamesCapacity) {
        int newCapacity;
        char **temp;

        newCapacity = constantNamesCapacity * 2;
        if (newCapacity <= constantNamesCapacity) {
            /* Handle overflow case*/
            fprintf(stderr, "Error: Cannot expand constant names array (capacity overflow)\n");
            return;
        }

        temp = (char **)realloc(constantNames, newCapacity * sizeof(char *));
        if (temp == NULL) {
            perror("Failed to allocate memory");
            return;
        }
        constantNames = temp;
        constantNamesCapacity = newCapacity;
    }

    constantNames[constantNamesSize] = strduppp(name);
    if (constantNames[constantNamesSize] == NULL) {
        perror("Failed to allocate memory");
        return;
    }
    constantNamesSize++;
}



/* Check if the given string matches any constant names
    return 1 if there is
    return 0 if not
notes: this function are help to verify valid macr name.
 */
int argStrContainConstantNames(char *argStr) {
    int i;
    for (i = 0; i < constantNamesSize; i++) {
        if (strcmp(argStr, constantNames[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Free all constant names */
void freeConstantNames(void) {
    int i;
    for (i = 0; i < constantNamesSize; i++) {
        free(constantNames[i]);
    }
    free(constantNames);
}

int preAssembler(char* fileName) {
    FILE* fptr;
    FILE* fptrAs;
    char *token;
    int flagIsMacro;
    int errorHapendesCount;
    int countRowInFile;
    int endmacrStauesChange;
    char *tempMacrName;
    char *tempName;
    char *foundContent;
    char *endMacr = "endmacr";
    char *searchHashTableResult;
    char *outputFile;
    char content[maxCharsInRow];
    char contentCopy[maxCharsInRow];
    HashTable hash_table;
    outputFile = convertFile(fileName,".am");
    fptr = fopen(fileName, "r");
    fptrAs = fopen(outputFile, "w");
    if (fptr == NULL || fptrAs == NULL) {
        perror("Failed to open file");
        return 1;
    }
    countRowInFile = 1;
    errorHapendesCount = 0;
    flagIsMacro = 0;
    initConstantNames();
    initHashTable(&hash_table, 5);
    /*get each row in the file.*/
    while (fgets(content, maxCharsInRow, fptr)) {
        removeLeadingSpacesAndTrailing(content);
        removeExtraSpaces(content);
        strcpy(contentCopy, content);
 	endmacrStauesChange = 1;
        if (content[0] == ';') {
            continue;
        }
        /*get the first word*/
        token = strtok(content, " ");
        if (token != NULL) {
            if (token[strlen(token) - 1] == ':') {
                if (argStrContainConstantNames(token)) {
		    errorHapendesCount++;
                    fprintf(stderr, "Invalid label name: `%s` is allready used or it assembler consts names ! at line number: %d.\n", token, countRowInFile);
                } else {
                    tempName = strduppp(token);
                    addConstantName(tempName);
                }
            }
            /*check if the word is macr defintion*/
            if (strcmp(token, "macr") == 0) {
                /*get the macr name*/
                token = strtok(NULL, " ");
                if (token != NULL) {
                    /*check if macr name is valid*/
                    if (argStrContainConstantNames(token)) {
			errorHapendesCount++;
                        fprintf(stderr, "Invalid macro name: macr name must be non-saved name but you passed: `%s` at line number: %d.\n", token, countRowInFile);
                    }
                    /*check if macr name is valid (macr name can be only one word.)*/
                    tempMacrName = strduppp(token);
                    token = strtok(NULL, " ");
                    if (token != NULL) {
			errorHapendesCount++;
                        fprintf(stderr, "Invalid macro name: macr name can be only one word without spaces but you provided also `%s` at line number: %d.\n", token, countRowInFile);
                    } else {
                    /*check if endmacr is valide*/
                        addConstantName(tempMacrName);
                        flagIsMacro = 1;
                    }
                }
            } else {
		/*check if has endmacr in this line*/
                foundContent = strstr(contentCopy, endMacr);
		/*if the first word is endmacr*/
                if (foundContent && strcmp(token, endMacr) == 0) {
                    token = strtok(NULL, " ");
 		/*check if exsist word or char after the first word*/
                    if (token != NULL) {
			errorHapendesCount++;
                        fprintf(stderr, "Invalid endmacr: at line number: %d. `endmacr` should be the only command in the line\n", countRowInFile);
                    } else {
			        /*if endmacr is valid change flag to false*/
                        flagIsMacro = 0;
			endmacrStauesChange = 0;
/* todo: delete before send Project
                        searchHashTableResult = searchHashTable(&hash_table, tempMacrName, 0);	
                        fprintf(fptrAs, "%s", searchHashTableResult); */
                    }
		        /*if has endmacr in line but has a word or symbole before*/
                } else if (foundContent) {
		    errorHapendesCount++;
                    fprintf(stderr, "Invalid endmacr: `endmacr` should be the only command on the line. eror happened at line number: %d.\n", countRowInFile);
                }
                else if (flagIsMacro && !(errorHapendesCount)) {
                    insertOrUpdateHashTable(&hash_table, "macr", tempMacrName, contentCopy);
                }
                while (token != NULL && !(errorHapendesCount))
                {
                    searchHashTableResult = searchHashTable(&hash_table, token, 0);
                    if (strcmp(searchHashTableResult,"-1") != 0 && !(flagIsMacro)){
                        fprintf(fptrAs, "%s", searchHashTableResult);
                    }else if(!(flagIsMacro)){
                        fprintf(fptrAs, "%s", token);
                    }
                    token = strtok(NULL, " ");
                    /*if this not the last word append space*/
                    if (token != NULL)
                    {
                        if (!(flagIsMacro) && endmacrStauesChange){fprintf(fptrAs, " ");}
                    }
                    
                }
                if (!(flagIsMacro) && endmacrStauesChange){fprintf(fptrAs, "\n");}
            }
        }
        countRowInFile++;
    }

    printHashTable(&hash_table);
    freeConstantNames();
    freeHashTable(&hash_table);
    free(tempName);
    free(tempMacrName);
    fclose(fptr);
    fclose(fptrAs);
    if (errorHapendesCount > 0) {
        if (remove(outputFile) != 0) {
            perror("Failed to delete output file");
        }
    }
    free(outputFile);

    return errorHapendesCount;
}
