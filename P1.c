#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "testscanner.c"
#include "token.h"


int main(int argc, char *argv[])
{

    token toke;
    //file input
    if(argc == 2)
    {
        keyStatus = 0;
        char *string = ".cs4280";
        char file[strlen(argv[1]) + strlen(string)];
        strcpy(file, argv[1]);
        strcat(file, string);
        printf("%s", argv[1]);
        input = fopen(file, "r+");

        if(input == NULL)
        {
            printf("File does not exist. Please create a valid input file.\n");
            exit(1);
        }
        testscanner(toke);
    }
    //keyboard input
    else if(argc == 1)
    {

        if(isatty(STDIN_FILENO))
        {
            keyStatus = 1;
            printf("Enter a string to be processed by the scanner:\n");
            keyboardScan();
        }
        testscanner(toke);   
    }
    else
    {
        printf("The program was unable to process the given arguments. Try again");
        exit(1);
    }



}