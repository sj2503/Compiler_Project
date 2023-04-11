#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ast.c"

#define SIZE_BOOLEAN 1
#define SIZE_INTEGER 2
#define SIZE_REAL 4
#define SIZE_POINTER 8
#define ERROR_OCCURED -1


int hashFunc(const char *key) // Function for assigning the Hash Value
{
    int hash = 0;
    const char *t;
    for (t = key; *t != '\0'; t++)
    {
        hash = 31 * hash + *t; // Hash Function
        hash = hash % TABLE_SIZE;
    }
    return hash % TABLE_SIZE;
}
// int compareStrings(char str1[], char str2[]) {
//     int i = 0;
//     while (str1[i] != '\0' && str2[i] != '\0') {
//         if (str1[i] != str2[i]) {
//             return 1;
//         }
//         i++;
//     }
//     if (str1[i] == '\0' && str2[i] == '\0') {
//         return 0;
//     }
//     return 1;
// }
void* search_table(hashTable t, char name[LEXEME_LEN])
{
    int hashing = hashFunc(name);
    if(t == NULL) 
        return NULL;
    hashElem next_elem = t[hashing];
    // char *str= (char*) &name[0];
    while(next_elem){
        // char* temp= (char*) &next_elem->name[0];
        if(strcmp(name, next_elem->name)==0){
            return next_elem->entry;
        }
        next_elem = next_elem->next;
    }
    return NULL;
}

void insert_table(hashTable t, char name[LEXEME_LEN], data_ptr* ptr)
{
    int hashing = hashFunc(name);
    hashElem next_elem = t[hashing];
    hashElem elem = (hashElem) malloc(sizeof(hashEntry));
    strncpy(elem->name, name, LEXEME_LEN);
    elem->entry = ptr;
    elem->next = NULL;
    if(next_elem == NULL){
        t[hashing] = elem;
        return;
    }
    while(next_elem->next){
       next_elem = next_elem->next;
    }
    next_elem->next = elem;
    return;
}

data_ptr* generateType(nodeAST node){
    data_ptr* type = (data_ptr*)malloc(sizeof(data_ptr));
    type->var= (idType*) malloc(sizeof(idType));
    Label data_type;
    bool primitive_type = true;
    bool dyn_arr = false;
    int count_elements = 1;

    if(node->name ==L_ARRAY){
        primitive_type = false;
        type->name = L_ARRAY;
        type->var->dataType = node->child->sibling->name;
        data_type = type->var->dataType;
        nodeAST node_low_range = node->child->child;
        nodeAST node_high_range = node->child->child->sibling;

        //setting low range
        if(node_low_range->child->sibling==NULL){
            type->var->start_ind_neg = false;
            if(node_low_range->child->name == L_NUM){
                type->var->start_ind.num = node_low_range->child->TI->val.valIfInt;
                type->var->isDynamic.starInd = false;
            }
            else{
                strcpy(type->var->start_ind.lex, node_low_range->child->TI->val.lexeme);
                type->var->isDynamic.starInd = true;
            }
        }
        else{
            if(node_low_range->child->name == L_PLUS){
                type->var->start_ind_neg = false;
            }
            else{
                type->var->start_ind_neg = true;
            }
            if(node_low_range->child->sibling->name == L_NUM){
                type->var->start_ind.num = node_low_range->child->sibling->TI->val.valIfInt;
                type->var->isDynamic.starInd = false;
            }
            else{
                strcpy(type->var->start_ind.lex, node_low_range->child->sibling->TI->val.lexeme);
                type->var->isDynamic.starInd = true;
            }
        }

        //setting high range
        if(node_high_range->child->sibling==NULL){
            type->var->end_ind_neg = false;
            if(node_high_range->child->name == L_NUM){
                type->var->end_ind.num = node_high_range->child->TI->val.valIfInt;
                type->var->isDynamic.endInd = false;
            }
            else{
                strcpy(type->var->end_ind.lex, node_high_range->child->TI->val.lexeme);
                type->var->isDynamic.endInd = true;
            }
        }
        else{
            if(node_high_range->child->name == L_PLUS){
                type->var->end_ind_neg = false;
            }
            else{
                type->var->end_ind_neg = true;
            }
            if(node_high_range->child->sibling->name == L_NUM){
                type->var->end_ind.num = node_high_range->child->sibling->TI->val.valIfInt;
                type->var->isDynamic.endInd = false;
            }
            else{
                strcpy(type->var->end_ind.lex, node_high_range->child->sibling->TI->val.lexeme);
                type->var->isDynamic.endInd = true;
            }
        }
        dyn_arr = type->var->isDynamic.endInd || type->var->isDynamic.starInd;
        if(!dyn_arr){
            int left, right;
            if(type->var->start_ind_neg){
                left = (-1) * type->var->start_ind.num;
            }
            else left = type->var->start_ind.num;

            if(type->var->end_ind_neg){
                right = (-1) * type->var->end_ind.num;
            }
            else{
                right = type->var->end_ind.num; 
            }
            count_elements = right - left + 1;
        }
        
        
    }
    else{
        type->name = node->name;
        data_type  = type->name;
        type->var->dataType = data_type;
    }

    if(primitive_type || !(dyn_arr)){
        if(data_type == L_INTEGER){
            type->width = SIZE_INTEGER * count_elements;
        }
        else if(data_type == L_BOOLEAN){
            type->width = SIZE_BOOLEAN * count_elements;
        }
        else if(data_type == L_REAL){
            type->width = SIZE_REAL * count_elements;
        }
        if(!dyn_arr && !primitive_type){
            type->offset +=1;
        }
    }
    else{
        type->width = 1;
    }

    type->isAssigned = false;
    return type;
}

