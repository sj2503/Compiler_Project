/*  Group No : 31
    Anushka Jain 2019B1A70904P
    Ritu Chandna 2019B4A70667P
    Payal Basrani 2019B5A70809P
    Ujjwal Jain 2019B4A70647P
    Shobhit Jain 2019B3A70385P
*/

#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

// extern FILE *file_ptr;
// extern int state;
// extern unsigned short lNo;
// extern unsigned short tokenCntr;
// extern char *text;
// extern char *text_twin;
// extern int begin;
// extern int forward;
// extern hashItem *keywordTable[KEYWORDSIZE];
// extern Error errors[100];
// extern int errorcntr;
// extern bool tokenised;
// extern tokenInfo *token;
// extern bool isEOF;
// extern bool isLexError;

void tokenize(int begin, int forward, char *buf, char *tokenName, char *dataType);
void fillKeywordTable();
void putHashPair(char *key, char *value);
char *getHashPair(char *key);
unsigned int hashVal(const char *key);
tokenInfo *getNextToken();
void remove_comments(char *testcaseFile, char *cleanFile);
void error_function();
void error_printing();

#endif