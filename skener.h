/*
Author: Adam Kulla
*/
#ifndef SKENER_H
#include <stdio.h>

int id_size = sizeof(char*);

typedef enum {
    KEYWORD,
    ID,
    STR,
    COMMA,
    COLON,
    RBR,
    LBR,
    NE,
    MUL,
    SUB,
    CMT,
    DIV,
    DIV2,
    ADD,
    LT,
    LTE,
    GT,
    GTE,
    ASSIGN,
    EQUAL,
    CNCT,
    INT,
    DBL,
    EXP,
    END_OF_FILE,
    SPACE,
    NONE
} token_type;

typedef enum {
    DO,
    ELSE,
    END,
    FUNCTION,
    GLOBAL,
    IF,
    INTEGER,
    LOCAL,
    NIL,
    NUMBER,
    REQUIRE,
    RETURN,
    STRING,
    THEN,
    WHILE,
    NAK
} keyword;

typedef union{
    int i;
    double d;
} num_val;

typedef struct{
    token_type type;
    keyword keyword;
    char* string_value;
    num_val number_value;
}token;

typedef struct{
    unsigned i;
    token data[100];
}token_stack;

typedef struct{
    FILE* source;
}scanner;

char keywords[][10] = {
    "do",
    "else",
    "end",
    "function",
    "global",
    "if",
    "integer",
    "local",
    "nil",
    "number",
    "require",
    "return",
    "string",
    "then",
    "while",
};

token read_next(scanner *scan);

token *stack;

#define SKENER_H

#endif