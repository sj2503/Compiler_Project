/*  Group No : 31
    Anushka Jain 2019B1A70904P
    Ritu Chandna 2019B4A70667P
    Payal Basrani 2019B5A70809P
    Ujjwal Jain 2019B4A70647P
    Shobhit Jain 2019B3A70385P
*/

#include "parser.h"
#define SYNC_NUM 5
char *Termstr[] = {
    "EPSILON", "DOLLAR", "DECLARE", "MODULE", "SEMICOL", "DRIVERDEF", "DRIVER", "PROGRAM",
    "DRIVERENDDEF", "DEF", "ID", "ENDDEF", "TAKES", "INPUT", "SQBO", "SQBC",
    "RETURNS", "COMMA", "COLON", "INTEGER", "REAL", "BOOLEAN", "ARRAY", "OF",
    "START", "END", "GET_VALUE", "BO", "BC", "PRINT", "NUM", "RNUM",
    "ASSIGNOP", "USE", "WITH", "PARAMETERS", "PLUS", "MINUS", "MUL", "DIV",
    "LT", "LE", "GT", "GE", "EQ", "NE", "AND", "OR", "SWITCH", "CASE", "BREAK", "TRUE", "FALSE",
    "DEFAULT", "FOR", "IN", "WHILE", "RANGEOP",

    // non terminals start here
    "program", "moduleDeclarations", "moduleDeclaration", "otherModules", "driverModule", "module",
    "ret", "input_plist", "input_plist2", "output_plist", "output_plist2", "dataType", "range_arr", "index_arr",
    "ind", "sign", "type", "moduleDef", "statements", "statement", "ioStmt", "var2", "boool", "whichID", "simpleStmt",
    "assignmentStmt", "whichStmt", "lvalueIDStmt", "lvalueArrStmt", "moduleReuseStmt", "optional", "paraList", "paraList2", "var",
    "idList", "idList2", "expression", "arithmeticOrBoolean", "term1", "arithmeticExpr", "arithmeticExpr2", "term2", "term3", "factor",
    "whichID2", "relationalExpr", "relationalOP", "logicalExpr", "logicalOP", "op1", "op2", "unaryExpr", "new_NT", "unaryOP", "idNumRnum",
    "arrExpr", "term6", "arithmeticExprArr", "arithmeticExprArr2", "term4", "term5", "factor2", "declareStmt",
    "conditionalStmt", "caseStmts", "caseStmts2", "value", "default1", "iterativeStmt", "range_loop", "index_loop"};

char *Termstr[];
int parseTable[COUNT_NT][COUNT_T - 1]; // removing epsilon from count of terminals
unsigned long long int first[COUNT_NT];
unsigned long long int follow[COUNT_NT];
gram *gramArr[NRULES];
char parseTable2[COUNT_NT][COUNT_T - 1][30];
char firstset[COUNT_NT][150];
char followset[COUNT_NT][150];
bool isError;
FILE* fileParseTree;

Term syncSet[SYNC_NUM]={SEMICOL, ENDDEF, START, statements, caseStmts};

void initializeStack(Stack *s)
{
    s->top = NULL;
}

void pushStack(Stack *s, Term data, treeNode *addre)
{
    if (s->top == NULL)
    {
        initializeStack(s);
    }

    StackEle *ele = (StackEle *)malloc(sizeof(StackEle));
    ele->term = data;
    ele->next = NULL;
    ele->addr = addre;
    StackEle *temp = s->top;
    s->top = ele;
    if (s->top != NULL)
        s->top->next = temp;
}

bool isStackEmpty(Stack *s)
{
    if (s->top == NULL)
        return true;
    return false;
}

void popStack(Stack *s)
{
    if (isStackEmpty(s))
        return;
    StackEle *temp = (StackEle *)malloc(sizeof(StackEle));
    temp = s->top;
    if (s->top != NULL)
        s->top = s->top->next;
    // free(temp);
    return;
}

StackEle *getTopStack(Stack *s)
{
    if (s->top == NULL)
    {
        printf("Stack is empty! \n");
        return NULL;
    }
    return s->top;
}

int stringToENUM(char *str)
{
    for (int i = 0; i < COUNT_NTandT; i++)
    {
        if (strcmp(Termstr[i], str) == 0)
        {
            return i;
        }
    }
}

