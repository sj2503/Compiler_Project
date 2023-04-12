/*  Group No : 31
    Anushka Jain 2019B1A70904P
    Ritu Chandna 2019B4A70667P
    Payal Basrani 2019B5A70809P
    Ujjwal Jain 2019B4A70647P
    Shobhit Jain 2019B3A70385P
*/

#ifndef AST_H
#define AST_H

#include "astDef.h"

nodeAST new_node(Label name, int rule_No, nodeAST isExist, nodeAST parent, nodeAST child, nodeAST sibling, nodeAST list_head, tokenInfo* info);
treeNode* parseChild(treeNode* parent, Term term, int occurance);
nodeAST constructAST(treeNode* parseNode, nodeAST parent);
void print_ast(nodeAST root);
nodeAST ast_main(treeNode* parseTreeRoot);



#endif
