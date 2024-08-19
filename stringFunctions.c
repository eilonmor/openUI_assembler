#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Duplicate a string */
char* strduppp(const char* str) {
    char* dup = (char*)malloc(strlen(str) + 1);
    if (dup == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    strcpy(dup, str);
    return dup;
}


/*remove mulipal spases. example: change "helo    world!" to "helo world!"*/
void removeExtraSpaces(char *str) {
    int i, j = 0;
    int len = strlen(str);
    int spaceFound = 0;

    for (i = 0; i < len; i++) {
        if (!isspace((unsigned char)str[i])) {
            if (spaceFound) {
                str[j++] = ' ';
                spaceFound = 0;
            }
            str[j++] = str[i];
        } else {
            spaceFound = 1;
        }
    }
    
    str[j] = '\0';
}

	
/*remove leading spaces and trailing spaces from string. example: "   helo world!  " to "helo world!"*/
void removeLeadingSpacesAndTrailing(char *str) {
    int start = 0;
    int end = strlen(str) - 1;
    int i;
    /*Remove leading spaces*/
    while (isspace((unsigned char)str[start])) {
        start++;
    }
    /*Remove trailing spaces*/
    while (end >= start && isspace((unsigned char)str[end])) {
        end--;
    }
    /*Shift the trimmed string to the beginning of the array*/
    for (i = start; i <= end; i++) {
        str[i - start] = str[i];
    }
    /*Null-terminate the trimmed string*/
    str[end - start + 1] = '\0';
}

/* Remove all spaces from a string */
void removeSpaces(char *str) {
    int i, count = 0;
    for (i = 0; str[i]; i++) {
        if (!isspace(str[i])) {
            str[count++] = str[i];
        }
    }
    str[count] = '\0';
}
/* Takes a filename and a new file extension (file type) as inputs, and returns a new filename with the specified file extension*/
char *convertFile(char *filename, char *newFileType) {
    char *dot;
    char *newFileName;
    int baseLength;
    dot = strrchr(filename, '.');
    if (dot) {
        baseLength = dot - filename;
    } else {
        baseLength = strlen(filename);
    }
    newFileName = (char *)malloc(baseLength + strlen(newFileType) + 1);
    if (!newFileName) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    strncpy(newFileName, filename, baseLength);
    newFileName[baseLength] = '\0';
    strcat(newFileName, newFileType);

    return newFileName;
}

int hasMultipalCommas(char *str) {
    int len = strlen(str);
    int i;
    /*Check for trailing comma*/ 
    if (len > 0 && str[len - 1] == ',') {
        return 1;  /* Trailing comma found*/
    }

    for (i = 0; i < len - 1; i++) {
        if (str[i] == ',') {
            /* Skip any spaces following the comma*/
            int j = i + 1;
            while (j < len && isspace((char)str[j])) {
                j++;
            }
            /* Check if the next non-space character is also a comma*/
            if (j < len && str[j] == ',') {
                return 1;  /* Found consecutive commas*/
            }
        }
    }
    
    
    return 0;  /* No consecutive commas or trailing comma found found*/
}