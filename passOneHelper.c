#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bitControler.h"
#include "dataStructuresFunc.h"
#include "stringFunc.h"
#define NUM_OF_NAMES 16
typedef struct {
    char name[5];
    int value;
} NameValuePair;

NameValuePair opcodeNameValue[NUM_OF_NAMES] = {
    {"mov", 0}, {"cmp", 1}, {"add", 2}, {"sub", 3},
    {"lea", 4}, {"clr", 5}, {"not", 6}, {"inc", 7},
    {"dec", 8}, {"jmp", 9}, {"bne", 10}, {"red", 11},
    {"prn", 12}, {"jsr", 13}, {"rts", 14}, {"stop", 15}
};
/**/
NameValuePair opcodeSizeWord[NUM_OF_NAMES] = {
    {"mov", 3}, {"cmp", 3}, {"add", 3}, {"sub", 3},
    {"lea", 3}, {"clr", 2}, {"not", 2}, {"inc", 2},
    {"dec", 2}, {"jmp", 2}, {"bne", 2}, {"red", 2},
    {"prn", 2}, {"jsr", 2}, {"rts", 1}, {"stop", 1}
};
/*return opcode value*/
int getOpcode(char *name) {
    int i;
    for (i = 0; i < NUM_OF_NAMES; i++) {
        if (strcmp(opcodeNameValue[i].name, name) == 0) {
            return opcodeNameValue[i].value;
        }
    }
    return -1;
}
/*return how much memmory each comand are taken (in most cases)*/
int getOpcodeSizeWord(char *name) {
    int i;
    for (i = 0; i < NUM_OF_NAMES; i++) {
        if (strcmp(opcodeSizeWord[i].name, name) == 0) {
            return opcodeSizeWord[i].value;
        }
    }
    return -1;
}

/* Function to check if a string is a valid decimal number */
int is_decimal_number(char *str) {
    int i;
    /* Check each character in the string */
    for (i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) {
            return 0;  /* Not a decimal number */
        }
    }
    return 1;  /* Valid decimal number */
}

/* Function to check the string pattern and return the corresponding value */
int getOpcodeMethod(char *oprand, HashTable* hash_table) {
    if (oprand[0] == '#') {
        /* Check if the rest of the string is a decimal number */
        if (is_decimal_number(oprand + 1)) {
            return 0;  /* String starts with # and followed by a decimal number */
        }
        /*1 is for data, 2 is for string, 4 is for exstern. Check if the oprand are in symbole table*/
    } else if (strcmp(searchHashTable(hash_table,oprand,1),"-1") != 0 || strcmp(searchHashTable(hash_table,oprand,2),"-1") != 0 || strcmp(searchHashTable(hash_table,oprand,4),"-1") != 0){
        return 1;
    }else if (oprand[0] == '*') {
        /* Check if the rest of the string is "r1" to "r7" */
        if (strlen(oprand) == 3 && oprand[1] == 'r' && oprand[2] >= '1' && oprand[2] <= '7') {
            return 2;  /* String starts with * and followed by r1 to r7 */
        }
    } else if (strlen(oprand) == 2 && oprand[0] == 'r' && oprand[1] >= '1' && oprand[1] <= '7') {
        return 3;  /* String is exactly "r1" to "r7" */
    }

    return -1;  /* String does not match any of the patterns */
}

/*sets the value in a cell by addressing method*/
void setByGetOpcodeMethod(ram_array array, int cell_index, int opcodeMethod,  char *arg, HashTable* hash_table, int registerIndexStart, int registerIndexEnd){
    int stringToInt;
    removeSpaces(arg);
    switch (opcodeMethod)
    {
    case 0:
        /*for A*/
        set_bit_in_cell(array, cell_index,0,1);
        /*set the number*/
        set_cell_value_by_method(array,cell_index,atoi(arg + 1));
        break;
    case 1:
        if (strcmp(searchHashTable(hash_table,arg,1),"-1") != 0 || strcmp(searchHashTable(hash_table,arg,2),"-1") != 0 )
        {
            /*its symbole that difined in this file so it "R"*/
            set_bit_in_cell(array, cell_index,1,1);
            set_cell_value_by_method(array,cell_index,atoi(searchHashTable(hash_table,arg,-1)));
        }else{
            /*is exstern*/
            set_bit_in_cell(array, cell_index,0,1);
        }
        break;
    case 2:
	stringToInt = arg[strlen(arg) - 1] - '0';
        setOpcodeBit(&array[cell_index],stringToInt,registerIndexStart,registerIndexEnd);
        break;
    case 3:
	stringToInt = arg[strlen(arg) - 1] - '0';        
	setOpcodeBit(&array[cell_index],stringToInt,registerIndexStart,registerIndexEnd);
        break;        
    default:
        /*this error is for me.*/
        fprintf(stderr,"case not exsists.\n");
        break;
    }
}

