#include "dataStructuresFunc.h"
#include "stringFunc.h"
#include "preAssembler.h"
#include "passOne.h"
#include "passTwo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
int main(int argc, char* argv[]) {
    int i;
    int preAsseblerResult, DC_StartCounting = 0;
    char *inputFile;
    HashTable hash_table;
    if (argc==1)
    {
        fprintf(stderr, "You did not provide a file, please provide at least 1 text file.\n");
        exit(1);
    }
    for (i = 1; i < argc; i++)
    {
	initHashTable(&hash_table, 6);
	inputFile = convertFile(argv[i],".as");
	/*preAssembler return the number of error*/
        preAsseblerResult = preAssembler(inputFile);
	if (preAsseblerResult){
	    continue;	
	}else{
	    inputFile = convertFile(argv[i],".am");
	    preAsseblerResult = passOne(inputFile, &hash_table, &DC_StartCounting);
        printf("DC_StartCounting: %d\n",DC_StartCounting);
	    if (preAsseblerResult){
            printf("error!!!!\n");
        }else{
            preAsseblerResult = pasTwo(inputFile, &hash_table, DC_StartCounting);
        }
	}
	freeHashTable(&hash_table);
    }
    return 0;
}