data_ptr* funcEntry(symbolTable* SymTable, char idname[LEXEME_LEN], nodeAST inList, nodeAST outList, int lineNo)
{
    data_ptr* exist= (data_ptr*) search_table(SymTable->localTable, idname);
    if(exist != NULL)
    {
        if(exist->func->isDefined)
        {
            printf("Semantic Error: Redefinition of %s function on line number %d", idname, lineNo);
            return NULL;
        }
        else
        {
            exist->func->isDefined = true;
        }
    }
    else
    {
        exist = (data_ptr*) malloc(sizeof(data_ptr));
        exist->func = (funcType*) malloc(sizeof(funcType));
        exist->line_no = lineNo;
    }

        exist->func->isDefined = true;
        //check declaration_valid attribute here
        exist->func->isDeclared = true;
        strcpy(exist->tokName, idname);

        if(strcmp(idname, "Driver")==0){
            exist->func->ipList = NULL;
            exist->func->opList = NULL;
        }
        else{
            if(inList->list_head == NULL){
                exist->func->ipList = NULL;
            }
            else{
                nodeAST temp = inList->list_head;
                exist->func->ipList = (list*) malloc(sizeof(list));
                list* ll = exist->func->ipList;
                strcpy(ll->lexeme, temp->child->TI->val.lexeme);
                ll->type = generateType(temp->child->sibling);
                temp = temp->sibling;
                while (temp!=NULL)
                {
                    list* temp2 = (list*) malloc(sizeof(list));
                    ll->next = temp2;
                    ll = ll->next;
                    strcpy(ll->lexeme, temp->child->TI->val.lexeme);
                    ll->type = generateType(temp->child->sibling);
                    temp = temp->sibling;
                }            
            }
        }

        if(outList!=NULL && outList->list_head!=NULL){
            nodeAST temp = outList->list_head;
            exist->func->opList = (list*) malloc(sizeof(list));
            list* ll = exist->func->opList;
            strcpy(ll->lexeme, temp->child->TI->val.lexeme);
            ll->type = generateType(temp->child->sibling);
            temp = temp->sibling;
            
            while (temp!=NULL)
            {
                list* temp2 = (list*) malloc(sizeof(list));
                ll->next = temp2;
                ll = ll->next;
                strcpy(ll->lexeme, temp->child->TI->val.lexeme);
                ll->type = generateType(temp->child->sibling);
                temp = temp->sibling;
            }
        }
        else{
            exist->func->opList = NULL;
        }
        exist->name = L_MODULE;
        exist->offset = 0;
        exist->isAssigned = false;
        exist->var = NULL;
        exist->width = 0;
        //modify for any extra nodes in data_ptr here

        insert_table(SymTable->localTable, idname, exist);
    

    return exist;
}

bool scope_change(Label l)
{
    switch(l){
        case L_PROGRAM:
        // case L_DRIVERMODULE:
        // case L_MODULE:
        case L_CASE:
        case L_FOR:
        case L_WHILE:
        case L_DEFAULT:
                return true;
                break;
        default: 
                return false;
                break;

    }
}

bool isDeclared(nodeAST node)
{
    if(node==NULL)
        return false;
    switch(node->name){
        case L_MODULEDECLARATIONS:
                return true;
                break;
        case L_IDLIST:
                if(node->parent &&  node->parent->name == L_DECLARE) 
                    return true;
                return false;
                break;
        case L_ELEMENT:
            {   nodeAST temp= node;
                if(temp->parent &&  temp->parent->name == L_RANGE){
                    temp = temp->parent;
                    if(temp->parent &&  temp->parent->name == L_ARRAY){
                        temp = temp->parent;
                        if(temp->parent &&  temp->parent->name == L_VARIABLE){
                            temp = temp->parent;
                            if(temp->parent &&  temp->parent->name == L_INPUT_PLIST){
                                return true;
                            }
                        }
                    }
                }
                return false;
            }
                break;
        default:       
                return false;
    }
}

