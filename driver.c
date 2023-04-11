/*  Group No : 31
    Anushka Jain 2019B1A70904P
    Ritu Chandna 2019B4A70667P
    Payal Basrani 2019B5A70809P
    Ujjwal Jain 2019B4A70647P
    Shobhit Jain 2019B3A70385P
*/

#include "parser.c"
#include <time.h>
// void lexerInitialization()
// {
//     file_ptr = fopen("t6.txt", "r");
//     fillKeywordTable();
//     if (file_ptr == NULL)
//     {
//         printf("Error opening the file\n");
//     }
// }

void global_vars()
{
    state = 1;              // State variable of DFA
    lNo = 1;                // Line Number Variable
    tokenCntr = 0;          // Token Counter for number of tokens
    text = NULL;            // Text buffer for storing source code
    text_twin = NULL;       // Text Twin buffer for storing leftover Text buffer contents
    begin = 0, forward = 0; // Begin and Forward Variables
    bufferSize = 512;

    errorcntr = 0;      // Error counter
    isLexError = false; // Flag for Lexical Error

    isEOF = false;

    parseTable[COUNT_NT][COUNT_T - 1]; // removing epsilon from count of terminals
    first[COUNT_NT];
    follow[COUNT_NT];
    gramArr[NRULES];
    parseTable2[COUNT_NT][COUNT_T - 1][30];
    firstset[COUNT_NT][150];
    followset[COUNT_NT][150];
    isError = false;

    fileParseTree = NULL;
}

treeNode* Parsing()
{
    treeNode* parseRoot= stackImp();
    return parseRoot;
}

void PrintTokens()
{
    // printf("Lexer");
    tokenInfo *token = (tokenInfo *)malloc(sizeof(tokenInfo));
    token = getNextToken();
    for (int i = 0; i < tokenCntr; i++)
    {

        if (token != NULL)
        {
            if (strcmp((token->tok), "DOLLAR") == 0)
                break;

            printf("Line %d : \t", token->lineNo);
            if (strcmp((token->tok), "NUM") == 0)
            {
                printf("Lexeme : %d\t", token->val.valIfInt);
            }
            else if (strcmp(token->tok, "RNUM") == 0)
            {
                printf("Lexeme : %f\t", token->val.valIfFloat);
            }
            else
            {
                printf("Lexeme : %s\t", token->val.lexeme);
            }
            printf("Token : %s\n", token->tok);
            printf("\n");
        }
        else
        {

            break;
        }
        token = getNextToken();
    }
}
int main(int argc, char *argv[]) // arguments
{
    fillKeywordTable();
    bufferSize = atoi(argv[3]);
    // bufferSize = argv[3];
    file_ptr = fopen(argv[1], "r");
    // file_ptr = fopen(argv[1], "r");
    if (file_ptr == NULL)
    {
        printf("File Opening Error\n");
    }
    fileParseTree = fopen(argv[2], "w");
    // fileParseTree = fopen("traverseFile.txt", "w");

    // lexerInitialization();
    printf("\n\n1. FIRST and FOLLOW set is AUTOMATED \n2. Both lexical and syntax analysis modules implemented \n3. Modules work with all testcases\n4. Parse Tree constructed\n");
    int input;
    do
    {
        global_vars(); // Initialization of the global variables

        printf("\n\n###### Enter a number corresponding to the function you want to execute ###### \n\n");
        printf("0: EXIT\n");
        printf("1: Removal of comments and print the comment free code\n");
        printf("2: Run lexer, Generate and Print token list\n");
        printf("3: Run Parser, Generate and print Parse Tree and verify syntactic correctness\n");
        printf("4: Total time taken by stage 1 code\n\n");
        printf("Enter Your Choice : ");
        scanf("%d", &input);
        printf("\n\n\n");
        switch (input)
        {
        case 0:
        {
            printf("\nSuccessfully EXITED\n");
            break;
        }
        case 1:
        {
            remove_comments(argv[1], "commentfree.txt");
            printf("A new file is created with name 'commentfree.txt' having the source code without any comments\n");
            break;
        }
        case 2:
        {
            file_ptr = fopen(argv[1], "r");
            fseek(file_ptr, 0, SEEK_SET);
            printf("TOKENS : \n\n\n");
            PrintTokens();
            fclose(file_ptr);
            break;
        }
        case 3:
        {
            file_ptr = fopen(argv[1], "r");

            fileParseTree = fopen(argv[2], "w");
            // fileParseTree = fopen("traversalFile.txt", "w");

            printf("PARSING...\n\n\n");
            Parsing();
            fclose(file_ptr);
            fclose(fileParseTree);
            break;
        }
        case 4:
        {
            file_ptr = fopen(argv[1], "r");

            fileParseTree = fopen(argv[2], "w");
            clock_t start_time, end_time;
            double total_CPU_time, total_CPU_time_in_seconds;
            start_time = clock();
            Parsing();
            end_time = clock();
            total_CPU_time = (double)(end_time - start_time);
            total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
            printf("Total CPU time taken by lexer and parser to verify syntactic correctness is : \n%lf Ticks\n", total_CPU_time);
            printf("Total CPU time per second taken by lexer and parser to verify syntactic correctness : \n%lf sec\n", total_CPU_time_in_seconds);
            fclose(file_ptr);
            fclose(fileParseTree);
            break;
        }
        default:
            printf("\nResponse %d is not valid, please enter the appropriate input\n", input);
        }
        // fclose(file_ptr);
    } while (input != 0);
    return 0;
}