int opcodeHelper(char *sourceCodeOreder, set binaryMachineCode, char *methodName, int countRowInFile, int *methodValue, int howMuchMethodToCheck,HashTable* hash_table){
    char *token;
    int targetStartIndex = 3, targetEndIndex = 6;
    int errorHapend = 0, opcodeExists, i;
    int matchFound = 0;
    /*get first oprand*/
    token = strtok(sourceCodeOreder, ",");
    /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
    opcodeExists = getOpcodeMethod(token,hash_table);
    for (i = 0; i < howMuchMethodToCheck; i++) {
        if (opcodeExists == methodValue[i]) {
            matchFound = 1;
            break;
        }
    }
    if (matchFound){
        fprintf(stderr,"Error: add cant get method %d for target addressing at line number %d\n",methodValue[i],countRowInFile);
        errorHapend++;
    }
    if (opcodeExists != -1 && !(errorHapend))
    {
        /*write the target code*/
        setOpcodeBit(binaryMachineCode,opcodeExists+targetStartIndex, targetStartIndex,targetEndIndex);
    }            
    token = strtok(NULL, ",");
    /*if there is more than 1 argument so print error*/
    if (token != NULL){
        fprintf(stderr,"Error: %s must get 1 arguments but you provided more than 1: '%s' at line number %d\n",methodName,token,countRowInFile);
        errorHapend++;
    }
    return errorHapend;
}