bool checkOPList(char varName[LEXEME_LEN], char fname[LEXEME_LEN])
{
    symbolTable* temp= mainSym;
    if(temp)
    {    data_ptr* matchPtr= (data_ptr*) search_table(temp->localTable, fname);
        // matchPtr->func= (funcType*) malloc(sizeof(funcType));
        if(matchPtr)
        {
            list* outList= matchPtr->func->opList;
            while(outList){
                if(strcmp(outList->lexeme, varName)==0)
                    return true;
                outList= outList->next;
            }
        }
    }
    return false;
}

void IDEntry(symbolTable* table, nodeAST node)
{
    if(node==NULL || node->parent==NULL)
        return;
    data_ptr* ptr= (data_ptr *)malloc(sizeof(data_ptr));
    ptr->var= (idType*) malloc(sizeof(idType));
    ptr->func= (funcType*) malloc(sizeof(funcType));
    switch(node->parent->name)
    {
        case L_MODULEDECLARATIONS:
            {

                ptr->name= L_MODULE;
                strcpy(ptr->tokName, node->TI->val.lexeme);
                strcpy(ptr->var->parentFunc, node->funcName);
                ptr->func->isDeclared= true;
                ptr->func->isDefined= false;
                ptr->func->ipList= NULL;
                ptr->func->opList= NULL;
                ptr->offset=0;
                ptr->width=0;
                ptr->line_no= node->TI->lineNo;
                ptr->isAssigned= false;   
            }
        break;
        case L_IDLIST:
            {
                nodeAST temp= node->parent;   
                if(temp->parent &&  temp->parent->name == L_DECLARE)
                {
                    
                    ptr= generateType(temp->sibling);
                    strcpy(ptr->tokName, node->TI->val.lexeme);
                    strcpy(ptr->var->parentFunc, node->funcName);
                    ptr->line_no= node->TI->lineNo;
                    ptr->isAssigned= false; 
                    // Add offset data
                }
                else
                {
                    return;
                }
            }
            break;
        case L_ELEMENT:
            {
                nodeAST temp= node->parent;
                if(temp->parent &&  temp->parent->name == L_RANGE){
                    temp = temp->parent;
                    if(temp->parent &&  temp->parent->name == L_ARRAY){
                        temp = temp->parent;
                        if(temp->parent &&  temp->parent->name == L_VARIABLE){
                            temp = temp->parent;
                            if(temp->parent &&  temp->parent->name == L_INPUT_PLIST)
                            {
                                ptr->name= L_INTEGER;
                                ptr->width= SIZE_INTEGER;
                                strcpy(ptr->tokName, node->TI->val.lexeme);
                                strcpy(ptr->var->parentFunc, node->funcName);
                                ptr->line_no= node->TI->lineNo;
                                ptr->isAssigned= false; 
                            }
                        }
                    }
                }
            }
            break;
        default:
            return;
            break;      
    }
    insert_table(table->localTable, node->TI->val.lexeme, ptr);
}

data_ptr* check_para(list* iplist, list* oplist, char lexeme[LEXEME_LEN], bool *isOuPara){
    bool found = false;
    while(iplist!=NULL){
        if(strcmp(lexeme, iplist->lexeme)==0){
            return iplist->type;
        }
        iplist = iplist->next;
    }
    while(oplist!=NULL){
        if(strcmp(lexeme, oplist->lexeme)==0){
            if(isOuPara!=NULL)
                {
                    *isOuPara = true;
                    oplist->type->isAssigned = true;
                }
            return oplist->type;
        }
        oplist = oplist->next;
    }
    return NULL;
}

data_ptr* return_from_parent(nodeAST node, symbolTable* table, bool* isOutPara){
    if(table == NULL){
        return NULL;
    }
    data_ptr* ptr = (data_ptr*)search_table(table->localTable, node->TI->val.lexeme);
    if(ptr == NULL){
        ptr = return_from_parent(node, table->parent, isOutPara);
    }
    else
    {
        node->ptrTable= table;
    }
    return ptr;
}
data_ptr* return_type(nodeAST node, symbolTable* table, bool* isOutPara){
     if(table == NULL){
        return NULL;
    }
    data_ptr* ptr = (data_ptr*) return_from_parent(node,table,isOutPara);
    if(ptr == NULL){
        data_ptr* func_type = search_table(mainSym->localTable, node->funcName);
        ptr = check_para(func_type->func->ipList, func_type->func->opList, node->TI->val.lexeme, isOutPara);
        
    }
    
    return ptr;
}

