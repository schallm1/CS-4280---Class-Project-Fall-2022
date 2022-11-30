#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "parser.c"
#include "scanner.c"
#include "testTree.c"
#include "staticSemantics.c"


int main(int argc, char *argv[])
{

    BNF *treeRoot;
    token toke;
    //file input
    if(argc == 2)
    {
        keyStatus = 0;
        char *string = ".cs4280";
        char file[strlen(argv[1]) + strlen(string)];
        strcpy(file, argv[1]);
        strcat(file, string);
        input = fopen(file, "r+");

        if(input == NULL)
        {
            printf("File does not exist. Please create a valid input file.\n");
            exit(1);
        }
        //parser calla and preorder printout
        treeRoot = parser();
        struct Stack *ST = createStack(100);
        staticSem(ST, treeRoot, 0);
    }



}