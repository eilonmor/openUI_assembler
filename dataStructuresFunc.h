#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

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
Node* createNode(char* name, char* data);

/* Initialize a linked list */
void initLinkedList(LinkedList* list);

/* Insert or update a node in the linked list */
void insertOrUpdateLinkedList(LinkedList* list, char* name, char* data);

/* Print all nodes in the linked list */
void printLinkedList(LinkedList* list);

/* Initialize the hash table with the given size */
void initHashTable(HashTable* hash_table, int size);

/* Hash function to determine the index for a key */
int hashFunction(char* key, int size);

/* Function to insert or update data in the hash table */
void insertOrUpdateHashTable(HashTable* hash_table, char* key, char* name, char* data);

/* Function to search for a name in the hash table */
char* searchHashTable(HashTable* hash_table, char* name, int index);

/* Print all entries in the hash table */
void printHashTable(HashTable* hash_table);

/* Free all nodes in a linked list */
void freeLinkedList(LinkedList* list);

/* Function to free the hash table */
void freeHashTable(HashTable* hash_table);

/* Function to add value to each node in a specific index of the hash table */
void addValueToHashTable(HashTable* hash_table, int index, int value_to_add);

#endif /* DATA_STRUCT_H */
