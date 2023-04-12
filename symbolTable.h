/*  Group No : 31
    Anushka Jain 2019B1A70904P
    Ritu Chandna 2019B4A70667P
    Payal Basrani 2019B5A70809P
    Ujjwal Jain 2019B4A70647P
    Shobhit Jain 2019B3A70385P
*/

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "symbolTableDef.h"

int hashFunc(const char *key);
void* search_table(hashTable t, char name[LEXEME_LEN]);
void insert_table(hashTable t, char name[LEXEME_LEN], data_ptr* ptr);
char* concatStr(char* s1, char* s2);
char* concatStr2(char* s1, char* s2);
data_ptr* generateType(nodeAST node);
data_ptr* funcEntry(symbolTable* SymTable, char idname[LEXEME_LEN], nodeAST inList, nodeAST outList, int lineNo);
bool scope_change(Label l);
bool isDeclared(nodeAST node);
bool checkOPList(char varName[LEXEME_LEN], char fname[LEXEME_LEN]);
void IDEntry(symbolTable* table, nodeAST node);
data_ptr* check_para(list* iplist, list* oplist, char lexeme[LEXEME_LEN], bool *isOuPara);
data_ptr* return_from_parent(nodeAST node, symbolTable* table, bool* isOutPara);
data_ptr* return_type(nodeAST node, symbolTable* table, bool* isOutPara);
int expressionType(nodeAST node);
int checkSwitch(nodeAST node, data_ptr* ptr);
void constructSymbolTable(nodeAST root);
void semantic_check(nodeAST root);
void printSymbolTable(symbolTable* table);
void printSymbolTable2(symbolTable *table);
void printAST(nodeAST root);




#endif