int expressionType(nodeAST node){//check for unary expression as well
    if(node==NULL)
        return -1;
    Label name  = node->name;
    if(name == L_UNARYEXPR){
        int type =  expressionType(node->child->sibling);
        i(type==L_BOOLEAN){
            printf("SEMANTIC ERROR: ")
        }
    }
    else if(name==L_PLUS || name ==L_MINUS || name==L_MUL){
        int left = expressionType(node->child);
        int right = expressionType(node->child->sibling);
        if(left == L_INTEGER && right ==L_INTEGER){
            return L_INTEGER;
        }
        else if(left==L_REAL && right == L_REAL){
            return L_REAL;
        }
        else{
            if(!(left==ERROR_OCCURED || right==ERROR_OCCURED)){
                printf("SEMANTIC ERROR: Type mismatch in line %d\n", node->TI->lineNo);
            }
            return ERROR_OCCURED;//SHOULD SAVE SEMANTIC ERROR HERE
        }
    }

    else if(name==L_DIV){
        int left = expressionType(node->child);
        int right = expressionType(node->child->sibling);
        if((left == L_INTEGER && right ==L_INTEGER) || (left==L_REAL && right == L_REAL) || (left==L_REAL && right == L_INTEGER) || (left==L_INTEGER && right == L_REAL)){
            return L_REAL;
        }
        else{
            printf("SEMANTIC ERROR: Type mismatch in line %d\n", node->TI->lineNo);
            return ERROR_OCCURED;//SHOULD SAVE SEMANTIC ERROR HERE
        }
    }
    
    else if(name == L_LT || name == L_LE|| name == L_GT || name == L_GE || name == L_NE || name==L_EQ){
        int left = expressionType(node->child);
        int right = expressionType(node->child->sibling);
        if((left==L_INTEGER) && (right==L_INTEGER)){
            return L_BOOLEAN;
        }
        else if(left == L_REAL && right==L_REAL){
            return L_BOOLEAN;
        }
        else return ERROR_OCCURED;//SHOULD SAVE SEMANTIC ERROR HERE
    }
    else if(name == L_AND || name==L_OR){
        int left = expressionType(node->child);
        int right = expressionType(node->child->sibling);
        if((left==L_BOOLEAN) && (right==L_BOOLEAN)){
            return L_BOOLEAN;
        }
        else return ERROR_OCCURED;//SHOULD SAVE SEMANTIC ERROR HERE
    }
    else if(name == L_ID){
        bool isOut = false;
        // data_ptr* temp = return_type(node, node->ptrTable, &isOut);
        data_ptr* temp = return_type(node, currSym, &isOut);
        if(temp==NULL){
            printf("SEMANTIC ERROR: %s declaration not found in line %d\n", node->TI->val.lexeme, node->TI->lineNo);
            return ERROR_OCCURED;
        }
        if(temp->var->dataType == L_ARRAY){
            printf("SEMANTIC ERROR: %s type mismatch in line %d\n", node->TI->val.lexeme, node->TI->lineNo);
            return ERROR_OCCURED; //SHOULD SAVE SEMANTIC ERROR HERE
        }
        return temp->name;
    }
    else if(name ==L_ARRAY_INDEX){
        if(node->child->sibling==NULL){
            return expressionType(node->child);
        }
        return expressionType(node->child->sibling);
    }
    else if(name == L_ARRAY_ACCESS || name == L_ARRAY_ASSIGN){
        // data_ptr* temp = return_type(node->child, node->ptrTable, NULL);
        data_ptr* temp = return_type(node->child, currSym, NULL);
        if(temp==NULL){
            printf("SEMANTIC ERROR: Array %s declaration not found in line %d\n", node->child->TI->val.lexeme, node->child->TI->lineNo);
            return ERROR_OCCURED;
        }
        else if(temp->name ==L_ARRAY){
            nodeAST temp2 = node->child->sibling;
            if(expressionType(temp2)==L_INTEGER){ //if access expression is of L_NUM continue check
            
                bool isDyn = temp->var->isDynamic.endInd || temp->var->isDynamic.starInd;
                if(!isDyn){
                    if((temp2->child->sibling==NULL && temp2->child->name==L_INTEGER)||(temp2->child->sibling!=NULL && temp2->child->sibling->name==L_INTEGER)){
                        int index;
                        if(temp2->child->sibling==NULL && temp2->child->name==L_INTEGER){
                            index = temp2->child->TI->val.valIfInt;
                        }
                        else {
                            index = temp2->child->sibling->TI->val.valIfInt;
                            if(temp2->child->name==L_MINUS){
                                index = index * (-1);
                            }
                        }
                        int left_bound, right_bound;
                        if(temp->var->start_ind_neg){
                            left_bound = temp->var->start_ind.num*(-1);
                        }
                        else{
                            left_bound = temp->var->start_ind.num;
                        }

                        if(temp->var->end_ind_neg){
                            right_bound = temp->var->end_ind.num*(-1);
                        }
                        else{
                            right_bound = temp->var->end_ind.num;
                        }
                        // int index = temp2->TI->val.valIfInt;
                        if(index<left_bound || index>right_bound){
                            printf("SEMANTIC ERROR: array index out of bound in line %d\n", temp2->TI->lineNo);
                            // return ERROR_OCCURED;
                        }
                    }
                }
                // return temp->var->dataType;
            }
            else{
                printf("SEMANTIC ERROR: invalid array access of array %s in line %d (Expected expression of INTEGER type)\n", node->child->TI->val.lexeme, node->child->TI->lineNo);
                // return ERROR_OCCURED;
            }
            // if(temp->var->dataType==L_INTEGER){
            //     return L_NUM;
            // }
            // else if (temp->var->dataType==L_REAL){
            //     return L_RNUM;
            // }
            return temp->var->dataType;
            
        }
        if(temp->var->dataType != L_ARRAY){
            printf("SEMANTIC ERROR: %s is not of array type in line %d\n", node->child->TI->val.lexeme, node->child->TI->lineNo);
            return ERROR_OCCURED; //SHOULD SAVE SEMANTIC ERROR HERE
        }
        
    }
    else{
        if(name==L_TRUE || name==L_FALSE){
            return L_BOOLEAN;
        }
        else if(name==L_NUM){
            return L_INTEGER;
        }
        else if(name==L_RNUM){
            return L_REAL;
        }
        return name;
    }
}
int checkSwitch(nodeAST node, data_ptr* ptr)
{
    Label dataType= ptr->name;
    if(dataType==L_REAL)
    {
        printf("SEMANTIC ERROR : Switch identifier %s of type REAL is not allowed in line %d.\n", node->TI->val.lexeme ,node->child->TI->lineNo); 
        return -1;
    }
    else if(dataType==L_ARRAY)
    {
        printf("SEMANTIC ERROR : Switch identifier %s of type ARRAY is not allowed in line %d.\n", node->TI->val.lexeme ,node->child->TI->lineNo); 
        return -1;
    }
    else if(dataType== L_BOOLEAN)
    {
        if(node->child->sibling->sibling!= NULL && node->child->sibling->sibling->name== L_DEFAULT)
        {
            printf("SEMANTIC ERROR : Default case not allowed with switch identifier %s of type BOOLEAN in line %d.\n", node->child->TI->val.lexeme ,node->child->sibling->sibling->TI->lineNo );
        }
        if(node->child->sibling && node->child->sibling->name== L_CASESTMTS)
        {
            nodeAST temp= node->child->sibling;
            if(temp->list_head->child->name!=L_TRUE && temp->list_head->child->name!=L_FALSE)
            {
                printf("SEMANTIC ERROR: Switch with BOOLEAN identifier can have TRUE/FALSE case only in line %d. \n", temp->list_head->child->TI->lineNo);
                return 0;
            }
            if(temp->list_head->child->name==L_TRUE) 
            {
                if(!temp->list_head->sibling->child->name==L_FALSE)
                    printf("SEMANTIC ERROR: Switch with BOOLEAN identifier should have FALSE case in line %d. \n", temp->list_head->sibling->child->TI->lineNo);
                    return 0;
            }
            if(temp->list_head->child->name==L_FALSE)// || (temp->list_head->child->name==L_FALSE && temp->list_head->child->sibling->sibling==L_TRUE) && temp->list_head->child->sibling->sibling->sibling== NULL))
            {
                if(!temp->list_head->sibling->child->name==L_TRUE)
                    printf("SEMANTIC ERROR: Switch with BOOLEAN identifier should have TRUE case in line %d.\n", temp->list_head->sibling->child->TI->lineNo);
                    return 0;
            }
            
            if(temp->list_head->child->sibling->sibling->sibling != NULL)
            {
                printf("SEMANTIC ERROR: Switch with BOOLEAN identifier can have only 2 cases in line %d. \n", temp->list_head->child->sibling->sibling->sibling->TI->lineNo);
                return 0;
            }
        }
    }
    else if(dataType== L_INTEGER)
    {
        if(node->child->sibling->sibling== NULL || node->child->sibling->sibling->name!= L_DEFAULT)
        {
            printf("SEMANTIC ERROR: Switch with INTEGER identifier should have DEFAULT case.\n");
            return 0;
        }
        else
        {
            nodeAST temp= node->child->sibling->list_head;
            while(temp)
            {
                if(temp->name!= L_NUM)
                {
                    printf("SEMANTIC ERROR: Switch with BOOLEAN identifier should have INTEGER case in line %d. \n", temp->TI->lineNo);
                    return 0;
                }
                // else
                // {
                //     constructSymbolTable();
                // }
                temp= temp->sibling;
            }

        }
        return 0;
    }
}
// void constructWhileList(nodeAST root, int line){
//     if(root==NULL) return;
//     if(root->name == L_ID ){
//         whileEx* node = (whileEx*) malloc(sizeof(whileEx));
//         node->type = root->;
//         node->lineNo = line;
//         node->isAssinged= false;
//         node->next = NULL;

