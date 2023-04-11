/*  Group No : 31
    Anushka Jain 2019B1A70904P
    Ritu Chandna 2019B4A70667P
    Payal Basrani 2019B5A70809P
    Ujjwal Jain 2019B4A70647P
    Shobhit Jain 2019B3A70385P
*/

#ifndef PARSERDEF_H
#define PARSERDEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "lexer.c"

#define COUNT_NTandT 129 // total number of terminals and non-terminals
#define COUNT_T 58
#define COUNT_NT 71
#define NRULES 137
#define SYNC_NUM 5

typedef enum Term
{
    EPSILON,
    DOLLAR,
    DECLARE,
    MODULE,
    SEMICOL,
    DRIVERDEF,
    DRIVER,
    PROGRAM,
    DRIVERENDDEF,
    DEF,
    ID,
    ENDDEF,
    TAKES,
    INPUT,
    SQBO,
    SQBC,
    RETURNS,
    COMMA,
    COLON,
    INTEGER,
    REAL,
    BOOLEAN,
    ARRAY,
    OF,
    START,
    END,
    GET_VALUE,
    BO,
    BC,
    PRINT,
    NUM,
    RNUM,
    ASSIGNOP,
    USE,
    WITH,
    PARAMETERS,
    PLUS,
    MINUS,
    MUL,
    DIV,
    LT,
    LE,
    GT,
    GE,
    EQ,
    NE,
    AND,
    OR,
    SWITCH,
    CASE,
    BREAK,
    TRUE,
    FALSE,
    DEFAULT,
    FOR,
    IN,
    WHILE,
    RANGEOP,

    // non terminals start here
    program,
    moduleDeclarations,
    moduleDeclaration,
    otherModules,
    driverModule,
    module,
    ret,
    input_plist,
    input_plist2,
    output_plist,
    output_plist2,
    dataType,
    range_arr,
    index_arr,
    ind,
    sign,
    type,
    moduleDef,
    statements,
    statement,
    ioStmt,
    var2,
    boool,
    whichID,
    simpleStmt,
    assignmentStmt,
    whichStmt,
    lvalueIDStmt,
    lvalueArrStmt,
    moduleReuseStmt,
    optional,
    paraList,
    paraList2,
    var,
    idList,
    idList2,
    expression,
    arithmeticOrBoolean,
    term1,
    arithmeticExpr,
    arithmeticExpr2,
    term2,
    term3,
    factor,
    whichID2,
    relationalExpr,
    relationalOP,
    logicalExpr,
    logicalOP,
    op1,
    op2,
    unaryExpr,
    new_NT,
    unaryOP,
    idNumRnum,
    arrExpr,
    term6,
    arithmeticExprArr,
    arithmeticExprArr2,
    term4,
    term5,
    factor2,
    declareStmt,
    conditionalStmt,
    caseStmts,
    caseStmts2,
    value,
    default1,
    iterativeStmt,
    range_loop,
    index_loop
} Term;

typedef struct treeNode treeNode;
typedef struct treeNode
{
    Term name;
    int ruleNo;
    treeNode *child;
    treeNode *sibling;
    treeNode *parent;
    tokenInfo *TI;
} treeNode;

typedef struct StackEle
{
    Term term;
    treeNode *addr;
    struct StackEle *next;
} StackEle;

typedef struct Stack
{
    StackEle *top;
} Stack;

typedef struct grammar
{
    Term name;
    char strName[50];
    int type; // 0 for non-teminals and 1 for terminals
    struct grammar *next;
} gram;

#endif
