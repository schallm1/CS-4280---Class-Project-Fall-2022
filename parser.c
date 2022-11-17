#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"

token tk;
int level;

BNF* parser()
{
    tk = scanner();
    fprintf(stderr, "Token: %s, type: %d\n", tk.tokenInstance, tk.tkn);
    newline();
    if(tk.tkn == eof)
    {
        fprintf(stderr, "No tokens found in input.\n");
        exit(0);
    }
    BNF *begin = program();
    if(tk.tkn == eof)
    return begin;
    else
    {
        fprintf(stderr, "Error. eof token not found in parser.\n");
        exit(0);
    }
}
//create and store new BNF unit
BNF *getDef(char *id, int level)
{
    BNF* newDef = (BNF *) malloc(sizeof(BNF));
    newDef->unit1 = NULL;
    newDef->unit2 = NULL;
    newDef->unit3 = NULL;
    newDef->unit4 = NULL;
    newDef->bnfLevel = level;
    newDef->bnfName = id;

    return newDef;
}
// <program> -> <vars> <block>
BNF* program()
{
    level = 0;
    BNF* prog = getDef("<program>", level);
    prog->unit1 = vars(level);
    prog->unit2 = block(level);
    return prog;
}
//<block> -> begin <vars> <stats> end
BNF* block(int level)
{
    level++;
    BNF* Block = getDef("<block>", level);
    if(tk.tkn == idKw && strstr(tk.tokenInstance,"begin")!=NULL)
    {  
        tk=scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Block->bnfName);
        newline();
        Block->unit1 = vars(level);
        Block->unit2 = stats(level);
        if(tk.tkn == idKw && strstr(tk.tokenInstance, "end")!=NULL)
        {
            tk = scanner();
            fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Block->bnfName);
            newline();
            return Block;
        }
        else
        {
            fprintf(stderr, "Error, no 'end' keyword identified in block. Token identified:%s\n", tk.tokenInstance);
            exit(0);
        }
    }
    else
    {
        fprintf(stderr, "Error, no 'begin' keyword identified in block.\n");
        exit(0);
    }
}
// <vars> -> empty | var identifier: Integer; <vars>
BNF* vars(int level)
{
    level++;
    BNF* Vars = getDef("<vars>", level);

    if(tk.tkn == idKw && strstr(tk.tokenInstance, "var")!=NULL)
    {
        tk=scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Vars->bnfName);
        newline();
        if(tk.tkn == idKw && strstr(tk.tokenInstance, "begin")==NULL && strstr(tk.tokenInstance, "end")==NULL && strstr(tk.tokenInstance, "var")==NULL && strstr(tk.tokenInstance, "fork")==NULL && strstr(tk.tokenInstance, "loop")==NULL && strstr(tk.tokenInstance, "then")==NULL && strstr(tk.tokenInstance, "scan")==NULL && strstr(tk.tokenInstance, "print") ==NULL)
        {
            Vars->tkn1 = tk;
            tk = scanner();
            fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Vars->bnfName);
            newline();
            if(tk.tkn == punc && strstr(tk.tokenInstance,":")!=NULL)
            {
                tk = scanner();
                fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Vars->bnfName);
                newline();
                if(tk.tkn == intg)
                {
                    Vars->tkn2 = tk;
                    tk=scanner();
                    fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Vars->bnfName);
                    newline();
                    if(tk.tkn == punc && strstr(tk.tokenInstance, ";") != NULL)
                    {
                        tk=scanner();
                        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Vars->bnfName);
                        newline();
                        Vars->unit1 = vars(level);
                        return Vars;
                    }
                    else
                    {
                        fprintf(stderr, "Error. No semicolon token found in <vars>.\n");
                        exit(0);
                    }
                }
                else
                {
                    fprintf(stderr, "Error. No integer token found for <vars>.\n");
                    exit(0);
                }
            }
            else
            {
                fprintf(stderr,"Error. No colon operator token found for <vars>.\n");
                exit(0);
            }
        }
        else
        {
            fprintf(stderr, "Error. No identifier token found for <vars>.\n");
            exit(0);
        }
    }
    else
    {
        return NULL;
    }
}
//<exp>-> <A> ++ <exp> | <A>++ (one token)
BNF* expr(int level)
{
    level++;
    BNF* Exp = getDef("<exp>", level);
    Exp->unit1=A(level);

    if(tk.tkn == increm)
    {   
        Exp->tkn1 = tk;
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Exp->bnfName);
        newline();
        Exp->unit2 = expr(level);
        return Exp;
    }
    else
    {
        return Exp;
    }
    return Exp;
}
//<A> -> <N> -- <A> | <N> (-- one token)
BNF* A(int level)
{
    level++;
    BNF *a = getDef("<A>", level);
    a->unit1 = N(level);

    if(tk.tkn == decrem)
    {
        a->tkn1 = tk;
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, a->bnfName);
        newline();
        a->unit2 = A(level);
        return a;
    }
    else
    {
        return a;
    }
    return a;
}
// <N> -> <M> / <N> | <M> * <N> | <M>
BNF* N(int level)
{
    level++;
    BNF *n = getDef("<N>", level);
    n->unit1 = M(level);

    if(tk.tkn == op && (strstr(tk.tokenInstance, "/")!=NULL || strstr(tk.tokenInstance, "*")!=NULL))
    {
        n->tkn1 = tk;
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, n->bnfName);
        newline();
        n->unit2 = N(level);
        return n;
    }
    else
    {
        return n;
    }
    return n;
}
// <M> -> -- <M> | <R> (-- one token)
BNF* M(int level)
{
    level++;
    BNF *m = getDef("<M>", level);
    if(tk.tkn == decrem)
    {
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, m->bnfName);
        newline();
        m->unit1 = M(level);
        return m;
    }
    else
    {
        m->unit1 = R(level);
        return m;
    }
    return m;
}
// <R> -> [ <exp> ] | Identifier | integer
BNF* R(int level)
{
    level++;
    BNF *r = getDef("<R>", level);

    if(tk.tkn == punc && strstr(tk.tokenInstance, "[") != NULL)
    {
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, r->bnfName);
        newline();
        r->unit1 = expr(level);
        if (tk.tkn == punc && strstr(tk.tokenInstance, "]") != NULL)
        {
            tk = scanner();
            fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, r->bnfName);
            newline();
            return r;
        }
        else
        {
            fprintf(stderr, "Error. No closing ']' was identified.\n");
            exit(0);
        }
        
    }
    else if(tk.tkn == idKw && strstr(tk.tokenInstance, "begin")==NULL && strstr(tk.tokenInstance, "end")==NULL && strstr(tk.tokenInstance, "var")==NULL && strstr(tk.tokenInstance, "fork")==NULL && strstr(tk.tokenInstance, "loop")==NULL && strstr(tk.tokenInstance, "then")==NULL && strstr(tk.tokenInstance, "scan")==NULL && strstr(tk.tokenInstance, "print") ==NULL)
    {
        r->tkn1 = tk;
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, r->bnfName);
        newline();
        return r;
    }
    else if(tk.tkn == intg)
    {
        r->tkn1 = tk;
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, r->bnfName);
        newline();
        return r;
    }
    else
    {
        fprintf(stderr, "Error. No proper token identified for <R>.\n");
        exit(0);
    }
}
//<stats> -> <stat> <mStat>
BNF *stats(int level)
{
    level++;
    BNF *Stats = getDef("<stats>", level);

    Stats->unit1 = stat(level);
    Stats->unit2 = mStat(level);
    return Stats;
}
//<mStat> -> empty | <stat> <mStat>
BNF *mStat(int level)
{
    level++;
    BNF *mstat = getDef("<mStat>", level);

    if(tk.tkn == idKw && strstr(tk.tokenInstance, "end") ==NULL)
    {
        newline();
        mstat->unit1 = stat(level);
        mstat->unit2 = mStat(level);
        return mstat;   
    }
    else
    {
        return NULL;
    }
}
//<stat> -> <in>; | <out>; | <block> | <if>; | <loop>; | <assign>;
BNF* stat(int level)
{
    level++;
    BNF *Stat = getDef("<stat>", level);

    if(tk.tkn == idKw)
    {
        if(strstr(tk.tokenInstance, "scan") != NULL)
        {
            Stat->unit1 = in(level);
        }
        else if (strstr(tk.tokenInstance, "print") != NULL)
        {
            Stat->unit1 = out(level);
        }
        else if(strstr(tk.tokenInstance, "begin") != NULL)
        {
            Stat->unit1 = block(level);
            return Stat;
        }
        else if(strstr(tk.tokenInstance, "fork") !=NULL)
        {
            Stat->unit1 = iff(level);
        }
        else if(strstr(tk.tokenInstance, "loop") != NULL)
        {
            Stat->unit1 = loop(level);
        }
        else if(strstr(tk.tokenInstance, "begin") == NULL && strstr(tk.tokenInstance, "end") == NULL && strstr(tk.tokenInstance, "then") == NULL)
        {
            Stat->unit1 = Assign(level);
        }
        else
        {
            fprintf(stderr, "Error. No proper BNF definition found inside of <stat>.\n");
            exit(0);
        }
        if(tk.tkn == punc && strstr(tk.tokenInstance, ";") != NULL)
        {
            tk = scanner();
            fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Stat->bnfName);
            newline();
            return Stat;
        }
        else
        {
            fprintf(stderr, "Error. No terminating semicolon token was identified.\n");
            exit(0);
        }
    }
    else
    {
        fprintf(stderr, "Error. No identifier or kewyord token was identified in <stat>.\n");
        exit(0);
    }
}
//<in> -> scan Identifier
BNF* in(int level)
{
    level++;
    BNF* In = getDef("<in>", level);

    if(tk.tkn == idKw && strstr(tk.tokenInstance, "scan") !=NULL)
    {
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, In->bnfName);
        newline();
        if(tk.tkn == idKw && strstr(tk.tokenInstance, "begin")==NULL && strstr(tk.tokenInstance, "end")==NULL && strstr(tk.tokenInstance, "var")==NULL && strstr(tk.tokenInstance, "fork")==NULL && strstr(tk.tokenInstance, "loop")==NULL && strstr(tk.tokenInstance, "then")==NULL && strstr(tk.tokenInstance, "scan")==NULL && strstr(tk.tokenInstance, "print") ==NULL)
        {
            In->tkn1 = tk;
            tk = scanner();
            fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, In->bnfName);
            newline();
            return In;
        }
        else
        {
            fprintf(stderr, "Error. No identifer token was found for <in>.\n");
            exit(0);
        }
    }
    else
    {
        fprintf(stderr, "Error. No scan token was found for <in>.\n");
        exit(0);
    }
}
//<out> -> print (<exp>)
BNF *out(int level)
{
    level++;
    BNF *Out = getDef("<out>", level);
    
    if(tk.tkn == idKw && strstr(tk.tokenInstance, "print")!=NULL)
    {
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Out->bnfName);
        newline();
        if(tk.tkn == punc && strstr(tk.tokenInstance, "(") != NULL)
        {
            tk = scanner();
            fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Out->bnfName);
            newline();
            Out->unit1 = expr(level);
            if(tk.tkn == punc && strstr(tk.tokenInstance, ")")!=NULL)
            {
                tk = scanner();
                fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Out->bnfName);
                newline();
                return Out;
            }
            else
            {
                fprintf(stderr, "Error. No closing ')' token found for <out>.\n");
                exit(0);
            }
        }
        else
        {
            fprintf(stderr, "Error. No opening '(' token found for <out>.\n");
            exit(0);
        }
    }
    else
    {
        fprintf(stderr, "Error. No print token found for <out>.\n");
        exit(0);
    }
}
//<if> -> fork (<exp> <RO> <exp>) then <stat>
BNF* iff(int level)
{
    level++;
    BNF *If = getDef("<if>", level);

    if(tk.tkn == idKw && strstr(tk.tokenInstance, "fork") != NULL)
    {
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, If->bnfName);
        newline();
        if(tk.tkn == punc && strstr(tk.tokenInstance, "(") != NULL)
        {
            tk=scanner();
            fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, If->bnfName);
            newline();
            If->unit1 = expr(level);
            If->unit2 = RO(level);
            If->unit3 = expr(level);
            if(tk.tkn == punc && strstr(tk.tokenInstance, ")") != NULL)
            {
                tk=scanner();
                fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, If->bnfName);
                newline();
                if(tk.tkn == idKw && strstr(tk.tokenInstance, "then") !=NULL)
                {
                    tk = scanner();
                    fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, If->bnfName);
                    newline();
                    If->unit4 = stat(level);
                    return If;
                }
                else
                {
                    fprintf(stderr, "Error. No closing 'then' token was found for <if>.\n");
                    exit(0);
                }
            }
            else
            {
                fprintf(stderr, "Error. No ')' token was found for <if>.\n");
                exit(0);
            }
        }
        else
        {
            fprintf(stderr, "Error. No '(' token was found for <if>.\n");
            exit(0);
        }
    }
    else
    {
        fprintf(stderr, "Error. No 'fork' keyword token was found for <if>.\n");
        exit(0);
    }
}
//<loop> -> loop ( <exp> <RO> <exp> ) <stat>
BNF* loop(int level)
{
    level++;
    BNF *Loop = getDef("<if>", level);

    if(tk.tkn == idKw && strstr(tk.tokenInstance, "loop") != NULL)
    {
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Loop->bnfName);
        newline();
        if(tk.tkn == punc && strstr(tk.tokenInstance, "(") != NULL)
        {
            tk=scanner();
            fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Loop->bnfName);
            newline();
            Loop->unit1 = expr(level);
            Loop->unit2 = RO(level);
            Loop->unit3 = expr(level);
            if(tk.tkn == punc && strstr(tk.tokenInstance, ")") != NULL)
            {
                tk=scanner();
                fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, Loop->bnfName);
                newline();
                Loop->unit4 = stat(level);
                return Loop;
            }
            else
            {
                fprintf(stderr, "Error. No ')' token was found for <if>.\n");
                exit(0);
            }
        }
        else
        {
            fprintf(stderr, "Error. No '(' token was found for <if>.\n");
            exit(0);
        }
    }
    else
    {
        fprintf(stderr, "Error. No 'loop' keyword token was found for <if>.\n");
        exit(0);
    }
}
//<assign> -> Identifier == <exp> (one == token)
BNF *Assign(int level)
{
    level++;
    BNF *assign = getDef("<assign>", level);

    if(tk.tkn == idKw && strstr(tk.tokenInstance, "begin")==NULL && strstr(tk.tokenInstance, "end")==NULL && strstr(tk.tokenInstance, "var")==NULL && strstr(tk.tokenInstance, "fork")==NULL && strstr(tk.tokenInstance, "loop")==NULL && strstr(tk.tokenInstance, "then")==NULL && strstr(tk.tokenInstance, "scan")==NULL && strstr(tk.tokenInstance, "print") ==NULL)
    {
        assign->tkn1 = tk;
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, assign->bnfName);
        newline();
        if(tk.tkn == equality)
        {
            tk = scanner();
            fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, assign->bnfName);
            newline();
            assign->unit1 = expr(level);
            return assign;
        }
        else
        {
            fprintf(stderr, "Error. No equality token found for <assign>.\n");
            exit(0);
        }
    }
    else
    {
        fprintf(stderr, "Error. No identifier token found for <assign>.\n");
        exit(0);
    }
}
//<RO> -> <= (one token) | >= (one token)| = | %
BNF *RO(int level)
{
    level++;
    BNF *rO = getDef("<RO>", level);

    if(tk.tkn == lthaneq || tk.tkn == gthaneq || tk.tkn == assign || (tk.tkn == op && strstr(tk.tokenInstance, "%%") != NULL))
    {
        rO->tkn1 = tk;
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d, BNF: %s\n", tk.tokenInstance, tk.tkn, rO->bnfName);
        newline();
        return rO;
    }
    else
    {
        fprintf(stderr, "No relational or modulus operator found for <RO>.\n");
        exit(0);
    }
}

void newline()
{
    if(tk.tkn == newLine)
    {
        tk = scanner();
        fprintf(stderr, "Token: %s, type: %d\n", tk.tokenInstance, tk.tkn);
        newline();
    }
}
