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

/* Define the Node structure */
typedef struct Node {
    char* name;  /* Name of the macro */
    char* data;  /* Data (dynamic string) */
    struct Node* next;
} Node;

/* Define the LinkedList structure */
typedef struct {
    Node* head;
} LinkedList;

/* Define the HashTable structure */
typedef struct {
    int size;
    LinkedList* table;
} HashTable;

/* Function prototypes */
Node* createNode(char* name, char* data);
void initLinkedList(LinkedList* list);
void insertOrUpdateLinkedList(LinkedList* list, char* name, char* data);
void printLinkedList(LinkedList* list);
void initHashTable(HashTable* hash_table, int size);
int hashFunction(char* key, int size);
void insertOrUpdateHashTable(HashTable* hash_table, char* key, char* name, char* data);
void printHashTable(HashTable* hash_table);
void freeLinkedList(LinkedList* list);
void freeHashTable(HashTable* hash_table);
void removeExtraSpaces(char *str);
void removeLeadingSpacesAndTrailing(char *str);
void removeSpaces(char *str);
void initConstantNames(void);
void addConstantName(char *name);
int argStrContainConstantNames(char *argStr);
void freeConstantNames(void);
char* strduppp(const char* str);

/* Create a new node with the given name and data */
Node* createNode(char* name, char* data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    new_node->name = strduppp(name);
    new_node->data = strduppp(data);
    new_node->next = NULL;
    return new_node;
}

/* Initialize a linked list */
void initLinkedList(LinkedList* list) {
    list->head = NULL;
}

/* Insert or update a node in the linked list */
void insertOrUpdateLinkedList(LinkedList* list, char* name, char* data) {
    Node* current = list->head;
    Node* new_node;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
	/*Append data instead of overwriting */
            char* new_data = (char*)malloc(strlen(current->data) + strlen(data) + 1);
            if (new_data == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                exit(1);
            }
            strcpy(new_data, current->data);
            strcat(new_data, "\n");
            strcat(new_data, data);
            free(current->data);
            current->data = new_data;
            return;
        }
        current = current->next;
    }

    new_node = createNode(name, data);
    new_node->next = list->head;
    list->head = new_node;
}

/* Print all nodes in the linked list */
void printLinkedList(LinkedList* list) {
    Node* current = list->head;
    while (current != NULL) {
        printf("Macro Name: %s, Data: %s\n", current->name, current->data);
        current = current->next;
    }
}

/* Initialize the hash table with the given size */
void initHashTable(HashTable* hash_table, int size) {
    int i;
    hash_table->size = size;
    hash_table->table = (LinkedList*)malloc(size * sizeof(LinkedList));
    if (hash_table->table == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    for (i = 0; i < size; ++i) {
        initLinkedList(&hash_table->table[i]);
    }
}

/* Hash function to determine the index for a key */
int hashFunction(char* key, int size) {
    if (strcmp(key, "macr") == 0) {
        return 0;
    } else if (strcmp(key, ".data") == 0) {
        return 1;
    } else if (strcmp(key, ".string") == 0) {
        return 2;
    } else if (strcmp(key, ".entry") == 0) {
        return 3;
    } else if (strcmp(key, ".extern") == 0) {
        return 4;
    }
/*Default case*/
    return size - 1; 
}

/* Function to insert or update data in the hash table */
void insertOrUpdateHashTable(HashTable* hash_table, char* key, char* name, char* data) {
    int index = hashFunction(key, hash_table->size);
    insertOrUpdateLinkedList(&hash_table->table[index], name, data);
}

/* Print all entries in the hash table */
void printHashTable(HashTable* hash_table) {
    int i;
    for (i = 0; i < hash_table->size; ++i) {
        printf("Index %d:\n", i);
        printLinkedList(&hash_table->table[i]);
    }
}

/* Free all nodes in a linked list */
void freeLinkedList(LinkedList* list) {
    Node* current = list->head;
    Node* temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp->name);
        free(temp->data);
        free(temp);
    }
}

/* Function to free the hash table */
void freeHashTable(HashTable* hash_table) {
    int i;
    for (i = 0; i < hash_table->size; ++i) {
        freeLinkedList(&hash_table->table[i]);
    }
    free(hash_table->table);
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

/*init constantNames to all defult saved names.
    {
        "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
        "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
        "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
        "macr", ".extern", ".entry", "label", ".data", ".string", "macr"
    };
*/
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

/* Main function */
int main() {
    FILE* fptr;
    FILE* fptrAs;
    char *token;
    int flagIsMacro;
    int countRowInFile;
    char *tempMacrName;
    char *tempName;
    char *foundContent;
    char *endMacr = "endmacr";
    char content[maxCharsInRow];
    char contentCopy[maxCharsInRow];
    HashTable hash_table;

    fptr = fopen("input.txt", "r");
    fptrAs = fopen("inputFileAfterMacroHandle.as", "w");
    if (fptr == NULL || fptrAs == NULL) {
        perror("Failed to open file");
        return 1;
    }

    countRowInFile = 1;
    initConstantNames();
    initHashTable(&hash_table, 5);
    /*get each row in the file.*/
    while (fgets(content, maxCharsInRow, fptr)) {
        removeLeadingSpacesAndTrailing(content);
        removeExtraSpaces(content);
        strcpy(contentCopy, content);

        if (content[0] == ';') {
            continue;
        }
	printf("%s \n",content);
        /*get the first word*/
        token = strtok(content, " ");
        if (token != NULL) {
            if (token[strlen(token) - 1] == ':') {
                if (argStrContainConstantNames(token)) {
                    fprintf(stderr, "Invalid label name: `%s` is allready used or it assembler consts names ! at line number: %d.\n", token, countRowInFile);
                    return 1;
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
                        fprintf(stderr, "Invalid macro name: macr name must be non-saved name but you passed: `%s` at line number: %d.\n", token, countRowInFile);
                        return 1;
                    }
                    /*check if macr name is valid (macr name can be only one word.)*/
                    tempMacrName = strduppp(token);
                    token = strtok(NULL, " ");
                    if (token != NULL) {
                        fprintf(stderr, "Invalid macro name: macr name can be only one word without spaces but you provided also `%s` at line number: %d.\n", token, countRowInFile);
                        return 1;
                    } else {
                    /*check if endmacr is valide*/
                        addConstantName(tempMacrName);
                        flagIsMacro = 1;
                    }
                }
            } else {
		/*check if has endmacr in this line*/
                foundContent = strstr(endMacr, contentCopy);
		/*if the first word is endmacr*/
                if (foundContent && strcmp(token, endMacr) == 0) {
                    token = strtok(NULL, " ");
 		/*check if exsist word or char after the first word*/
                    if (token != NULL) {
                        fprintf(stderr, "Invalid endmacr: at line number: %d. `endmacr` should be the only command in the line\n", countRowInFile);
                    } else {
			/*if endmacr is valid change flag to false*/
                        flagIsMacro = 0;
                    }
		/*if has endmacr in line but has a word or symbole before*/
                } else if (foundContent) {
                    fprintf(stderr, "Invalid endmacr: `endmacr` should be the only command on the line. eror happened at line number: %d.\n", countRowInFile);
                }
                if (flagIsMacro) {
                    insertOrUpdateHashTable(&hash_table, "macr", tempMacrName, contentCopy);
                }
            }
        }
        countRowInFile++;
    }

    printHashTable(&hash_table);
    freeConstantNames();
    freeHashTable(&hash_table);
    fclose(fptr);
    fclose(fptrAs);

    return 0;
}


