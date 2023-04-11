/*  Group No : 31
    Anushka Jain 2019B1A70904P
    Ritu Chandna 2019B4A70667P
    Payal Basrani 2019B5A70809P
    Ujjwal Jain 2019B4A70647P
    Shobhit Jain 2019B3A70385P
*/

#include "lexer.h"

#define KEYWORDSIZE 26 // Size of Keywords Set

FILE *file_ptr;        // File Pointer to Testcase
int state = 1;                // State variable of DFA
unsigned short lNo = 1;       // Line Number Variable
unsigned short tokenCntr = 0; // Token Counter for number of tokens
char *text = NULL;            // Text buffer for storing source code
char *text_twin = NULL;       // Text Twin buffer for storing leftover Text buffer contents
int begin = 0, forward = 0;   // Begin and Forward Variables
int bufferSize = 512;

hashItem *keywordTable[KEYWORDSIZE]; // Hash Buffer

Error errors[100];       // Error Buffer
int errorcntr = 0;       // Error counter
bool isLexError = false; // Flag for Lexical Error

unsigned int hashVal(const char *key) // Function for assigning the Hash Value
{
    unsigned int hashVal = 0;
    const char *t;
    for (t = key; *t != '\0'; t++)
    {
        hashVal = 31 * hashVal + *t; // Hash Function
    }
    return hashVal % KEYWORDSIZE;
}

void putHashPair(char *key, char *value) // Pushing the Hash Element in the Hash Table
{
    int idx = hashVal(key);
    int i = idx;
    do
    {
        if (keywordTable[i] == NULL)
        {
            keywordTable[i] = (hashItem *) malloc(sizeof(hashItem));
            keywordTable[i]->key = strdup(key);
            keywordTable[i]->value = value;
            return;
        }
        if (strcmp(keywordTable[i]->key, key) == 0)
        {
            keywordTable[i]->value = value;
            return;
        }
        i = (i + 1) % KEYWORDSIZE;
    } while (i != idx);
    printf("Sorry! The table has got full!\n");
}

char *getHashPair(char *key) // Getting the Hash Element
{
    int idx = hashVal(key);
    int i = idx;
    do
    {
        if (keywordTable[i] == NULL)
        {
            return NULL;
        }
        if (strcmp(keywordTable[i]->key, key) == 0)
        {
            return keywordTable[i]->value;
        }
        i = (i + 1) % KEYWORDSIZE;
    } while (i != idx);
    return NULL;
}

void fillKeywordTable() // Function for filling the Hash Table
{
    putHashPair("integer", "INTEGER");
    putHashPair("real", "REAL");
    putHashPair("boolean", "BOOLEAN");
    putHashPair("of", "OF");
    putHashPair("array", "ARRAY");
    putHashPair("start", "START");
    putHashPair("end", "END");
    putHashPair("declare", "DECLARE");
    putHashPair("module", "MODULE");
    putHashPair("driver", "DRIVER");
    putHashPair("program", "PROGRAM");
    putHashPair("get_value", "GET_VALUE");
    putHashPair("print", "PRINT");
    putHashPair("use", "USE");
    putHashPair("with", "WITH");
    putHashPair("parameters", "PARAMETERS");
    putHashPair("takes", "TAKES");
    putHashPair("input", "INPUT");
    putHashPair("returns", "RETURNS");
    putHashPair("for", "FOR");
    putHashPair("in", "IN");
    putHashPair("switch", "SWITCH");
    putHashPair("case", "CASE");
    putHashPair("break", "BREAK");
    putHashPair("default", "DEFAULT");
    putHashPair("while", "WHILE");
}

bool tokenised = false;  // Bool variable to check if token has been tokenised
tokenInfo *token = NULL; // Token to be returned by the getNextToken() function
bool isEOF = false;      // Bool Variable to check if End of File has been reached