unsigned long long int calUnion(unsigned long long int x, unsigned long long int y)
{
    return x | y;
}

int terminalInSet(unsigned long long int x, int term)
{
    return (x & (unsigned long long int)1 << term) ? 1 : 0;
}

unsigned long long int findFirst(Term y)
{
    int x = y - COUNT_T;
    if (first[x])
        return first[x];
    for (int i = 0; i < NRULES; i++)
    {
        if (gramArr[i]->name != y)
            continue;
        gram *node = gramArr[i]->next;
        if (node->type == 1)
        {
            first[x] = calUnion(first[x], ((unsigned long long int)1 << node->name));
        }
        else
        {
            unsigned long long int fst = findFirst(node->name);
            gram *node2 = node;
            while (fst % 2 && node2->next != NULL)
            {
                fst--;
                first[x] = calUnion(first[x], fst);
                node2 = node2->next;
                if (node2->type == 1)
                    break;
                fst = findFirst(node2->name);
            }

            if (node2->type == 1)
                first[x] = calUnion(first[x], ((unsigned long long int)1 << node2->name));

            else
                first[x] = calUnion(first[x], fst);
        }
    }
    return first[x];
}

unsigned long long int firstRHS(gram *node)
{
    unsigned long long int fst = 0;
    if (node->type == 1)
    {
        fst = calUnion(fst, ((unsigned long long int)1 << node->name));
    }
    else
    {
        gram *node2 = node;
        while (node2)
        {
            int ind = node2->name - COUNT_T;
            fst = calUnion(fst, first[ind]);
            if (first[ind] % 2 == 0)
            {
                if (fst % 2)
                    fst--;
                break;
            }
            node2 = node2->next;
            if (node2 != NULL && node2->type == 1)
                break;
        }
        if (node2 != NULL && node2->type == 1)
        {
            fst = calUnion(fst, ((unsigned long long int)1 << node2->name));
            if (fst % 2)
                fst--;
        }
    }
    return fst;
}

unsigned long long int findFollow(Term y)
{
    int x = y - COUNT_T;
    if (follow[x] != 0 && y != COUNT_T) // enum value COUNT_T for program
    {
        return follow[x];
    }
    if (y == COUNT_T)
    {
        follow[x] = calUnion(follow[x], ((unsigned long long int)1 << 1));
    }
    for (int i = 0; i < NRULES; i++)
    {
        gram *curr = gramArr[i]->next;
        while (curr != NULL)
        {
            if (curr->type == 1)
            {
                curr = curr->next;
                continue;
            }
            else
            {
                if (curr->name == y)
                {
                    unsigned long long int fst = 0;
                    gram *beta = curr->next;
                    if (beta == NULL)
                    {
                        if (gramArr[i]->name != y)
                        {
                            findFollow(gramArr[i]->name);
                            follow[x] = calUnion(follow[x], follow[gramArr[i]->name - COUNT_T]);
                            curr = curr->next;
                            continue;
                        }
                    }
                    else if (beta != NULL)
                        fst = firstRHS(beta);

                    follow[x] = calUnion(follow[x], fst);

                    if (follow[x] % 2)
                        follow[x]--;

                    if (fst % 2)
                    {
                        if (gramArr[i]->name != y)
                        {
                            findFollow(gramArr[i]->name);
                            follow[x] = calUnion(follow[x], follow[gramArr[i]->name - COUNT_T]);
                        }
                    }
                }
                curr = curr->next;
            }
        }
    }
    return follow[x];
}

void findFirstAndFollow()
{
    for (int i = COUNT_T; i < COUNT_NTandT; i++)
    {
        first[i - COUNT_T] = 0;
        findFirst(i);
    }
    for (int i = COUNT_T; i < COUNT_NTandT; i++)
    {
        follow[i - COUNT_T] = 0;
        findFollow(i);
    }
}

void printFirstAndFollow()
{
    for (int i = 0; i < COUNT_NT; i++)
    {
        for (int j = 0; j < COUNT_T; j++)
        {
            char str1[30], str2[30];
            if (first[i] >> j & 1)
            {
                strcpy(str1, Termstr[j]);
                strcat(firstset[i], str1);
                strcat(firstset[i], " ");
            }
            if (follow[i] >> j & 1)
            {
                strcpy(str2, Termstr[j]);
                strcat(followset[i], str2);
                strcat(followset[i], " ");
            }
        }
    }
    for (int x = 0; x < COUNT_NT; x++)
    {
        printf("First of %s: %s \n", Termstr[x + COUNT_T], firstset[x]);
    }
    printf("\n");
    for (int y = 0; y < COUNT_NT; y++)
    {
        printf("Follow of %s: %s \n", Termstr[y + COUNT_T], followset[y]);
    }
}