int opcodeExeForPassTwo(int value, char *sourceCodeOreder, int countRowInFile,HashTable* hash_table, int *L,ram_array array, int cell_index) {
    int targetStartIndex = 3, targetEndIndex = 6, sourceStartIndex = 7, sourceEndIndex = 10;
    int registerTargetStart = 3, registerTargetEnd = 5, registerSourceStart = 6, registerSourceEnd = 8;
    int errorHapend = 0, opcodeExists, getOpcodeMethodSystemNumber;
    int methodToCheck[4];
    int stringToInt;
    char *token;
    char *firstArg;
    char *secondArg;
    set binaryMachineCode;
    errorHapend = hasMultipalCommas(sourceCodeOreder);
    printf("value: %d, sourceCodeOreder: %s, countRowInFile: %d, L: %d, cell_index: %d\n",value,sourceCodeOreder,countRowInFile,L,cell_index);
    if (errorHapend)
    {
        fprintf(stderr,"Invalid commas: there is multipal commas at line number %d\n",countRowInFile);
        errorHapend++;
    }
    switch (value) {
        /*mov*/
        case 0:
            L++;
        /*get first oprand*/
            token = strtok(sourceCodeOreder, ",");
            /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
            opcodeExists = getOpcodeMethod(token,hash_table);
            if (opcodeExists != -1 && !(errorHapend))
            {
                /*set the system surce oprand*/
                setOpcodeBit(binaryMachineCode,opcodeExists+sourceStartIndex, sourceStartIndex,sourceEndIndex);
                firstArg = strduppp(token);
            }            
            token = strtok(NULL, ",");
            /*if there is only 1 argument so print error*/
            if (token == NULL){
                fprintf(stderr,"Error: mov must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
            }else
            {
                /*for the target code*/
                opcodeExists = getOpcodeMethod(token, hash_table);
                if (getOpcodeMethod(token, hash_table) == 0){
                    fprintf(stderr,"Error: mov cant get method 0 for target addressing at line number %d\n",countRowInFile);
                    errorHapend++;
                }else{
                    /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
                    if (opcodeExists != -1 && !(errorHapend))
                    {
                        /*set the system target oprand*/
                        setOpcodeBit(binaryMachineCode, getOpcodeMethod(token, hash_table)+targetStartIndex, targetStartIndex,targetEndIndex);
                        secondArg = strduppp(token);
                    }
                }
                /*move can get only 2 argument*/
                token = strtok(NULL, ",");
                if (token != NULL){
                fprintf(stderr,"Error: mov must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
                }
            }
            /*check if there is 2 registers, if has save it in one word*/
            getOpcodeMethodSystemNumber = getOpcodeMethod(firstArg,hash_table);
            if (getOpcodeMethodSystemNumber == 2 ||getOpcodeMethodSystemNumber == 3)
            {
                L++;
                cell_index++;
                removeSpaces(firstArg);
		stringToInt = firstArg[strlen(firstArg) - 1] - '0'; 
                setOpcodeBit(binaryMachineCode,stringToInt,registerSourceStart,registerSourceEnd);
                getOpcodeMethodSystemNumber = getOpcodeMethod(secondArg,hash_table);
                if (getOpcodeMethodSystemNumber == 2 ||getOpcodeMethodSystemNumber == 3)
                {
                    /*1 is transletion of command and the second is regiters*/
                    /*todo: add set for 2 r*/
		    stringToInt = secondArg[strlen(secondArg) - 1] - '0';
                    setOpcodeBit(binaryMachineCode,stringToInt,registerTargetStart,registerTargetEnd);
                }else if (getOpcodeMethodSystemNumber)
                {
                    L++;
                    cell_index++;
                    setByGetOpcodeMethod(array,cell_index,getOpcodeMethodSystemNumber,secondArg,hash_table,registerTargetStart,registerTargetEnd);
                }
                /*the first arg is not register*/
            }else{
                L++;
                cell_index++;
                setByGetOpcodeMethod(array,cell_index,getOpcodeMethodSystemNumber,firstArg,hash_table,registerSourceStart,registerSourceEnd);
                L++;
                cell_index++;
                getOpcodeMethodSystemNumber = getOpcodeMethod(secondArg,hash_table);
                setByGetOpcodeMethod(array,cell_index,getOpcodeMethodSystemNumber,secondArg,hash_table,registerTargetStart,registerTargetEnd);
            }            
            break;
            /*cmp*/
        case 1:
           /*get first oprand*/
            token = strtok(sourceCodeOreder, ",");
            /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
            opcodeExists = getOpcodeMethod(token, hash_table);
            if (opcodeExists != -1 && !(errorHapend))
            {
                /*write the surce code*/
                setOpcodeBit(binaryMachineCode,opcodeExists+sourceStartIndex, sourceStartIndex,sourceEndIndex);
            }            
            token = strtok(NULL, ",");
            /*if there is only 1 argument so print error*/
            if (token == NULL){
                fprintf(stderr,"Error: cmp must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
            }else
            {
            /*for the target code*/
                opcodeExists = getOpcodeMethod(token, hash_table);
                /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
                if (opcodeExists != -1 && !(errorHapend))
                {
                    setOpcodeBit(binaryMachineCode, getOpcodeMethod(token, hash_table)+targetStartIndex, targetStartIndex,targetEndIndex);
                }
                /*cmp can get only 2 argument*/
                token = strtok(NULL, ",");
                if (token != NULL){
                fprintf(stderr,"Error: cmp must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
                }
            }
            break;
            /*add*/
        case 2:
            /*get first oprand*/
            token = strtok(sourceCodeOreder, ",");
            /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
            opcodeExists = getOpcodeMethod(token, hash_table);
            if (opcodeExists != -1 && !(errorHapend))
            {
                /*write the surce code*/
                setOpcodeBit(binaryMachineCode,opcodeExists+sourceStartIndex, sourceStartIndex,sourceEndIndex);
            }            
            token = strtok(NULL, ",");
            /*if there is only 1 argument so print error*/
            if (token == NULL){
                fprintf(stderr,"Error: add must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
            }else
            {
                /*for the target code*/
                opcodeExists = getOpcodeMethod(token, hash_table);
                if (getOpcodeMethod(token, hash_table) == 0){
                    fprintf(stderr,"Error: add cant get method 0 for target addressing at line number %d\n",countRowInFile);
                    errorHapend++;
                }else{
                    /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
                    if (opcodeExists != -1 && !(errorHapend))
                    {
                        setOpcodeBit(binaryMachineCode, getOpcodeMethod(token, hash_table)+targetStartIndex, targetStartIndex,targetEndIndex);
                    }
                }
                /*add must get only 2 argument*/
                token = strtok(NULL, ",");
                if (token != NULL){
                fprintf(stderr,"Error: add must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
                }
            }
            break;
            /*sub*/
        case 3:
            /*get first oprand*/
            token = strtok(sourceCodeOreder, ",");
            /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
            opcodeExists = getOpcodeMethod(token, hash_table);
            if (opcodeExists != -1 && !(errorHapend))
            {
                /*write the surce code*/
                setOpcodeBit(binaryMachineCode,opcodeExists+sourceStartIndex, sourceStartIndex,sourceEndIndex);
            }            
            token = strtok(NULL, ",");
            /*if there is only 1 argument so print error*/
            if (token == NULL){
                fprintf(stderr,"Error: sub must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
            }else
            {
                /*for the target code*/
                opcodeExists = getOpcodeMethod(token, hash_table);
                if (getOpcodeMethod(token, hash_table) == 0){
                    fprintf(stderr,"Error: sub cant get method 0 for target addressing at line number %d\n",countRowInFile);
                    errorHapend++;
                }else{
                    /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
                    if (opcodeExists != -1 && !(errorHapend))
                    {
                        setOpcodeBit(binaryMachineCode, getOpcodeMethod(token, hash_table)+targetStartIndex, targetStartIndex,targetEndIndex);
                    }
                }
                /*sub can get only 2 argument*/
                token = strtok(NULL, ",");
                if (token != NULL){
                fprintf(stderr,"Error: sub must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
                }
            }
            break;
            /*lea*/
        case 4:
            /*get first oprand*/
            token = strtok(sourceCodeOreder, ",");
            /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
            opcodeExists = getOpcodeMethod(token, hash_table);
            if (opcodeExists == 0 || opcodeExists == 2 || opcodeExists == 3)
            {
                fprintf(stderr,"Error: lea cant get method 0 or 2 or 3 for source addressing at line number %d\n",countRowInFile);
                errorHapend++;
            }else if (opcodeExists != -1 && !(errorHapend))
            {
                /*write the surce code*/
                setOpcodeBit(binaryMachineCode,opcodeExists+sourceStartIndex, sourceStartIndex,sourceEndIndex);
            }            
            token = strtok(NULL, ",");
            /*if there is only 1 argument so print error*/
            if (token == NULL){
                fprintf(stderr,"Error: lea must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
            }else
            {
                /*for the target code*/
                opcodeExists = getOpcodeMethod(token, hash_table);
                if (getOpcodeMethod(token, hash_table) == 0){
                    fprintf(stderr,"Error: lea cant get method 0 for target addressing at line number %d\n",countRowInFile);
                    errorHapend++;
                }else{
                    /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
                    if (opcodeExists != -1 && !(errorHapend))
                    {
                        setOpcodeBit(binaryMachineCode, getOpcodeMethod(token, hash_table)+targetStartIndex, targetStartIndex,targetEndIndex);
                    }
                }
                /*lea can get only 2 argument*/
                token = strtok(NULL, ",");
                if (token != NULL){
                fprintf(stderr,"Error: lea must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
                }
            }
            break;
            /*clr*/
        case 5:
            methodToCheck[0] = 0;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"clr",countRowInFile,methodToCheck,1,hash_table);
            break;
        /*not*/
        case 6:
            methodToCheck[0] = 0;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"not",countRowInFile,methodToCheck,1,hash_table);
            break;
        /*inc*/
        case 7:
            methodToCheck[0] = 0;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"inc",countRowInFile,methodToCheck,1,hash_table);
            break;
        /*dec*/
        case 8:
            methodToCheck[0] = 0;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"dec",countRowInFile,methodToCheck,1,hash_table);
            break;
        /*jmp*/
        case 9:
            methodToCheck[0] = 0;
            methodToCheck[1] = 3;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"jmp",countRowInFile,methodToCheck,2,hash_table);
            break;
        /*bne*/
        case 10:
            methodToCheck[0] = 0;
            methodToCheck[1] = 3;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"bne",countRowInFile,methodToCheck,2,hash_table);
            break;
        /*red*/
        case 11:
            methodToCheck[0] = 0;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"red",countRowInFile,methodToCheck,1,hash_table);
            break;
        /*prn*/
        case 12:
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"prn",countRowInFile,methodToCheck,0,hash_table);
            break;
        /*jsr*/
        case 13:
            methodToCheck[0] = 0;
            methodToCheck[1] = 3;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"jsr",countRowInFile,methodToCheck,2,hash_table);
            break;
        /*rts*/
        case 14:
             methodToCheck[0] = 0;
    	     methodToCheck[1] = 1;
    	     methodToCheck[2] = 2;
    	     methodToCheck[3] = 3;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"rts",countRowInFile,methodToCheck,4,hash_table);
            break;
        /*stop*/
        case 15:
            methodToCheck[0] = 0;
    	    methodToCheck[1] = 1;
    	    methodToCheck[2] = 2;
	    methodToCheck[3] = 3;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"stop",countRowInFile,methodToCheck,4,hash_table);
            break;
        default:
            fprintf(stderr,"Invalid value\n");
            break;
    }
    return errorHapend;
}