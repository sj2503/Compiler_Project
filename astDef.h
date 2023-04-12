/*  Group No : 31
    Anushka Jain 2019B1A70904P
    Ritu Chandna 2019B4A70667P
    Payal Basrani 2019B5A70809P
    Ujjwal Jain 2019B4A70647P
    Shobhit Jain 2019B3A70385P
*/

#ifndef ASTDEF_H
#define ASTDEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.c"

#define COUNT_L 49
#define LEXEME_LEN 21
#define TABLE_SIZE 100

char* Label_str[COUNT_L]={
        "L_PROGRAM",
        "L_MODULEDECLARATIONS",
        "L_OTHERMODULES",
        "L_DRIVERMODULE", 
        "L_FOR", 
        "L_WHILE",
        "L_MODULE",
        "L_DEFAULT",
        "L_OUTPUT_PLIST", 
        "L_INPUT_PLIST", 
        "L_STATEMENTS", 
        "L_GET_VALUE", 
        "L_PRINT", 
        "L_ARRAY_ACCESS",        
        "L_ASSIGN", 
        "L_ARRAY_ASSIGN", 
        "L_MODULEREUSESTATEMENT",
        "L_PARALIST", 
        "L_IDLIST", 
        "L_UNARYEXPR", 
        "L_ARRAY_INDEX", 
        "L_DECLARE", 
        "L_SWITCH", 
        "L_CASESTMTS", 
        "L_CASE",       
        "L_VARIABLE", 
        "L_ARRAY", 
        "L_RANGE", 
        "L_ELEMENT", 

        "L_INTEGER", //terminals start here at 29
        "L_REAL", 
        "L_BOOLEAN",    
        "L_NUM", 
        "L_PLUS", 
        "L_MINUS",
        "L_LT", 
        "L_LE", 
        "L_GT", 
        "L_GE", 
        "L_NE", 
        "L_EQ", 
        "L_AND", 
        "L_OR", 
        "L_MUL", 
        "L_DIV",
        "L_RNUM", 
        "L_TRUE", 
        "L_FALSE",
        "L_ID"
};

typedef enum Label{
        L_PROGRAM,
        L_MODULEDECLARATIONS,
        L_OTHERMODULES,
        L_DRIVERMODULE, 
        L_FOR, 
        L_WHILE,
        L_MODULE,
        L_DEFAULT,
        L_OUTPUT_PLIST, 
        L_INPUT_PLIST, 
        L_STATEMENTS, 
        L_GET_VALUE, 
        L_PRINT, 
        L_ARRAY_ACCESS, 
        L_ASSIGN, 
        L_ARRAY_ASSIGN, 
        L_MODULEREUSESTATEMENT,
        L_PARALIST, 
        L_IDLIST,
        L_UNARYEXPR, 
        L_ARRAY_INDEX, 
        L_DECLARE, 
        L_SWITCH, 
        L_CASESTMTS, 
        L_CASE, 
        L_VARIABLE, 
        L_ARRAY, 
        L_RANGE, 
        L_ELEMENT, 

        L_INTEGER, //terminals start here at 29
        L_REAL, 
        L_BOOLEAN, 
        L_NUM, 
        L_PLUS, 
        L_MINUS,
        L_LT, 
        L_LE, 
        L_GT, 
        L_GE, 
        L_NE, 
        L_EQ, 
        L_AND, 
        L_OR, 
        L_MUL, 
        L_DIV,
        L_RNUM, 
        L_TRUE, 
        L_FALSE,
        L_ID
} Label;

typedef struct astStruct astStruct;
typedef struct data_ptr data_ptr;
typedef struct symbolTable symbolTable;
typedef struct hashEntry hashEntry;
typedef struct whileEx whileEx;
typedef struct astStruct
{
    int ruleNo;
    astStruct* parent;
    astStruct* child;
    astStruct* sibling;
    astStruct* list_head;
    Label name;
    tokenInfo *TI;
    symbolTable *ptrTable;
    char funcName[50];
}astStruct;

typedef struct astStruct* nodeAST;


typedef struct hashEntry
{
    char name[LEXEME_LEN];
    void *entry;
    hashEntry *next;
} hashEntry;

typedef hashEntry* hashElem;
typedef hashElem hashTable[TABLE_SIZE];


typedef struct symbolTable
{
    hashTable localTable;
    int level;
    symbolTable *parent;
    symbolTable *child;
    // symbolTable *lastChild;
    symbolTable *sibling;
}symbolTable;

symbolTable* currSym;
symbolTable* mainSym;
whileEx* whileList;

typedef struct list list;

typedef struct list
{
    data_ptr* type;
    char lexeme[LEXEME_LEN];
    list *next;
//     bool isAssigned=false;
}list;
typedef struct whileEx
{
    data_ptr* type;
    whileEx* next;
    int lineNo;
    bool isAssigned;
}whileEx;

typedef struct
{
    // char funcName[LEXEME_LEN];
    list *ipList;
    list *opList;
    bool isDeclared;
    bool isDefined;
    symbolTable* funcTable;

}funcType;


typedef struct{
    Label dataType;
    char parentFunc[LEXEME_LEN];
    struct isDynamic
    {
        bool starInd;
        bool endInd;
    } isDynamic;
    
    union{
        int num;
        char lex[LEXEME_LEN];
    }start_ind;

    bool start_ind_neg;
    bool end_ind_neg;

    union{
        int num;
        char lex[LEXEME_LEN];
    }end_ind;
}idType;

typedef struct data_ptr
{
    Label name;
    char tokName[LEXEME_LEN];
    funcType* func;
    idType* var;
    bool isAssigned;
    int width;
    int offset;
    int line_no;
}data_ptr;

#endif