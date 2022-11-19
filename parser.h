#ifndef PARSER_H
#define PARSER_H
#include "scanner.h"
#include "token.h"

typedef struct BNF
{
    token leftTkn1;
    token leftTkn2;
    token leftTkn3;
    token leftTkn4;
    token leftTkn5;
    token rightTkn1;
    token rightTkn2;
    token rightTkn3;
    token rightTkn4;
    token midTkn4;
    int bnfLevel;
    
    //the most possible definitional subunits in all nonterminals
    struct BNF* unit1;
    struct BNF* unit2;
    struct BNF* unit3;
    struct BNF* unit4;

    char *bnfName;

} BNF;
//BNF prototypes
void newline();
BNF* parser();
BNF *program();
BNF* getDef(char *, int);
BNF* block(int);
BNF* vars(int);
BNF* expr(int);
BNF* A(int);
BNF* N(int);
BNF* M(int);
BNF* R(int);
BNF* stats(int);
BNF* mStat(int);
BNF* stat(int);
BNF* in(int);
BNF* out(int);
BNF* iff(int);
BNF* loop(int);
BNF* Assign(int);
BNF* RO(int);
token tk;
int level;


#endif