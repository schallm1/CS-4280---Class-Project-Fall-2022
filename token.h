#ifndef TOKEN_H
#define TOKEN_H

typedef enum{
     idKw = 1001, intg, op, gthan, lthan, punc, add, sub, gthaneq, lthaneq, assign, equality, increm, decrem, eof,
     ERROR = -1

}tkType;


typedef struct
{
    tkType tkn;
    char *tokenName;
    int lineNum;
    char *tokenInstance;

}token;

#endif