#include "bitControler.h"
#include "dataStructuresFunc.h"
#ifndef PASS_ONE_HELPER_H
#define PASS_ONE_HELPER_H
#define NUM_OF_NAMES 16
typedef struct {
    char name[5];
    int value;
} NameValuePair;

extern NameValuePair opcodeNameValue[NUM_OF_NAMES];
extern NameValuePair opcodeSizeWord[NUM_OF_NAMES];

int getOpcode(char *name);

/*return how much memmory each comand are taken (in most cases)*/
int getOpcodeSizeWord(char *name); 

/* Function to check if a string is a valid decimal number */
int is_decimal_number(char *str);

/* Function to check the string pattern and return the corresponding value */
int getOpcodeMethod(char *oprand,HashTable* hash_table);

/*sets the value in a cell by addressing method*/
void setByGetOpcodeMethod(ram_array array, int cell_index, int opcodeMethod,  char *arg, HashTable* hash_table, int registerIndexStart, int registerIndexEnd);

int opcodeHelper(char *sourceCodeOreder, set binaryMachineCode, char *methodName, int countRowInFile, int *methodValue, int howMuchMethodToCheck,HashTable* hash_table);

int opcodeExeForPassTwo(int value, char *sourceCodeOreder, int countRowInFile,HashTable* hash_table, int *L);
#endif /* STRING_FUNC_H */
