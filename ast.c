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

nodeAST new_node(Label name, int rule_No, nodeAST isExist, nodeAST parent, nodeAST child, nodeAST sibling, nodeAST list_head, tokenInfo* info)
{
    nodeAST node= (nodeAST) malloc(sizeof(astStruct));
    node->name = name;
    node->ruleNo = rule_No;
    node->parent = parent;
    node->child = child;
    node->sibling = sibling;
    node->list_head= list_head;
    node->TI = info;

    return node;
}

treeNode* parseChild(treeNode* parent, Term term, int occurance)
{
    treeNode* temp = parent->child;
    int counter = 0;
    while (temp!=NULL && counter<occurance)
    {
        if(temp->name == term){
            counter++;
        }
        if(counter==occurance){
            break;
        }
        temp = temp->sibling;
    }
    return temp;   
}

nodeAST constructAST(treeNode* parseNode, nodeAST parent)
{
    if(parseNode == NULL){
        return NULL;
    }

    int rule_No = parseNode->child->ruleNo; 
    nodeAST synNode, inhNode, tempNode, node, temp;
    treeNode *ptnode;
//     printf("CASE 0");
    switch(rule_No)
    {
        case 0: 
                // printf("CASE 0");
                node = new_node(L_PROGRAM, rule_No, NULL, parent, NULL, NULL, NULL, NULL);
                //moduledeclaration
                ptnode = parseChild(parseNode, moduleDeclarations, 1);
                synNode = new_node(L_MODULEDECLARATIONS, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, NULL);
                node->child = synNode;
                constructAST(ptnode, synNode);
                //othermodules
                ptnode = parseChild(parseNode, otherModules, 1);
                synNode->sibling = new_node(L_OTHERMODULES, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, NULL);
                synNode = synNode->sibling;
                constructAST(ptnode, synNode);
                //drivermodule
                ptnode = parseChild(parseNode, driverModule, 1);
                synNode->sibling = new_node(L_DRIVERMODULE, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, NULL);
                synNode = synNode->sibling;
                synNode->child= constructAST(ptnode, synNode);
                //othermodules
                ptnode = parseChild(parseNode, otherModules, 2);
                synNode->sibling = new_node(L_OTHERMODULES, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, NULL);
                synNode = synNode->sibling;
                constructAST(ptnode, synNode);
                break;
                
        case 1:
                // printf("CASE 1");
                ptnode= parseChild(parseNode, moduleDeclarations, 1);
                node= constructAST(ptnode, parent);
                ptnode = parseChild(parseNode, moduleDeclaration, 1);
                temp = node->list_head;
                node->list_head = constructAST(ptnode, parent);
                node->list_head->sibling = temp;
                break;

        case 2:
                parent->list_head = NULL;
                node = parent; 
                break;

        case 3:
                ptnode = parseChild(parseNode, ID, 1);
                node = new_node(L_ID, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 4: 
                ptnode= parseChild(parseNode, otherModules, 1);
                node= constructAST(ptnode, parent);
                ptnode = parseChild(parseNode, module, 1);
                temp = node->list_head;
                node->list_head = constructAST(ptnode, parent);
                node->list_head->sibling = temp;
                break;

        case 5: 
                parent->list_head = NULL;
                node = parent; 
                break;
        
        case 6:
                
                ptnode = parseChild(parseNode, moduleDef, 1);
                node= constructAST(ptnode, parent);
                break;

        case 7:
                // ptnode= parseChild(parseNode, module, 1);
                node= new_node(L_MODULE, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);  
                ptnode= parseChild(parseNode, ID, 1);
                synNode= new_node(L_ID, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                node->child= synNode;
                ptnode= parseChild(parseNode, input_plist, 1);
                synNode->sibling= constructAST(ptnode, node);
                ptnode= parseChild(parseNode, ret, 1);
                synNode= synNode->sibling;
                synNode->sibling= constructAST(ptnode, node);
                ptnode= parseChild(parseNode, moduleDef, 1);
                synNode= synNode->sibling;
                synNode->sibling= constructAST(ptnode, node);
                break;

        case 8:
                //ptnode= parseChild(parseNode, ret, 1);
                node = new_node(L_OUTPUT_PLIST, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, output_plist , 1);
                node = constructAST(ptnode, node);
                break;
        
        case 9: 
                node = new_node(L_OUTPUT_PLIST, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                break;

        case 10: 
                node = new_node(L_INPUT_PLIST, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                tempNode = new_node(L_VARIABLE, parseNode->ruleNo, NULL, node, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, ID , 1);
                synNode = new_node(L_ID, ptnode->ruleNo, NULL, tempNode, NULL, NULL, NULL, ptnode->TI);
                tempNode->child= synNode;
                ptnode= parseChild(parseNode, dataType, 1);
                synNode->sibling= constructAST(ptnode, tempNode);
                ptnode= parseChild(parseNode, input_plist2, 1);
                node = constructAST(ptnode, node);
                temp = node->list_head;
                node->list_head = tempNode;
                node->list_head->sibling = temp;
                break;

        case 11: 
                ptnode= parseChild(parseNode, input_plist2, 1);
                node = constructAST(ptnode, parent);
                temp = node->list_head;
                tempNode = new_node(L_VARIABLE, rule_No, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, ID, 1);
                tempNode->child = new_node(L_ID, ptnode->ruleNo, NULL, tempNode, NULL, NULL, NULL, ptnode->TI);
                synNode = tempNode->child;
                ptnode = parseChild(parseNode, dataType, 1);
                synNode->sibling = constructAST(ptnode, tempNode);
                node->list_head = tempNode;
                node->list_head->sibling = temp;
                break;

        case 12:
                node=parent;
                node->list_head= NULL;
                break;

        case 13:
                node= parent;
                tempNode = new_node(L_VARIABLE, rule_No, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, ID , 1);
                synNode = new_node(L_ID, ptnode->ruleNo, NULL, tempNode, NULL, NULL, NULL, ptnode->TI);
                tempNode->child= synNode;
                ptnode= parseChild(parseNode, dataType, 1);
                synNode->sibling= constructAST(ptnode, tempNode);
                ptnode= parseChild(parseNode, output_plist2, 1);
                node = constructAST(ptnode, node);
                temp = node->list_head;
                node->list_head = tempNode;
                node->list_head->sibling = temp;
                break;
        
        case 14:
                ptnode= parseChild(parseNode, output_plist2, 1);
                node = constructAST(ptnode, parent);
                nodeAST temp = node->list_head;
                tempNode = new_node(L_VARIABLE, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, ID, 1);
                tempNode->child = new_node(L_ID, ptnode->ruleNo, NULL, tempNode, NULL, NULL, NULL, ptnode->TI);
                synNode = tempNode->child;
                ptnode = parseChild(parseNode, dataType, 1);
                synNode->sibling = constructAST(ptnode, tempNode);
                node->list_head = tempNode;
                node->list_head->sibling = temp;
                break;

        case 15:
                node=parent;
                node->list_head= NULL;
                break;
        
        case 16:
                ptnode= parseChild(parseNode, INTEGER, 1);
                node = new_node(L_INTEGER, rule_No, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 17:
                ptnode= parseChild(parseNode, REAL, 1);
                node = new_node(L_REAL, rule_No, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 18:
                ptnode= parseChild(parseNode, BOOLEAN, 1);
                node = new_node(L_BOOLEAN, rule_No, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                break;
                
        case 19:
                node = new_node(L_ARRAY, rule_No, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode= parseChild(parseNode, range_arr, 1); 
                node->child= constructAST(ptnode, node);
                ptnode= parseChild(parseNode, type, 1); 
                node->child->sibling= constructAST(ptnode, node);
                break;
        case 20:
                node = new_node(L_RANGE, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode= parseChild(parseNode, index_arr, 1); 
                node->child= constructAST(ptnode, node);
                ptnode= parseChild(parseNode, index_arr, 2); 
                node->child->sibling= constructAST(ptnode, node);
                break;

        case 21:
                node = new_node(L_ELEMENT, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode= parseChild(parseNode, sign, 1); 
                node->child= constructAST(ptnode, node);
                ptnode= parseChild(parseNode, ind, 1);
                if(node->child){
                        node->child->sibling= constructAST(ptnode, node);
                }
                 
                else{
                        node->child = constructAST(ptnode, node);
                }
                
                break;
        
        case 22:
                ptnode= parseChild(parseNode, NUM, 1);
                node = new_node(L_NUM, rule_No, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 23:
                ptnode= parseChild(parseNode, ID, 1);
                node = new_node(L_ID, rule_No, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;
        
        case 24:
                ptnode= parseChild(parseNode, PLUS, 1);
                node = new_node(L_PLUS, rule_No, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 25:
                ptnode= parseChild(parseNode, MINUS, 1);
                node = new_node(L_MINUS, rule_No, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;
        
        case 26:
                node = NULL;
                // node= new_node(L_PLUS, rule_No, NULL, parent, NULL, NULL, NULL, parseNode->TI); //CHECK
                break;

        case 27:
                ptnode= parseChild(parseNode, INTEGER, 1);
                node = new_node(L_INTEGER, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 28:
                ptnode= parseChild(parseNode, REAL, 1);
                node = new_node(L_REAL, rule_No, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 29:
                ptnode= parseChild(parseNode, BOOLEAN, 1);
                node = new_node(L_BOOLEAN, rule_No, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 30:
                ptnode = parseChild(parseNode, statements , 1);
                node = new_node(L_STATEMENTS, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                node = constructAST(ptnode, node);
                break;

        case 31:
                ptnode= parseChild(parseNode, statements, 1);
                node= constructAST(ptnode, parent);
                ptnode = parseChild(parseNode, statement, 1);
                temp = node->list_head;
                node->list_head = constructAST(ptnode, parent);
                node->list_head->sibling = temp;
                break;
            
        case 32:
                node= parent;
                node->list_head= NULL;
                break;
        
        case 33:
                ptnode = parseChild(parseNode, ioStmt, 1);
                node= constructAST(ptnode, parent);
                break;

        case 34:
                ptnode = parseChild(parseNode, simpleStmt, 1);
                node= constructAST(ptnode, parent);
                break;

        case 35:
                ptnode = parseChild(parseNode, declareStmt, 1);
                node= constructAST(ptnode, parent);
                break;

        case 36:
                ptnode = parseChild(parseNode, conditionalStmt, 1);
                node= constructAST(ptnode, parent);
                break;

        case 37:
                ptnode = parseChild(parseNode, iterativeStmt, 1);
                node= constructAST(ptnode, parent);
                break;
        
        case 38:
                ptnode= parseChild(parseNode, ID, 1);
                node= new_node(L_GET_VALUE, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                node->child = new_node(L_ID, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                break;
        
        case 39:
                node= new_node(L_PRINT, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode= parseChild(parseNode, var2, 1);
                synNode = constructAST(ptnode, node);
                node->child= synNode;
                break;

        case 40:
                node = new_node(L_ARRAY_ACCESS, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, whichID, 1);
                tempNode = constructAST(ptnode, node);
                if(tempNode!=NULL)
                {
                        ptnode = parseChild(parseNode, ID, 1);
                        synNode = new_node(L_ID, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                        node->child = synNode;
                        node->child->sibling = tempNode;
                }
                else
                {
                        ptnode = parseChild(parseNode, ID, 1);
                        node = new_node(L_ID, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                }
                break; 

        case 41:
                ptnode= parseChild(parseNode, NUM, 1);
                node = new_node(L_NUM, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 42:
                ptnode= parseChild(parseNode, RNUM, 1);
                node = new_node(L_RNUM, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 43:
                ptnode= parseChild(parseNode, boool, 1);
                node= constructAST(ptnode, parent);
                break;
        
        case 44:
                ptnode= parseChild(parseNode, TRUE, 1);
                node = new_node(L_TRUE, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 45:
                ptnode= parseChild(parseNode, FALSE, 1);
                node = new_node(L_FALSE, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 46:
                ptnode= parseChild(parseNode, index_arr, 1);
                node= constructAST(ptnode, parent);
                node->name = L_ARRAY_INDEX;
                break;

        case 47:
                node= NULL;
                break;
            
        case 48:
                ptnode= parseChild(parseNode, assignmentStmt, 1);
                node= constructAST(ptnode, parent);
                break;
        
        case 49:
                ptnode= parseChild(parseNode, moduleReuseStmt, 1);
                node= constructAST(ptnode, parent);
                break;

        case 50:
                ptnode=parseChild(parseNode, whichStmt, 1);
                node= constructAST(ptnode, parent);
                ptnode= parseChild(parseNode, ID, 1);
                synNode= new_node(L_ID, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                tempNode= node->child;
                node->child=synNode;
                node->child->sibling=tempNode;
                break;

        case 51:
                ptnode= parseChild(parseNode, lvalueIDStmt, 1);
                node= constructAST(ptnode, parent);
                break;

        case 52:
                ptnode= parseChild(parseNode, lvalueArrStmt, 1);
                node= constructAST(ptnode, parent);
                break;

        case 53:
                node= new_node(L_ASSIGN, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode= parseChild(parseNode, expression, 1);
                synNode= constructAST(ptnode, node);
                node->child= synNode;
                break;

        case 54:
                node= new_node(L_ARRAY_ASSIGN, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);

                tempNode = new_node(L_ARRAY_INDEX, parseNode->ruleNo, NULL, node, NULL, NULL, NULL, parseNode->TI);
                ptnode= parseChild(parseNode, arithmeticExprArr, 1);
                tempNode->child= constructAST(ptnode, tempNode);
                node->child = tempNode;
                ptnode = parseChild(parseNode, expression, 1);
                synNode= constructAST(ptnode, node);
                node->child->sibling= synNode;
                break;

        case 55:
                node= new_node(L_MODULEREUSESTATEMENT, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode= parseChild(parseNode, optional, 1);
                synNode= constructAST(ptnode, node);
                if(synNode!=NULL){
                        node->child = synNode;
                        ptnode = parseChild(parseNode, ID, 1);
                        synNode->sibling = new_node(L_ID, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                        ptnode = parseChild(parseNode, paraList, 1);
                        synNode->sibling->sibling = constructAST(ptnode, node);
                }
                else{
                        ptnode = parseChild(parseNode, ID, 1);
                        node->child = new_node(L_ID, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                        ptnode = parseChild(parseNode, paraList, 1);
                        node->child->sibling = constructAST(ptnode, node);
                }
                break;

        case 56:
                ptnode=  parseChild(parseNode, idList, 1);
                node= constructAST(ptnode, parent);
                break;

        case 57:
                node= NULL;
                break;

        case 58:
                node = new_node(L_PARALIST, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                tempNode = new_node(L_ELEMENT, parseNode->ruleNo, NULL, node, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, sign , 1);
                synNode = constructAST(ptnode, tempNode);
                ptnode= parseChild(parseNode, var, 1);

                if(synNode!=NULL){
                        tempNode->child= synNode;
                        synNode->sibling= constructAST(ptnode, tempNode);
                }
                else{
                        tempNode->child = constructAST(ptnode, tempNode);
                }

                ptnode= parseChild(parseNode, paraList2, 1);
                node = constructAST(ptnode, node);
                temp = node->list_head;
                node->list_head = tempNode;
                node->list_head->sibling = temp;
                break;

        case 59:
                ptnode= parseChild(parseNode, paraList2, 1);
                node = constructAST(ptnode, parent);
                temp = node->list_head;
                tempNode = new_node(L_ELEMENT, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, sign, 1);

                synNode = constructAST(ptnode, tempNode);
                // synNode = tempNode->child;
                ptnode = parseChild(parseNode, var, 1);
                if(synNode!=NULL){
                        synNode->sibling = constructAST(ptnode, tempNode);
                }
                else{
                        synNode = constructAST(ptnode, tempNode);
                }
                tempNode->child = synNode;
                                
                node->list_head = tempNode;
                node->list_head->sibling = temp;
                break;

        case 60:
                node = parent;
                node->list_head = NULL;
                break;
        case 61:
                ptnode= parseChild(parseNode, NUM, 1);
                node = new_node(L_NUM, rule_No, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;
        
        case 62:
                ptnode= parseChild(parseNode, RNUM, 1);
                node = new_node(L_RNUM, rule_No, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 63:
                ptnode = parseChild(parseNode, boool,1);
                node = constructAST(ptnode, parent);
                break;

        case 64: //check here - don't want array_access node in each case
                node = new_node(L_ARRAY_ACCESS, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, whichID2, 1);
                tempNode = constructAST(ptnode, node);
                if(tempNode!=NULL)
                {
                        ptnode = parseChild(parseNode, ID, 1);
                        synNode = new_node(L_ID, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                        node->child = synNode;
                        node->child->sibling = tempNode;
                }
                else
                {
                        ptnode = parseChild(parseNode, ID, 1);
                        node = new_node(L_ID, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                }
                
                break; 

        case 65:
                node = new_node(L_IDLIST, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, ID , 1);
                inhNode = new_node(L_ID, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);

                ptnode= parseChild(parseNode, idList2, 1);
                constructAST(ptnode, node);
                
                tempNode = node->list_head;
                node->list_head= inhNode;
                inhNode->sibling = tempNode;
                break;

        case 66:
                node = parent;
                ptnode= parseChild(parseNode, ID, 1);
                inhNode = new_node(L_ID, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);

                ptnode= parseChild(parseNode, idList2, 1);
                constructAST(ptnode, parent);
                
                tempNode = node->list_head;
                node->list_head= inhNode;
                inhNode->sibling = tempNode;
                break;

        case 67:
                node = parent;
                node->list_head = NULL;
                break;
        
        case 68:
                ptnode = parseChild(parseNode, arithmeticOrBoolean, 1);
                node = constructAST(ptnode, parent);
                break;

        case 69:
                ptnode = parseChild(parseNode, unaryExpr, 1);
                node = constructAST(ptnode, parent);
                break;

        case 70:
                ptnode = parseChild(parseNode, logicalExpr, 1);
                synNode = constructAST(ptnode, parent);
                ptnode = parseChild(parseNode, term1, 1);
                

                if(synNode!=NULL){
                    inhNode = constructAST(ptnode, synNode);
                    temp = synNode->child;
                    synNode->child = inhNode;
                    inhNode->sibling = temp;
                    node = synNode;
                }
                else{
                    inhNode = constructAST(ptnode, parent);
                    node = inhNode;
                }
                break;

        case 71:
                ptnode = parseChild(parseNode, relationalExpr, 1);
                synNode = constructAST(ptnode, parent);
                ptnode = parseChild(parseNode, arithmeticExpr, 1);

                if(synNode!=NULL){
                    inhNode = constructAST(ptnode, synNode);
                    nodeAST temp = synNode->child;
                    synNode->child = inhNode;
                    inhNode->sibling = temp;
                    node = synNode;
                }
                else{
                    inhNode = constructAST(ptnode, parent);
                    node = inhNode;
                }
                break;

        case 72:
                ptnode = parseChild(parseNode, boool, 1);
                node = constructAST(ptnode, parent);
                break;

        case 73:
                ptnode = parseChild(parseNode, arithmeticExpr2, 1);
                synNode = constructAST(ptnode, parent);
                ptnode = parseChild(parseNode, term2, 1);

                if(synNode!=NULL){
                    inhNode = constructAST(ptnode, synNode);
                    temp = synNode->child;
                    synNode->child = inhNode;
                    inhNode->sibling = temp;
                    node = synNode;
                }
                else{  
                    inhNode = constructAST(ptnode, parent);
                    node = inhNode;
                }
                break;

        case 74:
                ptnode = parseChild(parseNode, op1, 1);
                node = constructAST(ptnode, parent);                

                ptnode = parseChild(parseNode, arithmeticExpr2, 1);
                synNode = constructAST(ptnode, node);

                ptnode = parseChild(parseNode, term2, 1);

                if(synNode!=NULL){
                    inhNode = constructAST(ptnode, synNode);
                    temp = synNode->child;
                    synNode->child = inhNode;
                    inhNode->sibling = temp;
                    node->child = synNode;
                }
                else{
                    
                    inhNode = constructAST(ptnode, node);
                    node->child = inhNode;
                }
                break;

        case 75:
                node = NULL;
                break;

        case 76:
                ptnode = parseChild(parseNode, term3, 1);
                synNode = constructAST(ptnode, parent);
                ptnode = parseChild(parseNode, factor, 1);

                if(synNode!=NULL){
                    inhNode = constructAST(ptnode, synNode);
                    temp = synNode->child;
                    synNode->child = inhNode;
                    inhNode->sibling = temp;
                    node = synNode;

                }
                else{
                    inhNode = constructAST(ptnode, parent);
                    node = inhNode;
                }
                break;

        case 77:
                ptnode = parseChild(parseNode, op2, 1);
                node = constructAST(ptnode, parent);

                ptnode = parseChild(parseNode, term3, 1);
                synNode = constructAST(ptnode, node);

                ptnode = parseChild(parseNode, factor, 1);

                if(synNode!=NULL){
                    inhNode = constructAST(ptnode, synNode);
                    temp = synNode->child;
                    synNode->child = inhNode;
                    inhNode->sibling = temp;
                    node->child = synNode;
                }
                else{
                    inhNode = constructAST(ptnode, node);
                    node->child = inhNode;
                }
                break;

        case 78:
                node = NULL;
                break;

        case 79:
                ptnode = parseChild(parseNode, arithmeticOrBoolean, 1);
                node = constructAST(ptnode, parent);
                break;

        case 80:
                ptnode = parseChild(parseNode, NUM, 1);
                node = new_node(L_NUM, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 81:
                ptnode = parseChild(parseNode, RNUM, 1);
                node = new_node(L_RNUM, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;
        
        case 82:
                node = new_node(L_ARRAY_ACCESS, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, whichID2, 1);
                tempNode = constructAST(ptnode,node);
                if(tempNode!=NULL)
                {
                        ptnode = parseChild(parseNode, ID, 1);
                        synNode = new_node(L_ID, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                        node->child = synNode;
                        node->child->sibling = tempNode;
                }
                else
                {
                        ptnode = parseChild(parseNode, ID, 1);
                        node = new_node(L_ID, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                }
                break; 
        
        case 83:
                ptnode= parseChild(parseNode, arrExpr, 1);
                node= constructAST(ptnode, parent);
                break;
        
        case 84:
                node= NULL;
                break;

        case 85:
                ptnode = parseChild(parseNode, relationalOP, 1);
                node = constructAST(ptnode, parent);

                ptnode = parseChild(parseNode, arithmeticExpr, 1);
                synNode = constructAST(ptnode, node);

                node->child = synNode;
                break;

        case 86:
                node = NULL;
                break;

        case 87:
                ptnode = parseChild(parseNode, LT, 1);
                node = new_node(L_LT, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 88:
                ptnode = parseChild(parseNode, LE, 1);
                node = new_node(L_LE, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 89:
                ptnode = parseChild(parseNode, GT, 1);
                node = new_node(L_GT, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 90:
                ptnode = parseChild(parseNode, GE, 1);
                node = new_node(L_GE, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 91:
                ptnode = parseChild(parseNode, EQ, 1);
                node = new_node(L_EQ, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 92:
                ptnode = parseChild(parseNode, NE, 1);
                node = new_node(L_NE, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 93:
                ptnode = parseChild(parseNode, logicalOP, 1);
                node = constructAST(ptnode, parent);                

                ptnode = parseChild(parseNode, logicalExpr, 1);
                synNode = constructAST(ptnode, node);

                ptnode = parseChild(parseNode, term1, 1);

                if(synNode!=NULL){
                    inhNode = constructAST(ptnode, synNode);
                    temp = synNode->child;
                    synNode->child = inhNode;
                    inhNode->sibling = temp;
                    node->child = synNode;
                }
                else{
                    
                    inhNode = constructAST(ptnode, node);
                    node->child = inhNode;
                }
                break;

        case 94:
                node = NULL;
                break;

        case 95:
                ptnode = parseChild(parseNode, AND, 1);
                node = new_node(L_AND, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 96:
                ptnode = parseChild(parseNode, OR, 1);
                node = new_node(L_OR, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;
                

        case 97:
                ptnode = parseChild(parseNode, PLUS, 1);
                node = new_node(L_PLUS, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 98:
                ptnode = parseChild(parseNode, MINUS, 1);
                node = new_node(L_MINUS, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 99:
                ptnode = parseChild(parseNode, MUL, 1);
                node = new_node(L_MUL, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 100:
                ptnode = parseChild(parseNode, DIV, 1);
                node = new_node(L_DIV, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 101: //doubt- create new node in line 2 or not? yes, to differenctiate between if its of 
                  //unary expression or an operator of arithmetic expression
                ptnode = parseChild(parseNode, unaryOP, 1);
                node = new_node(L_UNARYEXPR, parent->ruleNo, NULL, parent, NULL, NULL, NULL, NULL);
                inhNode = constructAST(ptnode, node);
                node->child = inhNode;
                ptnode = parseChild(parseNode, new_NT, 1);
                inhNode->sibling = constructAST(ptnode, node);
                break;

        case 102:
                ptnode = parseChild(parseNode, arithmeticExpr, 1);
                node = constructAST(ptnode, parent);
                break;

        case 103:
                ptnode = parseChild(parseNode, idNumRnum, 1);
                node = constructAST(ptnode, parent);
                break;

        case 104:
                ptnode = parseChild(parseNode, PLUS, 1);
                node = new_node(L_PLUS, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 105:
                ptnode = parseChild(parseNode, MINUS, 1);
                node = new_node(L_MINUS, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 106:
                ptnode = parseChild(parseNode, ID, 1);
                node = new_node(L_ID, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 107:
                ptnode = parseChild(parseNode, NUM, 1);
                node = new_node(L_NUM, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 108:
                ptnode = parseChild(parseNode, RNUM, 1);
                node = new_node(L_RNUM, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 109: //doubt same as case 101
                
                node = new_node(L_ARRAY_INDEX, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, op1, 1);
                inhNode = constructAST(ptnode, node);
                ptnode = parseChild(parseNode, term6, 1);
                synNode = constructAST(ptnode, node);

                node->child = inhNode;
                inhNode->sibling = synNode;
                break;


        case 110: //should we also create a node here for array_index
                node = new_node(L_ARRAY_INDEX, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, arithmeticExprArr, 1);
                node->child = constructAST(ptnode, parent);
                break;

        case 111:
                ptnode = parseChild(parseNode, ind, 1);
                node = constructAST(ptnode, parent);
                break;

        case 112:
                ptnode = parseChild(parseNode, arithmeticExprArr, 1);
                node = constructAST(ptnode, parent);
                break;

        case 113:
                ptnode = parseChild(parseNode, arithmeticExprArr2, 1);
                synNode = constructAST(ptnode, parent);

                ptnode = parseChild(parseNode, term4, 1);

                if(synNode!=NULL){
                    inhNode = constructAST(ptnode, synNode);
                    temp = synNode->child;
                    synNode->child = inhNode;
                    inhNode->sibling = temp;
                    node = synNode;
                }

                else{
                    inhNode = constructAST(ptnode, parent);
                    node = inhNode;
                }
                break;

        case 114:
                ptnode = parseChild(parseNode, op1, 1);
                node = constructAST(ptnode, parent);                

                ptnode = parseChild(parseNode, arithmeticExprArr2, 1);
                synNode = constructAST(ptnode, node);

                ptnode = parseChild(parseNode, term4, 1);

                if(synNode!=NULL){
                    inhNode = constructAST(ptnode, synNode);
                    temp = synNode->child;
                    synNode->child = inhNode;
                    inhNode->sibling = temp;
                    node->child = synNode;
                }
                else{
                    
                    inhNode = constructAST(ptnode, node);
                    node->child = inhNode;
                }
                break;

        case 115:
                node = NULL;
                break;

        case 116:
                ptnode = parseChild(parseNode, term5, 1);
                synNode = constructAST(ptnode, parent);
                ptnode = parseChild(parseNode, factor2, 1);

                if(synNode!=NULL){
                    inhNode = constructAST(ptnode, synNode);
                    temp = synNode->child;
                    synNode->child = inhNode;
                    inhNode->sibling = temp;
                    node = synNode;
                }
                else{  
                    inhNode = constructAST(ptnode, parent);
                    node = inhNode;
                }
                break;

        case 117:
                ptnode = parseChild(parseNode, op2, 1);
                node = constructAST(ptnode, parent);                

                ptnode = parseChild(parseNode, term5, 1);
                synNode = constructAST(ptnode, node);

                ptnode = parseChild(parseNode, factor2, 1);

                if(synNode!=NULL){
                    inhNode = constructAST(ptnode, synNode);
                    temp = synNode->child;
                    synNode->child = inhNode;
                    inhNode->sibling = temp;
                    node->child = synNode;
                }
                else{
                    
                    inhNode = constructAST(ptnode, node);
                    node->child = inhNode;
                }
                break;

        case 118:
                node = NULL;
                break;

        case 119:
                ptnode = parseChild(parseNode, arithmeticExprArr, 1);
                node = constructAST(ptnode, parent);
                break;

        case 120:
                ptnode = parseChild(parseNode, boool, 1);
                node = constructAST(ptnode, parent);
                break;

        case 121:
                ptnode = parseChild(parseNode, ID, 1);
                node = new_node(L_ID, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 122:
                ptnode = parseChild(parseNode, NUM, 1);
                node = new_node(L_NUM, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 123:
                node  = new_node(L_DECLARE, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, parseNode->TI);
                ptnode = parseChild(parseNode, idList, 1);
                synNode = constructAST(ptnode, node);
                node->child = synNode;
                ptnode = parseChild(parseNode, dataType, 1);
                synNode->sibling = constructAST(ptnode, node);
                break;


        case 124:
                node = new_node(L_SWITCH, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, NULL);
                ptnode = parseChild(parseNode, ID, 1);

                inhNode = new_node(L_ID, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                node->child = inhNode;
                
                ptnode = parseChild(parseNode, caseStmts, 1);
                synNode = constructAST(ptnode, node);
                inhNode->sibling = synNode;

                ptnode = parseChild(parseNode, default1, 1);
                synNode->sibling = constructAST(ptnode, node);
                break;

        case 125:
                node = new_node(L_CASESTMTS, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, NULL);
                
                ptnode = parseChild(parseNode, value, 1);
                inhNode = new_node(L_CASE, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, NULL);
                              
                inhNode->child = constructAST(ptnode, inhNode);

                ptnode = parseChild(parseNode, statements, 1);
                synNode = new_node(L_STATEMENTS, ptnode->ruleNo, NULL, inhNode, NULL, NULL, NULL, ptnode->TI);
                inhNode->child->sibling = synNode;
                synNode = constructAST(ptnode, synNode);
                // inhNode->child->sibling = constructAST(ptnode, inhNode);

                ptnode = parseChild(parseNode, caseStmts2, 1);
                constructAST(ptnode, node);

                temp = node->list_head;
                node->list_head = inhNode;
                inhNode->sibling = temp;
                break;

        case 126:
                ptnode = parseChild(parseNode, value, 1);
                inhNode = new_node(L_CASE, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, NULL);
                              
                inhNode->child = constructAST(ptnode, inhNode);

                // ptnode = parseChild(parseNode, statements, 1);
                // inhNode->child->sibling = constructAST(ptnode, inhNode);
                ptnode = parseChild(parseNode, statements, 1);
                synNode = new_node(L_STATEMENTS, ptnode->ruleNo, NULL, inhNode, NULL, NULL, NULL, ptnode->TI);
                inhNode->child->sibling = synNode;
                synNode = constructAST(ptnode, synNode);

                ptnode = parseChild(parseNode, caseStmts2, 1);
                constructAST(ptnode, parent);

                temp = parent->list_head;
                parent->list_head = inhNode;
                inhNode->sibling = temp;
                node = parent;
                break;

        case 127:
                parent->list_head = NULL;
                node = parent;
                break;

        case 128:
                ptnode = parseChild(parseNode, NUM, 1);
                node = new_node(L_NUM, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;
                
        case 129:
                ptnode = parseChild(parseNode, TRUE, 1);
                node = new_node(L_TRUE, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 130:
                ptnode = parseChild(parseNode, FALSE, 1);
                node = new_node(L_FALSE, ptnode->ruleNo, NULL, parent, NULL, NULL, NULL, ptnode->TI);
                break;

        case 131:
        {
                // ptnode = parseChild(parseNode, statements, 1);
                // node = constructAST(ptnode, parent);
                tokenInfo* token = parseChild(parseNode, DEFAULT, 1)->TI;
                node= new_node(L_DEFAULT, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, token);
                ptnode = parseChild(parseNode, statements, 1);
                synNode = new_node(L_STATEMENTS, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                // inhNode->child->sibling = synNode;
                node->child= synNode;
                synNode= constructAST(ptnode, synNode);
                break;
        }

        case 132:
                node = NULL;
                break;
                
        case 133:
                node = new_node(L_FOR, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, NULL);
                ptnode = parseChild(parseNode, ID, 1);
                inhNode = new_node(L_ID, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                ptnode = parseChild(parseNode, range_loop, 1);
                inhNode->sibling = constructAST(ptnode, node);

                ptnode = parseChild(parseNode, statements, 1);
                synNode = new_node(L_STATEMENTS, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                constructAST(ptnode, synNode);
                inhNode->sibling->sibling = synNode;
                node->child = inhNode;
                break;

        case 134:
                node = new_node(L_WHILE, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, NULL);
                ptnode = parseChild(parseNode, arithmeticOrBoolean, 1);
                inhNode = constructAST(ptnode, node);
                ptnode = parseChild(parseNode, statements, 1);
                
                synNode = new_node(L_STATEMENTS, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                constructAST(ptnode, synNode);
                inhNode->sibling = synNode;
                node->child = inhNode;
                break;

        case 135:
                node = new_node(L_RANGE, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, NULL);
                ptnode = parseChild(parseNode, index_loop, 1);
                node->child = constructAST(ptnode, node);
                ptnode = parseChild(parseNode, index_loop, 2);
                node->child->sibling = constructAST(ptnode, node);
                break;
                
        case 136:
                node = new_node(L_ELEMENT, parseNode->ruleNo, NULL, parent, NULL, NULL, NULL, NULL);
                ptnode = parseChild(parseNode, sign, 1);
                inhNode = constructAST(ptnode, node);
                ptnode = parseChild(parseNode, NUM, 1);
                if(inhNode!=NULL){
                        inhNode->sibling = new_node(L_NUM, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                        node->child = inhNode;
                }
                else {
                        node->child = new_node(L_NUM, ptnode->ruleNo, NULL, node, NULL, NULL, NULL, ptnode->TI);
                        
                }
                break;
    }

    return node;
}

void print_ast(nodeAST root){
	// printf("entering print_ast \n");
	if (root == NULL) 
                return;
	printf("%s : ", Label_str[root->name]);
        // if(root->name == ){
        //         printf("lexeme: %s", root->TI->tok)
        // }
	if (root->parent)
                printf("%s \n", Label_str[root->parent->name]);
	else
                printf("NULL\n");
        
        int flag2 = 0;
        if(root->list_head){
                flag2 = 1;
                print_ast(root->list_head);
        }
	// if(root->g != NULL) printf("-%s ", names_ntandt[root->g->name]);
	int flag = 0;
        // if(flag2==0){
                if (root->child)
                {
                        printf("---->");
                        print_ast(root->child);
                        flag = 1;
                }
        // }
	
                
	if (root->sibling)
        {
                if(flag==0)
                {
                        printf("---->");
                }
                print_ast(root->sibling);
        }
	//printf("extiting print_ast\n");
	return;

}

nodeAST ast_main(treeNode* parseTreeRoot){
	printf("enter ast_main\n");
	nodeAST root = constructAST(parseTreeRoot, NULL);
	printf("AST CREATION DONE!!\n");
	return root;
}

// void main(int argc, char *argv[])
// {
//         fillKeywordTable();
//         // bufferSize = atoi(argv[3]);
//         // bufferSize = argv[3];
//         bufferSize=1000;
//         // file_ptr = fopen(argv[1], "r");
//         file_ptr = fopen("t5.txt", "r");
//         if (file_ptr == NULL)
//         {
//         printf("File Opening Error\n");
//         }

//         // fileParseTree = fopen(argv[2], "w");
//         fileParseTree = fopen("traversalFile.txt", "w");
//         treeNode* parseRoot= stackImp();
//         nodeAST astRoot= ast_main(parseRoot);
//         printf("\n PRINTING AST \n");
//         print_ast(astRoot);
//         fclose(file_ptr);
//         fclose(fileParseTree);
// }