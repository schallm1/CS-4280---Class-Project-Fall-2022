#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include "token.h"

token scanner();
token FADriver();
void processCharacter();
char* addChar(char *, char);
token assignToken(char*);
token setTokenName(token);





#endif