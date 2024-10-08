#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stringFunc.h"

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
    if (name == NULL) {
        fprintf(stderr, "Error: name is NULL in insertOrUpdateHashTable\n");
        return;
    }
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
	/*Append data*/
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
    }else if (strcmp(key, ".symbole") == 0) {
        return 5;
    }
    return size-1;
}

/* Function to insert or update data in the hash table */
void insertOrUpdateHashTable(HashTable* hash_table, char* key, char* name, char* data) {
    int index = hashFunction(key, hash_table->size);
    insertOrUpdateLinkedList(&hash_table->table[index], name, data);
}

/* Function to search for a name in the hash table */
char* searchHashTable(HashTable* hash_table, char* name, int index) {
    LinkedList* list;
    Node* current;
    int i;
    if (index < 0 || index >= hash_table->size) {
        /* Search in all indexes */
        for (i = 0; i < hash_table->size; ++i) {
            list = &hash_table->table[i];
            current = list->head;
            while (current != NULL) {
                if (strcmp(current->name, name) == 0) {
                    return current->data;
                }
                current = current->next;
            }
        }
    } else {
        /* Search in the specified index */
        list = &hash_table->table[index];
        current = list->head;
        while (current != NULL) {
            if (strcmp(current->name, name) == 0) {
                return current->data;
            }
            current = current->next;
        }
    }
    return "-1"; /* Return -1 if not found */
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