void generateGram(char *file)
{
    FILE *fp = fopen(file, "r");
    int i = 0;
    if (fp == NULL)
    {
        printf("File not opened \n");
        return;
    }
    char line[201];
    while (fgets(line, 200, fp) != NULL)
    {
        char *tok = strtok(line, " \n");
        int val = stringToENUM(tok);
        gramArr[i] = (gram *)malloc(sizeof(gram));
        gramArr[i]->name = val;
        strcpy(gramArr[i]->strName, tok);
        gramArr[i]->type = 0;
        gramArr[i]->next = NULL;
        gram *g = gramArr[i];
        while (tok = strtok(NULL, " \n"))
        {
            val = stringToENUM(tok);
            g->next = (gram *)malloc(sizeof(gram));
            g->next->name = val;
            strcpy(g->next->strName, tok);
            g->next->next = NULL;
            if (tok[0] > 64 && tok[0] < 91)
            {
                g->next->type = 1;
            }
            else
            {
                g->next->type = 0;
            }
            g = g->next;
        }
        i++;
    }
}

void printGramTable()
{
    int i = 0;
    while (i < NRULES)
    {
        gram *g = gramArr[i];
        printf("Rule %d: %s ->", i + 1, g->strName);
        g = g->next;
        while (g)
        {
            printf(" %s ", g->strName);
            g = g->next;
        }
        printf("\n");
        i++;
    }
}

void buildParseTable()
{

    for (int i = 0; i < COUNT_NT; i++)
    {
        unsigned long long int foll = follow[i];
        for (int j = 0; j < COUNT_T - 1; j++) // removing epsilon from count of terminals
        {
            parseTable[i][j] = -1;
            if (terminalInSet(foll, j + 1))
            {
                parseTable[i][j] = -2;
            }
        }
    }
    for (int i = 0; i < NRULES; i++)
    {
        unsigned long long int fst = firstRHS(gramArr[i]->next);
        for (int j = 1; j < COUNT_T; j++)
        {
            if (terminalInSet(fst, j))
            {
                parseTable[gramArr[i]->name - COUNT_T][j - 1] = i + 1;
            }
        }
        if (terminalInSet(fst, 0))
        {
            for (int j = 1; j < COUNT_T; j++)
            {
                if (terminalInSet(follow[gramArr[i]->name - COUNT_T], j))
                {
                    parseTable[gramArr[i]->name - COUNT_T][j - 1] = i + 1;
                }
            }
        }
    }
}

void buildParseTable2() // To check grammar
{
    char str[50];
    for (int i = 0; i < COUNT_NT; i++)
    {
        unsigned long long int foll = follow[i];
        for (int j = 0; j < COUNT_T - 1; j++) // removing epsilon from count of terminals
        {
            strcpy(parseTable2[i][j], "-1");
            if (terminalInSet(foll, j + 1))
            {
                strcat(parseTable2[i][j], "-2");
            }
        }
    }
    for (int i = 0; i < NRULES; i++)
    {
        unsigned long long int fst = firstRHS(gramArr[i]->next);
        for (int j = 1; j < COUNT_T; j++)
        {
            if (terminalInSet(fst, j))
            {
                sprintf(str, " %d", i + 1);
                strcat(parseTable2[gramArr[i]->name - COUNT_T][j - 1], str);
            }
        }
        if (terminalInSet(fst, 0))
        {
            for (int j = 1; j < COUNT_T; j++)
            {
                if (terminalInSet(follow[gramArr[i]->name - COUNT_T], j))
                {
                    sprintf(str, " %d", i + 1);
                    strcat(parseTable2[gramArr[i]->name - COUNT_T][j - 1], str);
                }
            }
        }
    }
}

