#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "staticSemantics.h"
#include "scanner.h"
#include "parser.h"
#include "token.h"
#include <stdbool.h>

int status = 0;
int ind = 0;
int scope = 0;
int Var = 0;
int varNumber = 0;
int f = 0;
char *tp;
char *varss[100];
//geeks for geeks stack
struct Stack* createStack(unsigned cap)
{
    token tok;
    tok.lineNum = 0;
    tok.tokenInstance="";
    tok.tkn=TBD;
    tok.tokenName="";
    struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
    stack->capacity = cap;
    stack->top = -1;
    stack->st = (token*)malloc(stack->capacity * sizeof(token));
    for(int j = 0; j<stack->capacity; j++)
        stack->st[j] = tok;
    return stack;
}

char *variable()
{
    
    asprintf(&tp, "var%d", Var);
    varss[Var++] = tp;
    return tp;

}
int isFull(struct Stack* stack)
{
    return stack->top == stack->capacity - 1;
}

int isEmpty(struct Stack* stack)
{
    return stack->top == -1;
}

void push(struct Stack* stack, token item)
{
    if (isFull(stack))
        exit(1);
    fprintf(stderr, "%d\n", stack->top);
    stack->st[ind++] = item;
    stack->top++;
    fprintf(stderr, "%s pushed to stack\n", item.tokenInstance);
}

char* pop(struct Stack* stack)
{
    if (isEmpty(stack))
        return "";
    ind--;
    return stack->st[stack->top--].tokenInstance;
}


int verify(struct Stack* ST, token tkn){
	for(int i = ST->top; i >= 0; i--)
    {
        fprintf(stderr, "%s\n", tkn.tokenInstance);
        fprintf(stderr, "%s\n", ST->st[i].tokenInstance);
        fprintf(stderr, "%d\n", ST->top);
		if(strcmp(ST->st[i].tokenInstance, tkn.tokenInstance)==0)
			return ST->top;
	}
	return -1;
}

int locate(struct Stack* ST, token tkn)
{
	for(int i = ST->top; i >= scope; i--){
		if(strstr(ST->st[i].tokenInstance, tkn.tokenInstance)!=NULL){
            fprintf(stderr, "locate works\n");
			return ST->top;
		}
	}
	return -1;
}

