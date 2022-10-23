#ifndef TOKEN_H
#define TOKEN_H

//enum for token values
typedef enum{
     idKw = 1001, intg, op, gthan, lthan, punc, add, sub, gthaneq, lthaneq, assign, equality, increm, decrem, eof,
     capLetterError = -1, unrecognizedCharacterError = -2, tooManyError = -3

}tkType;

//token type
typedef struct
{
    tkType tkn;
    char *tokenName;
    int lineNum;
    char *tokenInstance;

}token;

#endif