void errorReport(int errNo, Stack *st, tokenInfo *L)
{
    isError = true;
    if (errNo == 1 && !isStackEmpty(st))
    {
        if (strcmp(L->tok, "NUM") == 0)
            printf("Line %u : Error in the input as '%d' was not expected! Expected token is %s. \n\n", L->lineNo, L->val.valIfInt, Termstr[getTopStack(st)->term]);

        else if (strcmp(L->tok, "RNUM") == 0)
            printf("Line %u : Error in the input as '%f' was not expected! Expected token is %s. \n\n", L->lineNo, L->val.valIfFloat, Termstr[getTopStack(st)->term]);

        else
            printf("Line %u : Error in the input as '%s' was not expected! Expected token is %s. \n\n", L->lineNo, L->val.lexeme, Termstr[getTopStack(st)->term]);
    }
    else if (errNo == 2)
    {
        if (strcmp(L->tok, "NUM") == 0)
            printf("Line %u : Error in the input as '%d' was not expected. \n\n", L->lineNo, (L->val).valIfInt);

        else if (strcmp(L->tok, "RNUM") == 0)
            printf("Line %u : Error in the input as '%f' was not expected. \n\n", L->lineNo, (L->val).valIfFloat);

        else
            printf("Line %u : Error in the input as '%s' was not expected. \n\n", L->lineNo, (L->val).lexeme);
    }
    else if (errNo == 3)
    {
        printf("Input is consumed and stack is not empty!\n\n");
    }
    else if (errNo == 4)
    {
        printf("Line %d : Input is remaining and stack is empty!\n\n", L->lineNo);
    }
    else if (errNo == 5)
    {
        printf("Line %d : Error in the input as ';' missing!\n\n", L->lineNo - 1);
    }
    else
        printf("Error found\n\n");
}

