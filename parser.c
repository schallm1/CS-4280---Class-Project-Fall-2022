#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"

BNF* parser()
{
    tk = scanner();
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
    //begin token
    if(tk.tkn == idKw && strstr(tk.tokenInstance,"begin")!=NULL)
    {  
        Block->leftTkn1 = tk;
        tk=scanner();
        newline();
        Block->unit1 = vars(level);
        Block->unit2 = stats(level);
        //end token
        if(tk.tkn == idKw && strstr(tk.tokenInstance, "end")!=NULL)
        {
            Block->rightTkn2 = tk;
            tk = scanner();
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
    //var token
    if(tk.tkn == idKw && strstr(tk.tokenInstance, "var")!=NULL)
    {
        Vars->leftTkn1 = tk;
        tk=scanner();
        newline();
        //id token
        if(tk.tkn == idKw && strstr(tk.tokenInstance, "begin")==NULL && strstr(tk.tokenInstance, "end")==NULL && strstr(tk.tokenInstance, "var")==NULL && strstr(tk.tokenInstance, "fork")==NULL && strstr(tk.tokenInstance, "loop")==NULL && strstr(tk.tokenInstance, "then")==NULL && strstr(tk.tokenInstance, "scan")==NULL && strstr(tk.tokenInstance, "print") ==NULL)
        {
            Vars->leftTkn2 = tk;
            tk = scanner();
            newline();
            if(tk.tkn == punc && strstr(tk.tokenInstance,":")!=NULL)
            {
                Vars->leftTkn3 = tk;
                tk = scanner();
                newline();
                //integer token
                if(tk.tkn == intg)
                {
                    Vars->leftTkn4 = tk;
                    tk=scanner();
                    newline();
                    if(tk.tkn == punc && strstr(tk.tokenInstance, ";") != NULL)
                    {
                        Vars->leftTkn5 = tk;
                        tk=scanner();
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
        Exp->rightTkn1 = tk;
        tk = scanner();
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
        a->rightTkn1 = tk;
        tk = scanner();
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
        n->rightTkn1 = tk;
        tk = scanner();
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
        m->leftTkn1 = tk;
        tk = scanner();
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
        r->leftTkn1 = tk;
        tk = scanner();
        newline();
        r->unit1 = expr(level);
        if (tk.tkn == punc && strstr(tk.tokenInstance, "]") != NULL)
        {
            r->rightTkn1 = tk;
            tk = scanner();
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
        r->leftTkn1 = tk;
        tk = scanner();
        newline();
        return r;
    }
    else if(tk.tkn == intg)
    {
        r->leftTkn1 = tk;
        tk = scanner();
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
            Stat->rightTkn1 = tk;
            tk = scanner();
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
        In->leftTkn1 = tk;
        tk = scanner();
        newline();
        if(tk.tkn == idKw && strstr(tk.tokenInstance, "begin")==NULL && strstr(tk.tokenInstance, "end")==NULL && strstr(tk.tokenInstance, "var")==NULL && strstr(tk.tokenInstance, "fork")==NULL && strstr(tk.tokenInstance, "loop")==NULL && strstr(tk.tokenInstance, "then")==NULL && strstr(tk.tokenInstance, "scan")==NULL && strstr(tk.tokenInstance, "print") ==NULL)
        {
            In->leftTkn2 = tk;
            tk = scanner();
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
        Out->leftTkn1 = tk;
        tk = scanner();
        newline();
        if(tk.tkn == punc && strstr(tk.tokenInstance, "(") != NULL)
        {
            Out->leftTkn2 = tk;
            tk = scanner();
            newline();
            Out->unit1 = expr(level);
            if(tk.tkn == punc && strstr(tk.tokenInstance, ")")!=NULL)
            {
                Out->rightTkn1 = tk;
                tk = scanner();
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
        If->leftTkn1 = tk;
        tk = scanner();
        newline();
        if(tk.tkn == punc && strstr(tk.tokenInstance, "(") != NULL)
        {
            If->leftTkn2 = tk;
            tk=scanner();
            newline();
            If->unit1 = expr(level);
            If->unit2 = RO(level);
            If->unit3 = expr(level);
            if(tk.tkn == punc && strstr(tk.tokenInstance, ")") != NULL)
            {
                If->rightTkn3 = tk;
                tk=scanner();
                newline();
                if(tk.tkn == idKw && strstr(tk.tokenInstance, "then") !=NULL)
                {
                    If->midTkn4 = tk;
                    tk = scanner();
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
        Loop->leftTkn1 = tk;
        tk = scanner();
        newline();
        if(tk.tkn == punc && strstr(tk.tokenInstance, "(") != NULL)
        {
            Loop->leftTkn2 = tk;
            tk=scanner();
            newline();
            Loop->unit1 = expr(level);
            Loop->unit2 = RO(level);
            Loop->unit3 = expr(level);
            if(tk.tkn == punc && strstr(tk.tokenInstance, ")") != NULL)
            {
                Loop->rightTkn3 = tk;
                tk=scanner();
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
        assign->leftTkn1 = tk;
        tk = scanner();
        newline();
        if(tk.tkn == equality)
        {
            assign->leftTkn2 = tk;
            tk = scanner();
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
        rO->leftTkn1 = tk;
        tk = scanner();
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
        newline();
    }
}