//     }
// }
void constructSymbolTable(nodeAST root)
{
    if(root==NULL)
        return;
        
    nodeAST node= root;

    if(node->name < 29) //It node is non terminal
    {
        if(node->name== L_DRIVERMODULE || node->name== L_MODULE)
        {
            data_ptr* funcPtr=NULL;
            if(node->name==L_MODULE)
            {
                // printf(" Check mudule name: %s", node->child->TI->val.lexeme);
                char str[LEXEME_LEN];
                strcpy(str, node->child->TI->val.lexeme);
                funcPtr= funcEntry(mainSym, str, node->child->sibling, node->child->sibling->sibling, node->child->TI->lineNo);
                strcpy(node->funcName, node->child->TI->val.lexeme);
                node->ptrTable= mainSym;
            }
            else
            {
                funcPtr= funcEntry(mainSym, "Driver", NULL, NULL, 5);  
                strcpy(node->funcName, "Driver");    
                node->ptrTable= mainSym;      
            }
            //whether add the entry ptr in ast node or not?
            
            if(funcPtr)
            {
                symbolTable* newSym = (symbolTable *) malloc(sizeof(symbolTable));
                newSym->parent= currSym;
                newSym->child = NULL;
                newSym->sibling = NULL;
                if(currSym)  
                    newSym->level = currSym->level+1;
                else 
                    newSym->level = 0;

                funcPtr->func->funcTable= newSym;
                currSym= newSym;
            } 
            
        }
       
        else{
            if(node->parent)
                strcpy(node->funcName, node->parent->funcName);
            else
                strcpy(node->funcName, "MAIN_PROGRAM");
        }
        if(scope_change(node->name)==true) //New scope check
        {
            symbolTable* newSym = (symbolTable *) malloc(sizeof(symbolTable));
            newSym->parent= currSym;
            newSym->child = NULL;
            newSym->sibling = NULL;
            
            if(currSym)  
                newSym->level = currSym->level+1;
            else 
                newSym->level = 0;
                
            if(mainSym==NULL)
                mainSym=newSym;
            
            if(currSym){
                symbolTable* temp = currSym->child;
                if(!temp) 
                    currSym->child = newSym;
                else{
                    while(temp->sibling)
                        temp = temp->sibling;
                    temp->sibling = newSym;
                }
            }
            currSym = newSym;
            
        }
    }
    else //node is terminal
    {
        if(node->parent)
        {    
            strcpy(node->funcName, node->parent->funcName);
            // node->ptrTable= currSym;
        }
        if(node->name== L_ID)
        {
            void* exist = search_table(currSym->localTable, node->TI->val.lexeme);
            if(isDeclared(node->parent)==true)
            {
                if(!exist)
                {
                    // if(node->parent->name!= L_MODULEDECLARATIONS)
                    // {    
                        if(checkOPList(node->TI->val.lexeme, node->funcName))
                        {
                            //ERROR
                            printf("Redeclaration of output parameter %s\n", node->TI->val.lexeme);
                        }
                    // }
                    else
                    {
                        IDEntry(currSym, node);
                        node->ptrTable= currSym;
                    }
                    
                    //Check if new variable is not same as control variable of for loop
                    nodeAST temp= node->parent;
                    while(temp->name!= L_MODULE && temp->name!= L_DRIVERMODULE && temp->name!= L_MODULEDECLARATIONS)
                    {
                        if(temp->name==L_FOR)
                        {
                            if(strcmp(temp->child->TI->val.lexeme, node->TI->val.lexeme)==0)
                            {
                                printf("Redeclaration of for loop control variabe %s.\n", node->TI->val.lexeme);
                            }
                        }
                        temp= temp->parent;
                    }
                }
                else
                {
                    //ERROR
                    printf("Redeclaration of variable %s in same scope in line %d.\n ", node->TI->val.lexeme, node->TI->lineNo);
                }
            }
            else 
            {
                data_ptr* isPresent= NULL;
                // if(node->parent && node->parent->parent && (node->parent->parent->name != L_INPUT_PLIST && node->parent->parent->name != L_OUTPUT_PLIST))
                // {
                    bool isOutPara = false;
                    isPresent= return_type(node, currSym, &isOutPara);
                    if(!isPresent){
                        printf("Variable %s not declared before use in line %d. \n", node->TI->val.lexeme, node->TI->lineNo);
                    }
                // }

                if(node->parent && node->parent->name == L_SWITCH) //Check Semantic errors in Switch statement
                {
                    if(!isPresent) return;
                    int check = checkSwitch(node->parent, isPresent);
                    if(check == -1) return;
                    
                }
                if(node->parent && (node->parent->name == L_ASSIGN || node->parent->name== L_ARRAY_ASSIGN)) //Check Semantic errors in Assignment statement
                {
                    if(!isPresent) return;
                    Label LHSType= isPresent->name;
                    if(LHSType!= L_ARRAY)
                    {
                        constructSymbolTable(node->sibling);
                        int RHSType= expressionType(node->sibling);
                        if(RHSType!=-1)
                        {
                            if(LHSType!= RHSType)
                            {
                                printf("SEMANTIC ERROR: Assigning %s type value to %s in line %d.\n", Label_str[RHSType], Label_str[LHSType], node->TI->lineNo);
                            }
                        }
                        
                    }
                    else
                    {
                        LHSType= isPresent->var->dataType;
                        constructSymbolTable(node->sibling);
                        int LHSCheck = expressionType(node->parent);
                        if(LHSCheck!=-1)
                        {
                            // int indexType= expressionType(node->sibling);
                            // if(indexType != L_INTEGER)
                            // {
                            //     printf("SEMANTIC ERROR: Array Index should be of INTEGER type in Line %d.\n", node->TI->lineNo);
                            // // }
                            // else
                            // {
                                constructSymbolTable(node->sibling->sibling);
                                int RHSType= expressionType(node->sibling->sibling);
                                if(RHSType!=-1)
                                {
                                    if(LHSType!= RHSType)
                                    {
                                        printf("SEMANTIC ERROR: 2\n");
                                    }
                                }
                            // }
                        }
                        
                    }
                    return;
                }

                nodeAST temp= node->parent;
                while(temp->name!= L_MODULE && temp->name!= L_DRIVERMODULE && temp->name!= L_MODULEDECLARATIONS)
                {
                    if(temp->name==L_FOR && node->parent->name!=L_FOR)
                    {
                        if(strcmp(temp->child->TI->val.lexeme, node->TI->val.lexeme)==0)
                        {
                            printf("Assigning value to for loop control variabe %s.\n", node->TI->val.lexeme);
                        }
                    }
                    temp= temp->parent;
                }
            }
        }
    }

    if(node->child)
        constructSymbolTable(node->child);
    else if(node->list_head)
        constructSymbolTable(node->list_head);

    if(scope_change(node->name)==true || node->name==L_MODULE || node->name==L_DRIVERMODULE)
    {
        currSym = currSym->parent;
    } 
    if(node->sibling) 
        constructSymbolTable(node->sibling);

    if(currSym== mainSym && (node->name== L_MODULE || node->name== L_DRIVERMODULE ))
    {
        data_ptr* ptr = (data_ptr*) search_table(mainSym->localTable, node->funcName);
        if(ptr){
            list* outNode= ptr->func->opList;
            while(outNode){
                if(outNode->type->isAssigned == false){
                    printf("Parameter %s returned by the function are not assigned.\n", outNode->lexeme);
                }
                outNode = outNode->next;
            }
        }

    }
} 



