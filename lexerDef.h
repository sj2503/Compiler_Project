/*  Group No : 31
    Anushka Jain 2019B1A70904P
    Ritu Chandna 2019B4A70667P
    Payal Basrani 2019B5A70809P
    Ujjwal Jain 2019B4A70647P
    Shobhit Jain 2019B3A70385P
*/

#ifndef LEXERDEF_H
#define LEXERDEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define KEYWORDSIZE 26 // Size of Keywords Set

typedef struct hashItem // Hash Table Element
{
    char *key;
    char *value;
}hashItem;

typedef union variant // Union for value field of the Token
{
    char lexeme[21];
    int valIfInt;
    float valIfFloat;
} variant;

typedef struct tokenInfo // Token Structure
{
    char *tok;
    unsigned int lineNo;
    variant val;
} tokenInfo;

typedef struct Error // Error Structure
{
    char *error_name;
    int line_no;
} Error;

#endif