void staticSem(struct Stack* ST, BNF* bnf, int num)
{
    if(!bnf)
    return;
    // <program> -> <vars> <block>
    if(strstr(bnf->bnfName,"<program>")!=NULL)
    {
        int n = 0;
        if(bnf->unit1)
            staticSem(ST, bnf->unit1, n);

        if(bnf->unit2)
            staticSem(ST, bnf->unit2, n);

        for(int i = 0; i<Var; i++)
            fprintf(stderr, "%s 0\n", varss[i]);
        fprintf(stderr, "STOP\n");
    }
    // <vars> -> empty | var identifier: Integer; <vars>
    else if(strstr(bnf->bnfName, "<vars>")!=NULL)
    {
        int spot = locate(ST, bnf->leftTkn2);
        
        if(spot == -1)
        {
            push(ST, bnf->leftTkn2);
            num++;
        }
        else if(spot<num)
        {
            fprintf(stderr, "Variable in token %s was already delcared in this scope.", bnf->leftTkn1.tokenInstance);
            exit(1);
        }
        if(bnf->unit1)
            staticSem(ST, bnf->unit1, num);
    }
    //<block> -> begin <vars> <stats> end
    else if(bnf->bnfName=="<block>")
    {
        int n = 0;
        scope = ind;
        if(bnf->unit1)
            staticSem(ST, bnf->unit1, n);
        if(bnf->unit2)
            staticSem(ST, bnf->unit2, n);
        if(ST->top > -1)
            tp = pop(ST);
    }
    //<exp>-> <A> ++ <exp> | <A>++ (one token)
    else if(strstr(bnf->bnfName, "<exp>") != NULL)
    {
        if(bnf->unit2)
        {
            char * t = variable();
            staticSem(ST, bnf->unit1, num);
            fprintf(stderr, "STORE %s\n", t);
            staticSem(ST, bnf->unit2, num);
            fprintf(stderr, "ADD %s\n", t);
        }
        else if(bnf->unit1)
        staticSem(ST, bnf->unit1, num);
        
    }
    //<A> -> <N> -- <A> | <N> (-- one token)
    else if(strstr(bnf->bnfName, "<A>")!= NULL)
    {
        if(bnf->unit2)
        {
            char *t = variable();
            if(bnf->unit1)
            staticSem(ST, bnf->unit1, num);
            fprintf(stderr, "STORE %s\n", t);
            staticSem(ST, bnf->unit2, num);
            fprintf(stderr, "SUB %s\n", t);
        }
        else if(bnf->unit1)
            staticSem(ST, bnf->unit1, num);
    }
    else if(strstr(bnf->bnfName, "<stats>")!=NULL || strstr(bnf->bnfName, "<stat>") !=NULL || strstr(bnf->bnfName,"<mStat>") != NULL)
    {
        if(bnf->unit1 != NULL)
        {
            staticSem(ST, bnf->unit1, num);
        }
        if(bnf->unit2 != NULL)
        {
            staticSem(ST, bnf->unit2, num);
        }
        if(bnf->unit3 != NULL)
        {
            staticSem(ST, bnf->unit3, num);
        }
        if(bnf->unit4 != NULL)
        {
            staticSem(ST, bnf->unit4, num);
        }
    }
    // <N> -> <M> / <N> | <M> * <N> | <M>
    else if(bnf->bnfName=="<N>")
    {
        char *v = variable();
        if(bnf->rightTkn1.tkn == op && strstr(bnf->rightTkn1.tokenInstance, "/")!=NULL)
        {
            if(bnf->unit2)
                staticSem(ST, bnf->unit2, num);
            fprintf(stderr, "STORE %s\n",v);
            if (bnf->unit1)
                staticSem(ST, bnf->unit1, num);
            fprintf(stderr, "DIV %s\n",v);
        }
        else if(bnf->rightTkn1.tkn == op && strstr(bnf->rightTkn1.tokenInstance, "*")!=NULL)
        {
            if(bnf->unit2)
                staticSem(ST, bnf->unit2, num);
            fprintf(stderr, "STORE %s\n", v);
            if (bnf->unit1)
                staticSem(ST, bnf->unit1, num);
            fprintf(stderr, "MULT %s\n", v);
        }
        if(bnf->unit1)
            staticSem(ST, bnf->unit1, num);
    }
    // // <M> -> -- <M> | <R> (-- one token)
    else if(bnf->bnfName == "<M>")
    {
        if(bnf->leftTkn1.tkn==decrem)
        {
            if(bnf->unit1 != NULL)
            {
                staticSem(ST, bnf->unit1, num);
                fprintf(stderr, "MULT -1\n");
            }
            char *v = variable();
            fprintf(stderr, "STORE %s\n", v);
        }
        else if(bnf->unit2 != NULL)
            staticSem(ST, bnf->unit2, num);
    }
    // <R> -> [ <exp> ] | Identifier | integer
    else if(bnf->bnfName == "<R>")
    {
        if (bnf->leftTkn1.tkn == idKw)
        {
            status = verify(ST, bnf->leftTkn1);
            if(status == -1)
            {
                printf("ID %s undeclared.\n", bnf->leftTkn1.tokenInstance);
                exit(1);
            }   
            fprintf(stderr, "STACKR %d\n", status);
        }
        else if(bnf->leftTkn1.tkn = intg)
            fprintf(stderr, "LOAD %s\n", bnf->leftTkn1.tokenInstance);
        else if (bnf->unit1 != NULL)
            staticSem(ST, bnf->unit1, num);
    }
    //<in> -> scan Identifier
    else if(bnf->bnfName == "<in>")
    {
        status = verify(ST, bnf->leftTkn2);
        if (status == -1)
        {
            printf("ID %s undeclared.\n", bnf->leftTkn2.tokenInstance);
            exit(1);
        }
        char *v = variable();
        fprintf(stderr, "READ %s\n", v);
        fprintf(stderr, "LOAD %s\n", v);
        fprintf(stderr, "STACKW %d\n", status);
    }
    //<out> -> print (<exp>)
    else if(bnf->bnfName == "<out>")
    {
        if(bnf->unit1 != NULL)
            staticSem(ST, bnf->unit1, num);
        char *t = variable();
        fprintf(stderr, "STORE %s\n", t);
        fprintf(stderr, "WRITE %s\n", t);
    }
    //<if> -> fork (<exp> <RO> <exp>) then <stat>
    else if(bnf->bnfName == "<if>")
    {
        f++;
        int loopIt = f;
        char * v = variable();

        fprintf(stderr, "LOOP%d:\n", loopIt);
        staticSem(ST, bnf->unit1, num);
        fprintf(stderr, "STORE %s\n", v);
        staticSem(ST, bnf->unit3, num);
        fprintf(stderr, "SUB %s\n", v);
        staticSem(ST, bnf->unit2, num);
        staticSem(ST, bnf->unit4, num);

        fprintf(stderr, "final: NOOP\n", loopIt);
    }
    //<loop> -> loop ( <exp> <RO> <exp> ) <stat>
    else if(bnf->bnfName == "<loop>")
    {
        char *v = variable();
        f++;
        int loopNum = f;

        fprintf(stderr, "LOOP%d\n", loopNum);
        staticSem(ST, bnf->unit1, num);
        
        fprintf(stderr, "STORE %s\n", v);
        staticSem(ST, bnf->unit3, num);

        fprintf(stderr, "SUB %s\n", v);
        staticSem(ST, bnf->unit2, num);

        staticSem(ST, bnf->unit4, num);

        fprintf(stderr, "BR LOOP%d\n", loopNum);
        fprintf(stderr, "final%d: NOOP\n",loopNum);
    }
    //<assign> -> Identifier == <exp> (one == token)
    else if(bnf->bnfName=="<assign>")
    {
        if(bnf->unit1 != NULL)
            staticSem(ST, bnf->unit1, num);
        status = verify(ST, bnf->leftTkn1);
        if (status == -1)
        {
            printf("ID %s undeclared.\n", bnf->leftTkn1);
            exit(1);
        }

        fprintf(stderr, "STACKW %d\n", status);   
    }
    //<RO> -> <= (one token) | >= (one token)| = | %
    else if(bnf->bnfName=="<RO>")
    {
        if(bnf->leftTkn1.tkn ==lthaneq)
            fprintf(stderr, "BRZNEG: final%d\n", f);
        else if(bnf->leftTkn1.tkn ==gthaneq)
            fprintf(stderr, "BRZPOS: final%d\n", f);
        else if(bnf->leftTkn1.tkn == lthan)
            fprintf(stderr, "BRNEG: final%d\n", f);
        else if(bnf->leftTkn1.tkn== gthan) 
            fprintf(stderr, "BRPOS: final%d\n",f);
        else if(bnf->leftTkn1.tkn == assign)
            fprintf(stderr, "BRZERO: final%d\n", f);
        else if (bnf->leftTkn1.tkn == op && strstr(bnf->leftTkn1.tokenInstance, "%%"))
            fprintf(stderr, "DIV %s", bnf->leftTkn2.tokenInstance);
    }
}