void semantic_check(nodeAST root){
    if(root==NULL)
        return;
        
    nodeAST node= root;

    // if(node->name== L_SWITCH)
    // {
    //     checkSwitch(node);
    // }

    if(node->child)
        semantic_check(node->child);
    else if(node->list_head)
        semantic_check(node->list_head);

    // if(scope_change(node->name)==true || node->name==L_MODULE || node->name==L_DRIVERMODULE)
    // {
    //     currSym = currSym->parent;
    // } 
    if(node->sibling) 
        semantic_check(node->sibling);
}
void printSymbolTable(symbolTable* table)
{
    if(table== NULL)
    {
        return;
    }
    for(int i=0; i<TABLE_SIZE; i++)
    {   
        if(table->localTable[i])
        { 
            data_ptr* ptr= (data_ptr*) table->localTable[i]->entry;
            if(ptr!=NULL)
            {
                if(ptr->name== L_MODULE)
                {
                    printf("Module Name: %s \n", ptr->tokName);
                    if(ptr->func->ipList!=NULL)
                    {
                        printf("Input List: ");
                        list* inList= ptr->func->ipList;
                        while(inList)
                        {
                            printf("%s, ",inList->lexeme);
                            inList=inList->next;
                        }
                    }
                    if(ptr->func->opList!=NULL)
                    {
                        printf("Output List: ");
                        list* outList= ptr->func->opList;
                        while(outList)
                        {
                            printf("%s \n",outList->lexeme);
                            outList=outList->next;
                        }
                    }
                    printSymbolTable(ptr->func->funcTable);
                }
                else
                {
                    printf("ID Name: %s \n", ptr->tokName);
                    printf("Parent function: %s \n", ptr->var->parentFunc);
                }
            }
        }
    }
    if(table->child)
        printSymbolTable(table->child);
    else if(table->sibling)
        printSymbolTable(table->sibling);
}

