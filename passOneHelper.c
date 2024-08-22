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

int getOpcode(char *name) {
    int i;
    for (i = 0; i < NUM_OF_NAMES; i++) {
        if (strcmp(opcodeNameValue[i].name, name) == 0) {
            return opcodeNameValue[i].value;
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
        /* Check if the rest of the string is "r0" to "r7" */
        if (strlen(oprand) == 3 && oprand[1] == 'r' && oprand[2] >= '0' && oprand[2] <= '7') {
            return 2;  /* String starts with * and followed by r0 to r7 */
        }
    } else if (strlen(oprand) == 2 && oprand[0] == 'r' && oprand[1] >= '0' && oprand[1] <= '7') {
        return 3;  /* String is exactly "r0" to "r7" */
    }

    return -1;  /* String does not match any of the patterns */
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

int opcodeExe(int value, char *sourceCodeOreder, int countRowInFile,HashTable* hash_table, int *L) {
    int targetStartIndex = 3, targetEndIndex = 6, sourceStartIndex = 7, sourceEndIndex = 10;
    int errorHapend = 0, opcodeExists, isDubeleRegister = 0;
    int methodToCheck[4];
    char *token;
    set binaryMachineCode;
    *L = 1;
    errorHapend = hasMultipalCommas(sourceCodeOreder);
    if (errorHapend)
    {
        fprintf(stderr,"Invalid commas: there is multipal commas at line number %d\n",countRowInFile);
        errorHapend++;
    }
    printf("L:%d\n",*L);
    removeSpaces(sourceCodeOreder);
    switch (value) {
        /*mov*/
        case 0:
        /*get first oprand*/
            token = strtok(sourceCodeOreder, ",");
            /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
            opcodeExists = getOpcodeMethod(token,hash_table);
            if (opcodeExists == 2 || opcodeExists == 3)
            {
                isDubeleRegister++;
            }
            if (opcodeExists != -1 && !(errorHapend))
            {
                /*write the surce code*/
                setOpcodeBit(binaryMachineCode,opcodeExists+sourceStartIndex, sourceStartIndex,sourceEndIndex);
            }   
            (*L)++;         
            token = strtok(NULL, ",");
            /*if there is only 1 argument so print error*/
            if (token == NULL){
                fprintf(stderr,"Error: mov must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
            }else
            {
                /*for the target code*/
                opcodeExists = getOpcodeMethod(token, hash_table);
                if (opcodeExists == 2 || opcodeExists == 3)
                {
                    isDubeleRegister++;
                }

                if (opcodeExists == 0){
                    fprintf(stderr,"Error: mov cant get method 0 for target addressing at line number %d\n",countRowInFile);
                    errorHapend++;
                }else{
                    /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
                    if (opcodeExists != -1 && !(errorHapend))
                    {
                        setOpcodeBit(binaryMachineCode, getOpcodeMethod(token, hash_table)+targetStartIndex, targetStartIndex,targetEndIndex);
                    }
                    if (isDubeleRegister != 2)
                    {
                        (*L)++;
                    }
                }
                /*move can get only 2 argument*/
                token = strtok(NULL, ",");
                if (token != NULL){
                fprintf(stderr,"Error: mov must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
                }
            }
            break;
            /*cmp*/
        case 1:
           /*get first oprand*/
            token = strtok(sourceCodeOreder, ",");
            /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
            opcodeExists = getOpcodeMethod(token, hash_table);
            if (opcodeExists == 2 || opcodeExists == 3)
            {
                isDubeleRegister++;
            }
            if (opcodeExists != -1 && !(errorHapend))
            {
                /*write the surce code*/
                setOpcodeBit(binaryMachineCode,opcodeExists+sourceStartIndex, sourceStartIndex,sourceEndIndex);
            }
            (*L)++;            
            token = strtok(NULL, ",");
            /*if there is only 1 argument so print error*/
            if (token == NULL){
                fprintf(stderr,"Error: cmp must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
            }else
            {
            /*for the target code*/
                opcodeExists = getOpcodeMethod(token, hash_table);
                if (opcodeExists == 2 || opcodeExists == 3)
                {
                    isDubeleRegister++;
                }
                /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
                if (opcodeExists != -1 && !(errorHapend))
                {
                    setOpcodeBit(binaryMachineCode, getOpcodeMethod(token, hash_table)+targetStartIndex, targetStartIndex,targetEndIndex);
                }
                if (isDubeleRegister != 2)
                {
                    (*L)++;
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
            if (opcodeExists == 2 || opcodeExists == 3)
            {
                isDubeleRegister++;
            }
            if (opcodeExists != -1 && !(errorHapend))
            {
                /*write the surce code*/
                setOpcodeBit(binaryMachineCode,opcodeExists+sourceStartIndex, sourceStartIndex,sourceEndIndex);
            } 
            (*L)++;           
            token = strtok(NULL, ",");
            /*if there is only 1 argument so print error*/
            if (token == NULL){
                fprintf(stderr,"Error: add must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
            }else
            {
                /*for the target code*/
                opcodeExists = getOpcodeMethod(token, hash_table);
                if (opcodeExists == 2 || opcodeExists == 3)
                {
                    isDubeleRegister++;
                }
                if (getOpcodeMethod(token, hash_table) == 0){
                    fprintf(stderr,"Error: add cant get method 0 for target addressing at line number %d\n",countRowInFile);
                    errorHapend++;
                }else{
                    /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
                    if (opcodeExists != -1 && !(errorHapend))
                    {
                        setOpcodeBit(binaryMachineCode, getOpcodeMethod(token, hash_table)+targetStartIndex, targetStartIndex,targetEndIndex);
                    }
                    if (isDubeleRegister != 2)
                    {
                        (*L)++;
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
	    printf("**********************opcodeExists2: %d\n",opcodeExists);
            if (opcodeExists == 2 || opcodeExists == 3)
            {
                isDubeleRegister++;
            }
            if (opcodeExists != -1 && !(errorHapend))
            {
                /*write the surce code*/
                setOpcodeBit(binaryMachineCode,opcodeExists+sourceStartIndex, sourceStartIndex,sourceEndIndex);
            }            
            (*L)++;
            token = strtok(NULL, ",");
            /*if there is only 1 argument so print error*/
            if (token == NULL){
                fprintf(stderr,"Error: sub must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
            }else
            {
                /*for the target code*/
                opcodeExists = getOpcodeMethod(token, hash_table);
		printf("**********************opcodeExists2: %d ||| token: %s\n",opcodeExists,token);
                if (opcodeExists == 2 || opcodeExists == 3)
                {
                    isDubeleRegister++;
                }
                if (getOpcodeMethod(token, hash_table) == 0){
                    fprintf(stderr,"Error: sub cant get method 0 for target addressing at line number %d\n",countRowInFile);
                    errorHapend++;
                }else{
                    /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
                    if (opcodeExists != -1 && !(errorHapend))
                    {
                        setOpcodeBit(binaryMachineCode, getOpcodeMethod(token, hash_table)+targetStartIndex, targetStartIndex,targetEndIndex);
                    }
		    printf("isDubeleRegister: %d. || L: %d\n",isDubeleRegister,*L);
                    if(isDubeleRegister != 2){(*L)++;}
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
            if (opcodeExists == 2 || opcodeExists == 3)
            {
                isDubeleRegister++;
            }
            if (opcodeExists == 0 || opcodeExists == 2 || opcodeExists == 3)
            {
                fprintf(stderr,"Error: lea cant get method 0 or 2 or 3 for source addressing at line number %d\n",countRowInFile);
                errorHapend++;
            }else if (opcodeExists != -1 && !(errorHapend))
            {
                /*write the surce code*/
                setOpcodeBit(binaryMachineCode,opcodeExists+sourceStartIndex, sourceStartIndex,sourceEndIndex);
            }
            (*L)++;            
            token = strtok(NULL, ",");
            /*if there is only 1 argument so print error*/
            if (token == NULL){
                fprintf(stderr,"Error: lea must get 2 arguments but you provided only 1: '%s' at line number %d\n",token,countRowInFile);
                errorHapend++;
            }else
            {
                /*for the target code*/
                opcodeExists = getOpcodeMethod(token, hash_table);
                if (opcodeExists == 2 || opcodeExists == 3)
                {
                    isDubeleRegister++;
                }
                if (getOpcodeMethod(token, hash_table) == 0){
                    fprintf(stderr,"Error: lea cant get method 0 for target addressing at line number %d\n",countRowInFile);
                    errorHapend++;
                }else{
                    /*check if the oprand is valid its ok to get invalid (like label that are not defiend yet.)*/
                    if (opcodeExists != -1 && !(errorHapend))
                    {
                        setOpcodeBit(binaryMachineCode, getOpcodeMethod(token, hash_table)+targetStartIndex, targetStartIndex,targetEndIndex);
                    }
                    if(isDubeleRegister != 2){(*L)++;}
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
            if (!(errorHapend))
            {
                (*L)++;
            }
            break;
        /*not*/
        case 6:
            methodToCheck[0] = 0;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"not",countRowInFile,methodToCheck,1,hash_table);
            if (!(errorHapend))
            {
                (*L)++;
            }
            break;
        /*inc*/
        case 7:
            methodToCheck[0] = 0;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"inc",countRowInFile,methodToCheck,1,hash_table);
            if (!(errorHapend))
            {
                (*L)++;
            }
            break;
        /*dec*/
        case 8:
            methodToCheck[0] = 0;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"dec",countRowInFile,methodToCheck,1,hash_table);
            if (!(errorHapend))
            {
                (*L)++;
            }
            break;
        /*jmp*/
        case 9:
            methodToCheck[0] = 0;
            methodToCheck[1] = 3;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"jmp",countRowInFile,methodToCheck,2,hash_table);
            if (!(errorHapend))
            {
                (*L)++;
            }
            break;
        /*bne*/
        case 10:
            methodToCheck[0] = 0;
            methodToCheck[1] = 3;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"bne",countRowInFile,methodToCheck,2,hash_table);
            if (!(errorHapend))
            {
                (*L)++;
            }
            break;
        /*red*/
        case 11:
            methodToCheck[0] = 0;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"red",countRowInFile,methodToCheck,1,hash_table);
            if (!(errorHapend))
            {
                (*L)++;
            }
            break;
        /*prn*/
        case 12:
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"prn",countRowInFile,methodToCheck,0,hash_table);
            if (!(errorHapend))
            {
                (*L)++;
            }
            break;
        /*jsr*/
        case 13:
            methodToCheck[0] = 0;
            methodToCheck[1] = 3;
            errorHapend += opcodeHelper(sourceCodeOreder,binaryMachineCode,"jsr",countRowInFile,methodToCheck,2,hash_table);
            if (!(errorHapend))
            {
                (*L)++;
            }
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
