#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitControler.h"
#include "dataStructuresFunc.h"
#include "stringFunc.h"
#include "passTwoHelper.h"
#define maxCharsInRow 80
#define MAX_DC_CHAR 5



int passTwo(char* fileName, HashTable* hash_table, ram_array array, int DC_StartCounting){
    FILE* fptr;
    int IC = 100, DC = DC_StartCounting, L, countRowInFile =1, i = 0;
    int errorHapend = 0;
    char content[maxCharsInRow];
    char *token;
    char *tokenCommas;
    char *tempName;
    char *symboleNane;
    char *remainderContent;
    char *token_copy;
    char *externOrEntry;
    int tokenCommasInt;
    fptr = fopen(fileName, "r");
    if (fptr == NULL) {
        perror("Failed to open file");
        return 1;
    }
    while (fgets(content, maxCharsInRow, fptr)){
        token = strtok(content, " ");
        while (token != NULL){
            /*if it symbole*/
            if (token[strlen(token) - 1] == ':'){ 
                /*search in symbole table*/
                if (strcmp(searchHashTable(hash_table,token,-1), "-1") == 0){
                    errorHapend++;
                    fprintf(stderr,"%s is Unrecognized symbol. error happend at line number %d.\n",token,countRowInFile);
                }
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
                /*if it ".string" command*/
                if (strcmp(tempName, ".string") == 0){
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
                                /*convert char to asci command. and set the value in ram memoery*/
                                set_cell_value(array,DC,(int)tokenCommas[i]);
                                DC++;
                            }
                            /*update DC and i*/
                            i++;
                        }
                        /*set the '\0'*/
                        set_cell_value(array,DC,0);
                        /*update DC*/                        
                        DC ++;
                        /*take the next string*/
                        tokenCommas = strtok(NULL,",");
                    }
                    /*if command are ".data"*/                     
                }else if (strcmp(tempName, ".data") == 0){
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
                        set_cell_value(array,DC,tokenCommasInt);
                        DC++;
                        tokenCommas = strtok(NULL,",");
                    }
                } 
            }else if (getOpcode(token) != -1)
            {
                /*value of the command example "mov #3, r3" remainderContent wiil be "#3, r3"  */
		        token_copy = strduppp(token);
                remainderContent = strtok(NULL,"");
                if (remainderContent != NULL)
                {
                    /*opcodeExeForPassOne function that handele all 16 cases (mov,lea,stop....)*/
                    errorHapend += opcodeExeForPassTwo(getOpcode(token_copy), remainderContent,countRowInFile, hash_table, &L,array,IC);
                }else{
                    errorHapend += opcodeExeForPassTwo(getOpcode(token_copy), token_copy,countRowInFile, hash_table, &L,array,IC);
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