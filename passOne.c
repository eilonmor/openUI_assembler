#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitControler.h"
#include "dataStructuresFunc.h"
#include "stringFunc.h"
#include "passOneHelper.h"
#define maxCharsInRow 80
#define MAX_DC_CHAR 5



int passOne(char* fileName, HashTable* hash_table, int *DC_StartCounting){
    FILE* fptr;
    int IC = 0, DC = 0, L, countRowInFile =1, i = 0;
    int errorHapend = 0;
    char content[maxCharsInRow];
    char *token;
    char *tokenCommas;
    char *tempName;
    char *symboleNane;
    char *remainderContent;
    char *token_copy;
    char *externOrEntry;
    char convertIntToStringBuffer[MAX_DC_CHAR];
    int thereIsSymbole, tokenCommasInt;
    ram_array RAM_memoery = {0};
    fptr = fopen(fileName, "r");
    if (fptr == NULL) {
        perror("Failed to open file");
        return 1;
    }
    while (fgets(content, maxCharsInRow, fptr)){
	thereIsSymbole = 0;
        token = strtok(content, " ");
        while (token != NULL){
            /*if it symbole*/
            if (token[strlen(token) - 1] == ':'){ 
                /*symbole are exsist*/
                if (strcmp(searchHashTable(hash_table,token,-1), "-1") != 0){
                    fprintf(stderr,"%s is allready defined. error happend at line number %d.\n",token,countRowInFile);
                }else{
                    thereIsSymbole = 1;
                    symboleNane = strduppp(token);
		            removeSpaces(symboleNane);
                }
            /*.data command or string*/    
            }else if (strcmp(token, ".string") == 0 || strcmp(token, ".data") == 0)
            {
                /*duplicate string*/
                tempName = strduppp(token);
                /*in pre assembler (macro handel) I take care of spaces and comments*/
                token = strtok(NULL, "");
		token_copy = strduppp(token);
                /*search for unvalid commas like " ,  ," or ",1,2 " or " 1 , " */
                errorHapend = hasMultipalCommas(token);
                if (errorHapend){
                    fprintf(stderr,"Invalid commas: there is multipal commas at line number %d \n",countRowInFile);
                }
                /*convert int to string*/
                sprintf(convertIntToStringBuffer, "%d", DC);
                /*if it ".string" command*/
                if (strcmp(tempName, ".string") == 0){
                    /*update the table if there is symbole*/
                    if (thereIsSymbole){
                        insertOrUpdateHashTable(hash_table, ".string", symboleNane, convertIntToStringBuffer);
                        thereIsSymbole = 0;
                    }
                    /*take the first string*/
                    tokenCommas = strtok(token_copy,",");
                    /*remove all spacses*/
                    removeSpaces(tokenCommas);
                    /*loop - each string*/
                    while(tokenCommas != NULL){
                        /*loop - each char*/
                        while (tokenCommas[i] != '\0')
                        {
                            if (tokenCommas[i] >= 32 && tokenCommas[i] <= 126){
                                set_cell_value(RAM_memoery,DC,(int)tokenCommas[i]);
                                DC++;
                            }
                            i++;
                        }
                        set_cell_value(RAM_memoery,DC,0);
                        /*update DC for the last char '\0'*/                         
                        DC ++;
                        /*take the next string*/
                        tokenCommas = strtok(NULL,",");
                    }
                    /*if command are ".data"*/
                }else if (strcmp(tempName, ".data") == 0){
                    /*update the table if it symbole*/
                    if (thereIsSymbole){
                        insertOrUpdateHashTable(hash_table, ".data", symboleNane, convertIntToStringBuffer);
                        thereIsSymbole = 0;
                    }
                    /*take the first number*/
                    tokenCommas = strtok(token_copy,",");
                    /*remove spaces if has*/
                    removeSpaces(tokenCommas);
                    /*convert the number to int*/
                    tokenCommasInt = atoi(tokenCommas);
                    /*atoi return 0 if the string is not valid number. also check that the number at the first place are not 0*/
                    if (!(tokenCommasInt) && tokenCommas[0] == '0'){
                    fprintf(stderr,"Invalid value: data can get only number. at line number %d\n",countRowInFile);
                    }
                    /*if the number are valid so set the cell*/
                    while(tokenCommas != NULL){
                        set_cell_value(RAM_memoery,DC,tokenCommasInt);
                        DC++;
                        tokenCommas = strtok(NULL,",");
                    }
                }
            /*entry or exstern command*/    
            }else if (strcmp(token, ".entry") == 0 || strcmp(token, ".extern") == 0)
            {
                externOrEntry = strduppp(token);
                token = strtok(NULL," ");
                while (token != NULL)
                {
                    /*check duplicate defenition*/
                    if (strcmp(searchHashTable(hash_table,token,-1), "-1") != 0)
                    {
                        errorHapend++;
                        fprintf(stderr,"Duplicate defnition: there is allready variable named: '%s'. you cant import (extern) variable with the same name. error hapend at line: %d.\n",token,countRowInFile);
                    }
		    /*remove \n*/
		    if(token[strlen(token) - 1] == '\n'){token[strlen(token) - 1] = '\0';}
                    /*if it extern*/
                    if (strcmp(externOrEntry, ".extern") == 0){
                        insertOrUpdateHashTable(hash_table, ".extern", token, "-1");
                    }else{
                        insertOrUpdateHashTable(hash_table, ".entry", token, "-1");
                    }
                    token = strtok(NULL," ");
                }
                free(externOrEntry);
            /*it "regular comand from 0 to 15"*/    
            }else if (getOpcode(token) != -1)
            {
                /*value of the command example "mov #3, r3" remainderContent wiil be "#3, r3"  */
                token_copy = strduppp(token);
                remainderContent = strtok(NULL,"");
                if (thereIsSymbole){
                    /*convert int to string*/
                    sprintf(convertIntToStringBuffer, "%d", IC);
                    insertOrUpdateHashTable(hash_table, ".symbole", symboleNane, convertIntToStringBuffer);
                    thereIsSymbole = 0;
                }
                if (remainderContent != NULL)
                {
                    /*opcodeExe function that handele all 16 cases (mov,lea,stop....)*/
                    errorHapend += opcodeExe(getOpcode(token_copy), remainderContent,countRowInFile, hash_table, &L);
                }else{
                    errorHapend += opcodeExe(getOpcode(token_copy), token_copy,countRowInFile, hash_table, &L);
                }
                IC += L;
            }else
            {
                /*the command are not difined.*/
                fprintf(stderr,"Error, invalid instruction at line %d\n",countRowInFile);
                errorHapend++;
            }
	    token = strtok(NULL, " ");
	    if (token != NULL){removeSpaces(token);}
        }
	countRowInFile++;
        
    }
    printHashTable(hash_table);
    printf("\n\n");
    (*DC_StartCounting) = IC;
    if (IC<1)
    {
        addValueToHashTable(hash_table, 1, 100);
        addValueToHashTable(hash_table, 2, 100);
    }else{
        addValueToHashTable(hash_table, 1, 101+IC-L);
        addValueToHashTable(hash_table, 2, 101+IC-L);
    }
    addValueToHashTable(hash_table, 5, 100);
    printHashTable(hash_table);
    fclose(fptr);
    return errorHapend;
}