void tokenize(int begin, int forward, char *buf, char *tokenName, char *dataType) // Tokenise funtion to generate the tokens
{
    tokenInfo *TI = (tokenInfo *)malloc(sizeof(tokenInfo));
    TI->tok = tokenName;

    TI->lineNo = lNo;
    int size = 0;
    int text_twin_len = 0;
    if (text_twin != NULL)
    {
        size = strlen(text_twin) + forward - begin + 1;
        text_twin_len = strlen(text_twin);
    }
    else
        size = forward - begin + 1;
    char *lex = (char *)malloc(sizeof(char) * size);
    for (int i = 0; i < text_twin_len; i++)
    {
        lex[i] = text_twin[i];
    }
    for (int i = begin; i < forward; i++)
        lex[i - begin + text_twin_len] = buf[i];
    lex[size - 1] = '\0';

    tokenised = true;
    if (strcmp(tokenName, "ID") == 0)
    {
        char *hashPair = getHashPair(lex);
        if (hashPair != NULL)
        {
            TI->tok = hashPair;
        }
    }

    if (strcmp(dataType, "INT") == 0)
    {
        (TI->val).valIfInt = atoi(lex);
    }
    else if (strcmp(dataType, "REAL") == 0)
    {
        (TI->val).valIfFloat = atof(lex);
    }
    else
    {
        if (lex != NULL && strlen(lex) <= 20)
        {
            strcpy((TI->val).lexeme, lex);
        }
        else
        {
            TI = NULL;
            printf("Line %d : Error in Lexeme : %s (Exceeds Maximum length for Identifier)\n\n", lNo, lex);
            isLexError = true;
            tokenised = false;
            begin = forward;
            state = 1;
            forward--;
        }
    }
    if (text_twin != NULL)
    {
        text_twin = NULL;
    }

    token = TI;
    tokenCntr++;
}

void error_printing() // Function for printing the Lexical Errors
{
    printf("*****Lexical Errors*****\n");
    for (int i = 0; i < errorcntr; i++)
    {
        printf("Line %d : Error in the input as expected character does not match  %s\n", errors[i].line_no, errors[i].error_name);

        printf("\n");
    }
}

void error_function() // Error function to populate the Errors buffer
{
    char *lex = (char *)malloc(sizeof(char) * (forward - begin + 2));
    for (int i = begin; i <= forward; i++)
        lex[i - begin] = text[i];
    lex[forward - begin + 1] = '\0';
    Error *er = (Error *)malloc(sizeof(Error));
    er->error_name = lex;
    er->line_no = lNo;
    memcpy(&errors[errorcntr], er, sizeof(Error));
    printf("Line %d : Error in the input as expected characters does not match  %s\n\n", er->line_no, er->error_name);
    errorcntr++;
    isLexError = true;
    begin = forward;
    state = 1;
    forward--;
}

FILE *getStream(FILE *file_ptr) // getStream function for populating the Text buffer
{
    if(file_ptr == NULL) {
        printf("NULL PTR\n");
        return file_ptr;
    }
    for (int i = 0; i < bufferSize; i++)
    {
        if ((text[i] = getc(file_ptr)) == EOF)
        {
            text[i] = '$';
            break;
        }
    }
    return file_ptr;
}

void remove_comments(char *testcaseFile, char *cleanFile) // Function to clean the source code by removal of comments.
{
    FILE *fp_input = fopen(testcaseFile, "r");
    FILE *fp_output = fopen(cleanFile, "w");
    bool withinComment = false;
    int c, prev_c = '\0';

    while ((c = fgetc(fp_input)) != EOF)
    {
        if (c == '*' && prev_c == '*' && !withinComment)
        {
            withinComment = true;
        }
        else if (c == '*' && prev_c == '*' && withinComment)
        {
            withinComment = false;
            prev_c = '\0';
            continue;
        }

        if (!withinComment && prev_c != '\0')
        {
            fputc(prev_c, fp_output);
        }
        prev_c = c;
    }

    if (prev_c != '\0' && !withinComment)
    {
        fputc(prev_c, fp_output);
    }

    fclose(fp_output);
    fclose(fp_input);
}

