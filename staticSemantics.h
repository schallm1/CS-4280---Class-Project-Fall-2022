#ifndef STATIC_SEM_H
#define STATIC_SEM_H

#include "token.h"
#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"
#include "parser.h"

struct Stack
{
    token *st;
    int top;
    unsigned capacity;
};

struct Stack* createStack(unsigned);
int isFull(struct Stack*);
int isEmpty(struct Stack*);
void push(struct Stack*,token);
char * pop(struct Stack*);
int verify(struct Stack*, token);
int locate(struct Stack*, token);
void staticSem(struct Stack*, BNF*, int);

#endif