void printSymbolTable2(symbolTable *table)
{
    if (table == NULL)
    {
        // printf("NULL------");
        return;
    }
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (table->localTable[i])
        {
            // printf("table->localTable[i] : ");
            data_ptr *ptr = (data_ptr *)table->localTable[i]->entry;
            if (ptr->var != NULL)
            {
                // printf("\nPTR->VAR NOT NULL\n");
                printf("%s\t\t", ptr->tokName);
                printf("%s\t\t", ptr->var->parentFunc);
                printf("[%d- ]\t\t", ptr->line_no);
                if (strcmp("L_INTEGER", Label_str[ptr->var->dataType]) == 0)
                {
                    printf("Integer\t\t");
                }
                else if (strcmp("L_REAL", Label_str[ptr->var->dataType]) == 0)
                {
                    printf("Real\t\t");
                }
                else
                    printf("Boolean\t\t");
                // printf("%s\t\t", Label_str[ptr->var->dataType]);
                if (strcmp("L_ARRAY", Label_str[ptr->name]) == 0)
                {
                    printf("Yes\t\t");
                    if (ptr->var->isDynamic.starInd || ptr->var->isDynamic.endInd)
                    {
                        printf("Dyanamic\t\t");
                        printf("**\t\t");
                    }
                    else
                    {
                        printf("Static\t\t");
                        if (ptr->var->start_ind_neg && ptr->var->end_ind_neg)
                        {
                            printf("[-%d,-%d]\t\t", ptr->var->start_ind.num, ptr->var->end_ind.num);
                        }
                        else if (ptr->var->start_ind_neg)
                        {
                            printf("[-%d,%d]\t\t", ptr->var->start_ind.num, ptr->var->end_ind.num);
                        }
                        else if (ptr->var->start_ind_neg)
                        {
                            printf("[%d,-%d]\t\t", ptr->var->start_ind.num, ptr->var->end_ind.num);
                        }
                        else
                            printf("[%d,%d]\t\t", ptr->var->start_ind.num, ptr->var->end_ind.num);
                    }
                }
                else
                {
                    printf("No\t\t");
                    printf("**\t\t");
                    printf("**\t\t");
                }
                printf("%d\t\t", ptr->width);
                printf("%d\t\t", ptr->offset);
                printf("%d\t\t\n", table->level);
            }

            else
            {
                if (ptr->func != NULL)
                {
                    printSymbolTable2(ptr->func->funcTable);
                }
            }
        }
    }
    if (table->child)
        printSymbolTable2(table->child);
    else if (table->sibling)
        printSymbolTable2(table->sibling);
}

