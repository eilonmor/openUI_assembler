#ifndef PASS_ONE_HELPER_H
#define PASS_ONE_HELPER_H
#define NUM_OF_NAMES 16
typedef struct {
    char name[5];
    int value;
} NameValuePair;

extern NameValuePair opcodeNameValue[NUM_OF_NAMES];

int getOpcode(char *name);

/* Function to check if a string is a valid decimal number */
int is_decimal_number(char *str);

/* Function to check the string pattern and return the corresponding value */
int getOpcodeMethod(char *oprand,HashTable* hash_table);

int opcodeHelper(char *sourceCodeOreder, set binaryMachineCode, char *methodName, int countRowInFile, int *methodValue, int howMuchMethodToCheck,HashTable* hash_table);

int opcodeExe(int value, char *sourceCodeOreder, int countRowInFile,HashTable* hash_table, int *L);
#endif /* STRING_FUNC_H */
