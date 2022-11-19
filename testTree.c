#include "testTree.h"

void preOrder(BNF *root)
{
    for(int a =0; a<root->bnfLevel; a++)
    printf("  ");

    printf("%s\n", root->bnfName);

    if(root->leftTkn1.tokenInstance)
    {
        for(int i =0; i<root->bnfLevel; i++)
        printf("  ");
        printf("%s\n", root->leftTkn1.tokenInstance);
    }
    if(root->leftTkn2.tokenInstance)
    {
        for(int j =0; j<root->bnfLevel; j++)
        printf("  ");
        printf("%s\n", root->leftTkn2.tokenInstance);
    }
    if(root->leftTkn3.tokenInstance)
    {
        for(int k =0; k<root->bnfLevel; k++)
        printf("  ");
        printf("%s\n", root->leftTkn3.tokenInstance);
    }
    if(root->leftTkn4.tokenInstance)
    {
        for(int l =0; l<root->bnfLevel; l++)
        printf("  ");
        printf("%s\n", root->leftTkn4.tokenInstance);
    }
    if(root->leftTkn5.tokenInstance)
    {
        for(int m =0; m<root->bnfLevel; m++)
        printf("  ");
        printf("%s\n", root->leftTkn5.tokenInstance);
    }

    if(root->unit1)
    {
        preOrder(root->unit1);
        if(root->rightTkn1.tokenInstance)
        {
            for(int n =0; n<root->bnfLevel; n++)
            printf("  ");
            printf("%s\n", root->rightTkn1.tokenInstance);
        }
    }
    if(root->unit2)
    {
        preOrder(root->unit2);
        if(root->rightTkn2.tokenInstance)
        {
            for(int o =0; o<root->bnfLevel; o++)
            printf("  ");
            printf("%s\n", root->rightTkn2.tokenInstance);
        }
    }
    if(root->unit3)
    {
        preOrder(root->unit3);
        if(root->rightTkn3.tokenInstance)
        {
            for(int p =0; p<root->bnfLevel; p++)
            printf("  ");
            printf("%s\n", root->rightTkn3.tokenInstance);
        }
    }
    if(root->unit4)
    {
        if(root->midTkn4.tokenInstance)
        {
            for(int q =0; q<root->bnfLevel; q++)
            printf("  ");
            printf("%s\n", root->midTkn4.tokenInstance);
        }
        preOrder(root->unit4);
        if(root->rightTkn4.tokenInstance)
        {
            for(int q =0; q<root->bnfLevel; q++)
            printf("  ");
            printf("%s\n", root->rightTkn4.tokenInstance);
        }
    }
}