void printAST(nodeAST root)
{
        if (root == NULL)
                return;
        printf("\n");
        if(strcmp("L_ASSIGN",Label_str[root->name])==0){
            int n = expressionType(root->child->sibling);
            if (n != -1)
            {
                printf("\nexpressionType Output --- %s\n", Label_str[n]);
            }
            else
            {
                printf("\nexpressionType Output --- %d\n", n);
            }
        }
        printf("%s\t\t", Label_str[root->name]);
        if (root->TI)
        {
                tokenInfo *token = root->TI;
                if (strcmp((token->tok), "NUM") == 0)
                {
                        printf("%d\t\t", token->val.valIfInt);
                }
                else if (strcmp(token->tok, "RNUM") == 0)
                {
                        printf("%f\t\t", token->val.valIfFloat);
                }
                else
                {
                        printf("%s\t\t", token->val.lexeme);
                }
                // printf("%s\t\t", root->TI->tok);
                printf("%d\t\t", root->TI->lineNo);
        }
        else
        {
                printf("**\t\t");
                printf("**\t\t");
        }
        if (root->parent)
        {
                printf("%s\t\t", Label_str[root->parent->name]);
        }
        else
                printf("**\t\t");
        int flag1, flag2, flag3 = 0;
        if (root->child)
        {
                printf("%s\t\t", Label_str[root->child->name]);
                flag1 = 1;
                // printAST(root->child);
        }
        else if (root->list_head)
        {
                printf("%s\t\t", Label_str[root->list_head->name]);
                flag2 = 1;
                // printAST(root->list_head);
        }
        else
                printf("**\t\t");
        if (root->sibling)
        {
                printf("%s", Label_str[root->sibling->name]);
                flag3 = 1;
                // printAST(root->sibling);
        }
        else
                printf("**");
        if (flag1)
        {
                // printf("C -- ");
                printAST(root->child);
        }
        if (flag2)
        {
                // printf("L -- ");
                printAST(root->list_head);
        }
        if (flag3)
        {
                // printf("S -- ");
                printAST(root->sibling);
        }

        return;
}


// void printSymbolTable(symbolTable* table){
    
// }



void main(int argc, char *argv[])
{
        fillKeywordTable();
        // bufferSize = atoi(argv[3]);
        // bufferSize = argv[3];
        bufferSize=1000;
        // file_ptr = fopen(argv[1], "r");
        file_ptr = fopen("t5.txt", "r");
        if (file_ptr == NULL)
        {
        printf("File Opening Error\n");
        }

        // fileParseTree = fopen(argv[2], "w");
        fileParseTree = fopen("traversalFile.txt", "w");
        treeNode* parseRoot= stackImp();
        nodeAST astRoot= ast_main(parseRoot);
        printf("\n PRINTING AST \n");
        // printAST(astRoot);

        constructSymbolTable(astRoot);
        printf("\n PRINTING SYMBOL TABLE \n");
        //printSymbolTable2(mainSym);
        // printAST(astRoot);
        fclose(file_ptr);
        fclose(fileParseTree);
}