tokenInfo *errorHandling(int errNo, Stack *st, tokenInfo *L)
{
    if (errNo == 1)
    {
        popStack(st);
        // while (!isStackEmpty(st) && getTopStack(st)->term < COUNT_T)
        // {
        //     if (strcmp(Termstr[getTopStack(st)->term], L->tok) == 0)
        //     {
        //         return L;
        //     }
        //     popStack(st);
        // }
        while (!isStackEmpty(st))
        {

            int flag = 0;
            if (getTopStack(st)->term < COUNT_T && strcmp(Termstr[getTopStack(st)->term], L->tok) == 0)
            {
                return L;
            }
            for (int i = 0; i < SYNC_NUM; i++)
            {
                if (syncSet[i] == getTopStack(st)->term)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {
                break;
            }
            popStack(st);
        }
        if (!isStackEmpty(st) && (getTopStack(st)->term == SEMICOL || getTopStack(st)->term == ENDDEF))
        {
            popStack(st);
        }
        unsigned int lno = L->lineNo;
        while (L != NULL && L->lineNo == lno)
        {
            if (strcmp(L->tok, "SEMICOL") == 0)
            {
                L = getNextToken();
                break;
            }
            L = getNextToken();
        }
    }
    else if (errNo == 2)
    {
        StackEle *x = getTopStack(st);
        while (L != NULL && parseTable[x->term - COUNT_T][stringToENUM(L->tok) - 1] == -1)
        {
            L = getNextToken();
        }
        if (L == NULL)
        {
            while (!isStackEmpty(st))
                popStack(st);
        }
        if (L != NULL && !isStackEmpty(st) && parseTable[x->term - COUNT_T][stringToENUM(L->tok) - 1] == -2)
        {
            popStack(st);
        }
    }
    // else if (errNo == 2)
    // {
    //     StackEle *x = getTopStack(st);
    //     unsigned int lno = L->lineNo;
    //     while (L != NULL && L->lineNo == lno && parseTable[x->term - COUNT_T][stringToENUM(L->tok) - 1] == -1)
    //     {
    //         if(strcmp(L->tok, "SEMICOL") == 0)
    //         {
    //             L = getNextToken();
    //             break;
    //         }
    //         L = getNextToken();
    //     }
    //     if (L != NULL && !isStackEmpty(st) && parseTable[x->term - COUNT_T][stringToENUM(L->tok) - 1] == -2)
    //     {
    //         popStack(st);
    //         return L;
    //     }
    //     else if(L->lineNo!=lno)
    //     {
    //         int flag = 0;
    //         while (!isStackEmpty(st))
    //         {
    //             for (int i = 0; i < SYNC_NUM; i++)
    //             {
    //                 if (syncSet[i] == getTopStack(st)->term)
    //                 {
    //                     flag = 1;
    //                     break;
    //                 }
    //             }
    //             if (flag == 1)
    //             {
    //                 break;
    //             }
    //             popStack(st);
    //         }
    //         if (!isStackEmpty(st) && (getTopStack(st)->term == SEMICOL || getTopStack(st)->term == ENDDEF))
    //         {
    //             popStack(st);
    //         }
    //     }
    //     if (L == NULL)
    //     {
    //         while (!isStackEmpty(st))
    //             popStack(st);
    //     }
        
    // }
    else if (errNo == 3)
    {
        while (!isStackEmpty(st))
        { // check case of dollar
            popStack(st);
        }
    }

    else if (errNo == 4)
    {
        while (getNextToken() != NULL)
            ;
    }

    return L;
}

void printtreeNode(treeNode *root, FILE *fp)
{
    if (root != NULL)
    {
        bool isTerminal = (root->name < COUNT_T);
        char *isLeaf = (root->child == NULL) ? "yes" : "no";
        tokenInfo *token = root->TI;
        treeNode *par = root->parent;
        if (isTerminal && token != NULL)
        {
            if (strcmp(token->tok, "NUM") == 0)
            {
                fprintf(fp, "TREENODE---> lexeme: %d, line number: %u, grammar token name: %s, value of number: %d, parent node symbol: %s, is leaf node or not: %s \n\n", (token->val).valIfInt, token->lineNo, Termstr[root->name], (token->val).valIfInt, Termstr[par->name], isLeaf);
            }
            else if (strcmp(token->tok, "RNUM") == 0)
            {
                fprintf(fp, "TREENODE---> lexeme: %f, line number: %u, grammar token name: %s, value of number: %f, parent node symbol: %s, is leaf node or not: %s \n\n", (token->val).valIfFloat, token->lineNo, Termstr[root->name], (token->val).valIfFloat, Termstr[par->name], isLeaf);
            }
            else
                fprintf(fp, "TREENODE---> lexeme: %s, line number: %u, grammar token name: %s, value of number: %s, parent node symbol: %s, is leaf node or not: %s \n\n", (token->val).lexeme, token->lineNo, Termstr[root->name], (token->val).lexeme, Termstr[par->name], isLeaf);
        }
        else
        {
            if (par == NULL)
            {
                fprintf(fp, "TREENODE---> lexeme: ----, grammar token name: ROOT: %s, is leaf node or not: %s\n\n", Termstr[root->name], isLeaf);
            }
            else
            {
                fprintf(fp, "TREENODE---> lexeme: ----, grammar token name: %s, parent node symbol: %s, is leaf node or not: %s\n\n", Termstr[root->name], Termstr[par->name], isLeaf);
            }
        }
    }
}

void treeTraversal(treeNode *root, FILE *fp)
{
    if (root == NULL)
        return;
    if (root->child != NULL)
    {
        treeTraversal(root->child, fp);
    }
    else
    {
        if (root->name >= COUNT_T)
            fprintf(fp, "TREE TRAVERSAL -> EPSILON\n");
        printtreeNode(root, fp);
    }
    if ((root->parent != NULL) && (root->parent->child == root))
    {
        printtreeNode(root->parent, fp);
    }
    if (root->sibling != NULL)
        treeTraversal(root->sibling, fp);
    return;
}

void printParseTree(treeNode *root)
{

    if (fileParseTree == NULL)
    {
        printf("outfile not found");
        exit(0);
    }
    treeTraversal(root, fileParseTree);
    // fclose(fileParseTree);
}

treeNode* stackImp()
{

    char *file = "grammar.txt";
    generateGram(file);
    findFirstAndFollow();
    buildParseTable();
    buildParseTable2();

    if (file_ptr == NULL)
    {
        printf("File Opening Error\n");
    }

    Stack *s = (Stack *)malloc(sizeof(Stack));
    initializeStack(s);
    pushStack(s, DOLLAR, NULL);
    pushStack(s, program, NULL);

    treeNode *root = (treeNode *)malloc(sizeof(treeNode));
    (s->top->addr) = root;
    root->name = program;
    root->parent = NULL;
    root->child = NULL;
    root->sibling = NULL;
    root->ruleNo = 0;
    tokenInfo *L = getNextToken();
    while (L != NULL && !isStackEmpty(s))
    {

        StackEle *X = getTopStack(s);
        int enumVal = X->term;
        bool isTerminal = (enumVal < COUNT_T);

        if (isLexError)
        {
            L = errorHandling(1, s, L);
            isError = true;
            isLexError = false;
        }
        else if (isTerminal)
        {

            if (strcmp(Termstr[X->term], L->tok) == 0)
            {
                if (strcmp(L->tok, "DOLLAR") != 0)
                {
                    X->addr->TI = L;
                    X->addr->name = stringToENUM(L->tok);
                }
                popStack(s);
                L = getNextToken();
            }
            else if (X->term == 4)
            {
                errorReport(5, s, L);
                popStack(s);
            }
            else
            {
                errorReport(1, s, L); // Report Error E1
                L = errorHandling(1, s, L);
            }
        }

        else if (isStackEmpty(s))
        {
            errorReport(4, s, L); // Report Error
            L = errorHandling(4, s, L);
        }

        else if (!(isTerminal))
        {

            if (parseTable[enumVal - COUNT_T][stringToENUM(L->tok) - 1] >= 0)
            {
                int gramRuleNum = parseTable[enumVal - COUNT_T][stringToENUM(L->tok) - 1] - 1;
                treeNode *top_node = getTopStack(s)->addr;
                popStack(s);
                gram *g = gramArr[gramRuleNum];
                g = g->next;
                Stack *temp = (Stack *)malloc(sizeof(Stack));
                initializeStack(temp);
                if(g != NULL && strcmp(g->strName, "EPSILON") == 0)
                {
                    treeNode *node = (treeNode *)malloc(sizeof(treeNode));
                    top_node->child = node;
                    node->parent = top_node;
                    node->sibling = NULL;
                    node->ruleNo = gramRuleNum;
                    node->TI = NULL;
                    node->name= EPSILON;
                }
                else
                {
                    while (g != NULL && (strcmp(g->strName, "EPSILON") != 0))
                    {
                        treeNode *node = (treeNode *)malloc(sizeof(treeNode));
                        if (top_node->child == NULL)
                        {
                            top_node->child = node;
                            node->parent = top_node;
                        }
                        else
                        {
                            if (!isStackEmpty(temp))
                            {
                                if ((getTopStack(temp)->addr) != NULL)
                                {
                                    if (((getTopStack(temp)->addr))->sibling == NULL)
                                    {
                                        (getTopStack(temp)->addr)->sibling = node;
                                    }
                                }
                            }
                        }
                        if (g->type == 0)
                        {
                            node->name = g->name;
                            node->child = NULL;
                            node->sibling = NULL;
                            node->ruleNo = gramRuleNum;
                            node->TI = NULL;
                            node->parent = top_node;
                        }
                        else
                        {
                            node->parent = top_node;
                            node->sibling = NULL;
                            node->ruleNo = gramRuleNum;
                            node->TI = NULL;
                        }
                        StackEle *ele = (StackEle *)malloc(sizeof(StackEle));
                        ele->term = g->name;
                        ele->addr = node;
                        pushStack(temp, g->name, node);
                        g = g->next;
                    }
                }
                
                while (!(isStackEmpty(temp)))
                {
                    if (getTopStack(temp) == NULL)
                    {
                        printf("NULL\n");
                    }

                    StackEle *TI = getTopStack(temp);
                    if (TI == NULL)
                        printf("NULL ELE\n");
                    if (!(isStackEmpty(temp)) && getTopStack(temp) != NULL)
                    {
                        int termVal = getTopStack(temp)->term;
                        treeNode *termTreeNode = getTopStack(temp)->addr;
                        pushStack(s, termVal, termTreeNode);
                    }
                    popStack(temp);
                }
            }
            else
            {
                errorReport(2, s, L); // Report Error 2
                L = errorHandling(2, s, L);
            }
        }

        else
            continue;
    }
    if (!(isStackEmpty(s)))
    {
        errorReport(3, s, L); // Report Error 3
        L = errorHandling(3, s, L);
    }

    if (!isError)
    {
        printf("Input source code is syntactically correct..........\n");
    }
    printParseTree(root);
    // fclose(file_ptr);
    // fclose(fileParseTree);
    return root;
}