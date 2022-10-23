#include "token.h"
#include <stdio.h>
#include "scanner.h"
#include "scanner.c"
#include "testscanner.h"

void testscanner(token toke)
{
    while(1)
        {
            if (toke.tkn == eof)
                break;
            toke = scanner();
            printf("token string: %s\n", toke.tokenInstance);
            printf("token type: %s\n", toke.tokenName);
            printf("token line number: %d\n\n", toke.lineNum);
        }
}