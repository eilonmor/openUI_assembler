#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitControler.h"
#include "dataStructuresFunc.h"
#include "stringFunc.h"
#include "passOneHelper.h"
#define maxCharsInRow 80
#define MAX_DC_CHAR 5

int passOne(char* fileName, HashTable* hash_table){
    FILE* fptr;
    int IC = 0, DC = 0, L=0, countRowInFile =1, i = 0;
    int errorHapend = 0;
    char content[maxCharsInRow];
    char *token;
    char *tokenCommas;
    char *tempName;
    char *symboleNane;
    char *remainderContent;
    char *token_copy;
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
		        printf("token: |%s|\n",token);
                if (strcmp(searchHashTable(hash_table,token,-1), "-1") != 0){
                    fprintf(stderr,"%s is allready defined. error happend at line number %d. (forme)\n",token,countRowInFile);
                }else{
                    thereIsSymbole = 1;
                    symboleNane = strduppp(token);
		            printf("symboleNane: %s\n",symboleNane);
		            removeSpaces(symboleNane);
                }
            /*.data command or string*/    
            }else if (strcmp(token, ".string") == 0 || strcmp(token, ".data") == 0)
            {
                tempName = strduppp(token);
                token = strtok(NULL, "");
		        token_copy = strduppp(token);
                /*search for unvalid commas like " ,  ," or ",1,2 " or " 1 , " */
                errorHapend = hasMultipalCommas(token);
                if (errorHapend){
                    fprintf(stderr,"Invalid commas: there is multipal commas at line number %d \n",countRowInFile);
                }
                /*convert int to string*/
                sprintf(convertIntToStringBuffer, "%d", DC);
                if (strcmp(tempName, ".string") == 0){
                    if (thereIsSymbole){
                        insertOrUpdateHashTable(hash_table, ".string", symboleNane, convertIntToStringBuffer);
                        thereIsSymbole = 0;
                    }
                    tokenCommas = strtok(token_copy,",");
                    removeSpaces(tokenCommas);
                    while(tokenCommas != NULL){
                        while (tokenCommas[i] != '\0')
                        {
                            /*set each char*/
                            set_cell_value(RAM_memoery,DC,(int)tokenCommas[i]);
                            i++;
                            DC++;
                        }
                        set_cell_value(RAM_memoery,DC,0);                        
                        DC ++;
                        tokenCommas = strtok(NULL,",");
                    }                     
                }else if (strcmp(tempName, ".data") == 0){
                    if (thereIsSymbole){
                        insertOrUpdateHashTable(hash_table, ".data", symboleNane, convertIntToStringBuffer);
                        thereIsSymbole = 0;
                    }
                    tokenCommas = strtok(token_copy,",");
                    removeSpaces(tokenCommas);
		    tokenCommasInt = atoi(tokenCommas);
		    if (!(tokenCommasInt) && tokenCommas[0] == '0'){
			fprintf(stderr,"Invalid value: data can get only number. at line number %d\n",countRowInFile);
		    }
                    while(tokenCommas != NULL){
                        set_cell_value(RAM_memoery,DC,tokenCommasInt);
                        DC++;
                        tokenCommas = strtok(NULL,",");
                    }
                }    
            }else if (strcmp(token, ".entry") == 0 || strcmp(token, ".extern") == 0)
            {
                
                    token = strtok(NULL," ");
                    while (token != NULL)
                    {
                        if (strcmp(searchHashTable(hash_table,token,-1), "-1") == 0)
                        {
                            errorHapend++;
                            fprintf(stderr,"Duplicate defnition: there is allready variable named: '%s'. you cant import (extern) variable with the same name. error hapend at line: %d.\n",token,countRowInFile);
                        }
                        if (strcmp(token, ".extern") == 0){
                            insertOrUpdateHashTable(hash_table, ".extern", symboleNane, "null");
                            }else{
                                insertOrUpdateHashTable(hash_table, ".entry", symboleNane, "null");
                            }
                        token = strtok(NULL," ");
                    }
	    
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
                    IC++;
                }
                if (remainderContent != NULL)
                {
                    printf("debug: key: remainderContent  value: %s\n",remainderContent);
                    /*opcodeExe function that handele all 16 cases (mov,lea,stop....)*/
                    errorHapend += opcodeExe(getOpcode(token_copy), remainderContent,countRowInFile, hash_table, &L);
                }else{
                    errorHapend += opcodeExe(getOpcode(token_copy), token_copy,countRowInFile, hash_table, &L);
                }
                IC += L;
            }else
            {
		printf("before ERR: %s\n",token);
                fprintf(stderr,"Error, invalid instruction at line %d\n",countRowInFile);
                errorHapend++;
            }
	    token = strtok(NULL, " ");
	    if (token != NULL){removeSpaces(token);}
        }
	countRowInFile++;
    }
    printHashTable(hash_table);
    fclose(fptr);
    return errorHapend;
}
