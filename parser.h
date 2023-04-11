/*  Group No : 31
    Anushka Jain 2019B1A70904P
    Ritu Chandna 2019B4A70667P
    Payal Basrani 2019B5A70809P
    Ujjwal Jain 2019B4A70647P
    Shobhit Jain 2019B3A70385P
*/

#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

// extern char *Termstr[];
// extern int parseTable[COUNT_NT][COUNT_T - 1]; // removing epsilon from count of terminals
// extern unsigned long long int first[COUNT_NT];
// extern unsigned long long int follow[COUNT_NT];
// extern gram *gramArr[NRULES];
// extern char parseTable2[COUNT_NT][COUNT_T - 1][30];
// extern char firstset[COUNT_NT][150];
// extern char followset[COUNT_NT][150];
// extern FILE *fileParseTree;
// bool isError = false;
// Term syncSet[SYNC_NUM] = {SEMICOL, START, statements, ENDDEF};

void initializeStack(Stack *s);
void pushStack(Stack *s, Term data, treeNode *addre);
bool isStackEmpty(Stack *s);
void popStack(Stack *s);
StackEle *getTopStack(Stack *s);
int stringToENUM(char *str);
unsigned long long int calUnion(unsigned long long int x, unsigned long long int y);
int terminalInSet(unsigned long long int x, int term);
unsigned long long int findFirst(Term y);
unsigned long long int firstRHS(gram *node);
unsigned long long int findFollow(Term y);
void findFirstAndFollow();
void printFirstAndFollow();
void generateGram(char *file);
void printGramTable();
void buildParseTable();
void treeTraversal(treeNode *root, FILE *fp);
void printParseTree(treeNode *root);
void printtreeNode(treeNode *root, FILE *fp);
void buildParseTable2();
tokenInfo *errorHandling(int errNo, Stack *st, tokenInfo *L);
void errorReport(int errNo, Stack *st, tokenInfo *L);
treeNode* stackImp();

#endif