tokenInfo *getNextToken() // getNextToken function used by parser to get the next token
{
    token = (tokenInfo *)malloc(sizeof(tokenInfo));
    if (file_ptr == NULL)
    {
        printf("FILE PTR NULL\n");
    }
    if (isEOF)
    {
        // printf("EOF CONDITION\n");
        return NULL;
    }
    while (!(tokenised) && !(isEOF))
    {

        if (text == NULL && !(isEOF))
        {
            text = (char *)malloc(bufferSize); // Character array to store the text
            file_ptr = getStream(file_ptr);
        }

        while (forward < bufferSize && (forward == 0 || (int)text[forward - 1] != 0))
        {

            char c = text[forward];

            switch (state) // DFA
            {
            case 1:
                if (c >= (int)'0' && c <= (int)'9')
                {
                    state = 2;
                }
                else if (c == (int)'(')
                {
                    state = 8;
                }
                else if (c == (int)')')
                {
                    state = 9;
                }
                else if (c == (int)'t')
                {
                    state = 15;
                }
                else if (c == (int)'f')
                {
                    state = 10;
                }
                else if ((c != (int)'t' && c != (int)'f' && c != (int)'A' && c != (int)'O') && ((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || c == (int)'_'))
                {
                    state = 19;
                }
                else if (c == (int)'A')
                {
                    state = 20;
                }
                else if (c == (int)'O')
                {
                    state = 23;
                }
                else if (c == (int)'+')
                {
                    state = 25;
                }
                else if (c == (int)'*')
                {
                    state = 26;
                }
                else if (c == (int)'-')
                {
                    state = 30;
                }
                else if (c == (int)'/')
                {
                    state = 31;
                }
                else if (c == (int)'!')
                {
                    state = 32;
                }
                else if (c == (int)'<')
                {
                    state = 34;
                }
                else if (c == (int)'>')
                {
                    state = 38;
                }
                else if (c == (int)';')
                {
                    state = 42;
                }
                else if (c == (int)'=')
                {
                    state = 43;
                }
                else if (c == (int)':')
                {
                    state = 45;
                }
                else if (c == (int)'.')
                {
                    state = 47;
                }
                else if (c == (int)'[')
                {
                    state = 49;
                }
                else if (c == (int)']')
                {
                    state = 51;
                }
                else if (c == (int)',')
                {
                    state = 50;
                }
                else if (c == (int)' ' || c == '\n' || c == '\t')
                {
                    state = 1;
                    if (c == '\n')
                        lNo++;
                    begin = forward + 1;
                }
                else if (c == (int)'$')
                {
                    isEOF = true;
                    token->lineNo = lNo;
                    token->tok = "DOLLAR";
                    strcpy(token->val.lexeme, "$");
                    tokenCntr++;
                    return token;
                }
                else
                {
                    if (text[forward] != 0)
                    {
                        error_function();
                    }
                }
                break;
            case 2:
                if (c >= (int)'0' && c <= (int)'9')
                {
                    state = 2;
                }
                else if (c == (int)'.')
                {
                    state = 3;
                }
                else
                {
                    tokenize(begin, forward, text, "NUM", "INT");

                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;
            case 3:
                if (c >= (int)'0' && c <= (int)'9')
                {
                    state = 4;
                }
                // Retraction
                else if (c == (int)'.')
                {
                    forward--;
                    tokenize(begin, forward, text, "NUM", "INT");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                else
                {
                    if (text[forward] != 0)
                    {
                        error_function();
                    }
                }
                break;
            case 4:
                if (c >= (int)'0' && c <= (int)'9')
                {
                    state = 4;
                }
                else if (c == (int)'E' || c == (int)'e')
                {
                    state = 5;
                }
                else
                {
                    tokenize(begin, forward, text, "RNUM", "REAL");
                    begin = forward;
                    forward--;
                    state = 1;
                }
                break;
            case 5:
                if (c >= (int)'0' && c <= (int)'9')
                {
                    state = 7;
                }
                else if (c == (int)'+' || c == (int)'-')
                {
                    state = 6;
                }
                // Retraction
                else if ((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || c == (int)'_')
                {
                    forward--;
                    tokenize(begin, forward, text, "RNUM", "REAL");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                else
                {
                    if (text[forward] != 0)
                    {
                        error_function();
                    }
                }
                break;
            case 6:
                if (c >= (int)'0' && c <= (int)'9')
                {
                    state = 7;
                }
                else
                {
                    if (text[forward] != 0)
                    {
                        error_function();
                    }
                }
                break;
            case 7:
                if (c >= (int)'0' && c <= (int)'9')
                {
                    state = 7;
                }
                else
                {
                    tokenize(begin, forward, text, "RNUM", "REAL");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;
            case 8:
                tokenize(begin, forward, text, "BO", "CHAR");

                begin = forward;
                state = 1;
                forward--;

                break;
            case 9:
                tokenize(begin, forward, text, "BC", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;
            case 10:
                if (c == (int)'a')
                {
                    state = 11;
                }
                else if (c != (int)'a' && ((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "ID", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;
            case 11:
                if (c == (int)'l')
                {
                    state = 12;
                }
                else if (c != (int)'l' && ((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "ID", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;
            case 12:
                if (c == (int)'s')
                {
                    state = 13;
                }
                else if (c != (int)'s' && ((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "ID", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;
            case 13:
                if (c == (int)'e')
                {
                    state = 14;
                }
                else if (c != (int)'e' && ((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "ID", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;
            case 14:
                if (((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "FALSE", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;
            case 15:
                if (c == (int)'r')
                {
                    state = 16;
                }
                else if (c != (int)'r' && ((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "ID", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;
            case 16:
                if (c == (int)'u')
                {
                    state = 17;
                }
                else if (c != (int)'u' && ((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "ID", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;
            case 17:
                if (c == (int)'e')
                {
                    state = 18;
                }
                else if (c != (int)'u' && ((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "ID", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;

            case 18:
                if (((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "TRUE", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;
            case 19:
                if (((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "ID", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;
            case 20:
                if (c == (int)'N')
                {
                    state = 21;
                }
                else if (c != (int)'N' && ((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 21;
                }
                else
                {
                    tokenize(begin, forward, text, "ID", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;
            case 21:
                if (c == (int)'D')
                {
                    state = 22;
                }
                else if (c != (int)'D' && ((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "ID", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;

            case 22:
                if (((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "AND", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;
            case 23:
                if (c == (int)'R')
                {
                    state = 24;
                }
                else if (c != (int)'R' && ((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "ID", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;

            case 24:
                if (((c >= (int)'a' && c <= (int)'z') || (c >= (int)'A' && c <= (int)'Z') || (c >= (int)'0' && c <= (int)'9') || c == (int)'_'))
                {
                    state = 19;
                }
                else
                {
                    tokenize(begin, forward, text, "OR", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;

            case 25:
                tokenize(begin, forward, text, "PLUS", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 26:
                if (c == (int)'*')
                {
                    state = 27;
                }
                else
                {
                    tokenize(begin, forward, text, "MUL", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                    state = 1;
                }
                break;

            case 27:
                if (c == (int)'*')
                {
                    state = 28;
                }
                else
                {
                    if (c == '\n')
                        lNo++;
                    state = 27;
                }
                break;

            case 28:
                if (c == (int)'*')
                {
                    state = 29;
                }
                else
                {
                    if (c == '\n')
                        lNo++;
                    state = 27;
                }
                break;

            case 29:
                begin = forward;
                forward--;
                state = 1;
                break;

            case 30:
                tokenize(begin, forward, text, "MINUS", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 31:
                tokenize(begin, forward, text, "DIV", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 32:
                if (c == (int)'=')
                {
                    state = 33;
                }
                else
                {
                    if (text[forward] != 0)
                    {
                        error_function();
                    }
                }
                break;

            case 33:
                tokenize(begin, forward, text, "NE", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 34:
                if (c == (int)'<')
                {
                    state = 36;
                }
                else if (c == (int)'=')
                {
                    state = 35;
                }
                else
                {
                    tokenize(begin, forward, text, "LT", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                    state = 1;
                }
                break;

            case 35:
                tokenize(begin, forward, text, "LE", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 36:
                if (c == (int)'<')
                {
                    state = 37;
                }
                else
                {
                    tokenize(begin, forward, text, "DEF", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                    state = 1;
                }
                break;

            case 37:
                tokenize(begin, forward, text, "DRIVERDEF", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 38:
                if (c == (int)'>')
                {
                    state = 40;
                }
                else if (c == (int)'=')
                {
                    state = 39;
                }
                else
                {
                    tokenize(begin, forward, text, "GT", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                    state = 1;
                }
                break;

            case 39:
                tokenize(begin, forward, text, "GE", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 40:
                if (c == (int)'>')
                {
                    state = 41;
                }
                else
                {
                    tokenize(begin, forward, text, "ENDDEF", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                    state = 1;
                }
                break;

            case 41:
                tokenize(begin, forward, text, "DRIVERENDDEF", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 42:
                tokenize(begin, forward, text, "SEMICOL", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 43:
                if (c == (int)'=')
                {
                    state = 44;
                }
                else
                {
                    if (text[forward] != 0)
                    {
                        error_function();
                    }
                }
                break;

            case 44:
                tokenize(begin, forward, text, "EQ", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 45:
                if (c == (int)'=')
                {
                    state = 46;
                }

                else
                {
                    tokenize(begin, forward, text, "COLON", "CHAR");
                    begin = forward;
                    state = 1;
                    forward--;
                }
                break;

            case 46:
                tokenize(begin, forward, text, "ASSIGNOP", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 47:
                if (c == (int)'.')
                {
                    state = 48;
                }
                else
                {
                    if (text[forward] != 0)
                    {
                        error_function();
                    }
                }
                break;

            case 48:
                tokenize(begin, forward, text, "RANGEOP", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 49:
                tokenize(begin, forward, text, "SQBO", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 50:
                tokenize(begin, forward, text, "COMMA", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;

            case 51:
                tokenize(begin, forward, text, "SQBC", "CHAR");
                begin = forward;
                state = 1;
                forward--;
                break;
            }
            forward++;
            if (tokenised)
            {
                tokenised = false;
                return token;
            }
        }

        if (!(forward < bufferSize && (forward == 0 || (int)text[forward - 1] != 0))) // Populating the Text_twin Buffer
        {
            if (state != 1 && state != 26 && state != 27 && state != 28 && state != 29)
            {
                // free(text_twin);
                text_twin = NULL;
                text_twin = (char *)malloc(sizeof(char) * (forward - begin + 1));
                for (int i = begin; i < forward; i++)
                    text_twin[i - begin] = text[i];
                text_twin[forward - begin] = '\0';
            }
            begin = 0;
            forward = 0;
            // free(text);
            text = NULL;
        }
    }
    return token;
}
