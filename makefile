mainP: main.c preAssembler.c stringFunctions.c dataStructuresFunc.c 
	gcc -ansi -pedantic -g -Wall main.c preAssembler.c passOne.c stringFunctions.c dataStructuresFunc.c bitControler.c passOneHelper.c -o mainP
