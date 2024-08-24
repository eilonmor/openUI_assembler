#include "bitControler.h"
#include "dataStructuresFunc.h"
#include "passOneHelper.h"
#ifndef PASS_TWO_HELPER_H
#define PASS_TWO_HELPER_H


extern NameValuePair opcodeNameValue[NUM_OF_NAMES];
extern NameValuePair opcodeSizeWord[NUM_OF_NAMES];

/*return how much memmory each comand are taken (in most cases)*/
int getOpcodeSizeWord(char *name); 

/*sets the value in a cell by addressing method*/
void setByGetOpcodeMethod(ram_array array, int cell_index, int opcodeMethod,  char *arg, HashTable* hash_table, int registerIndexStart, int registerIndexEnd);

int opcodeHelperforPassTWo(char *sourceCodeOreder, ram_array array, int cell_index, char *methodName, int countRowInFile, int *methodValue, int howMuchMethodToCheck,HashTable* hash_table);

int opcodeExeForPassTwo(int value, char *sourceCodeOreder, int countRowInFile,HashTable* hash_table, int *L,ram_array array, int cell_index);
#endif /* STRING_FUNC_H */
