#include "testTree.h"

void preOrder(BNF *root)
{
    printf("BNF definition: %s at level: %d.\n", root->bnfName, root->bnfLevel);
    if(root->unit1)
    preOrder(root->unit1);
    if(root->unit2)
    preOrder(root->unit2);
    if(root->unit3)
    preOrder(root->unit3);
    if(root->unit4)
    preOrder(root->unit4);
}