/*
Author: Adam Kulla
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "skener.h"
#include "error.c"


bool is_char(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_num(char c) {
    return (c >= '0' && c <= '9');
}

void push_token(token_stack *tks, token token){
    tks->data[tks->i] = token;
    tks->i++;
}

int is_tks_empty(token_stack *tks){
    return tks->i==0;
}

token pop_token(token_stack *tks){
    if (!is_tks_empty(tks)){
        tks->i--;
    }
    return tks->data[tks->i];
}


void set_stack (token_stack *stack, scanner *scan){
    token tok = read_next(scan);
    stack->i = 0;
    while(tok.type != END_OF_FILE){
        push_token(stack, tok);
        tok = read_next(scan);
    }
    stack->i--;
}

keyword handle_keyword(char *id){
    if (!strcmp(id, keywords[0])) {
        return DO;
    } else if (!strcmp(id, keywords[1])) {
        return ELSE;
    } else if (!strcmp(id, keywords[2])) {
        return END;
    } else if (!strcmp(id, keywords[3])) {
        return FUNCTION;
    } else if (!strcmp(id, keywords[4])) {
        return GLOBAL;
    } else if (!strcmp(id, keywords[5])) {
        return IF;
    } else if (!strcmp(id, keywords[6])) {
        return INTEGER;
    } else if (!strcmp(id, keywords[7])) {
        return LOCAL;
    } else if (!strcmp(id, keywords[8])) {
        return NIL;
    } else if (!strcmp(id, keywords[9])) {
        return NUMBER;
    } else if (!strcmp(id, keywords[10])) {
        return REQUIRE;
    } else if (!strcmp(id, keywords[11])) {
        return RETURN;
    } else if (!strcmp(id, keywords[12])) {
        return STRING;
    } else if (!strcmp(id, keywords[13])) {
        return THEN;
    } else if (!strcmp(id, keywords[14])) {
        return WHILE;
    } else {
        return NAK;
    }
}

char* get_string_literal(scanner *scan, token *token){
    char* strLiteral = (char*)calloc(256, sizeof(char*));
    int i = 0;
    
    if (strLiteral == NULL){
        erro(INTERN_ERR);
    }
    garbCol_add(strLiteral);

    token->string_value = strLiteral;
    token->type = STR;

    while(true){
        char c = (char)getc(scan->source);
        if (c == '\"'){
            return strLiteral;
        }
        if (c == '\\'){
            c = (char)getc(scan->source);
            switch(c){
                case '"':
                    strLiteral[i] = '\"';
                    i++;
                    break;
                case 'n':
                    strLiteral[i] = '\n';
                    i++;
                    break;
                case 't':
                    strLiteral[i] = '\t';
                    i++;
                    break;
                case '\\':
                    strLiteral[i] = '\\';
                    i++;
                    break;
                default:
                    if(is_num(c)){
                        ungetc(c, scan->source);
                        char *code = malloc(sizeof(char));
                        if (code == NULL){
                            erro(INTERN_ERR);
                        }
                        int cnt = 0;
                        while (cnt < 3){
                            code[cnt] = getc(scan->source);
                            cnt++;
                        }
                        strLiteral[i] = (char)atoi(code);
                        i++;
                    }else{
                        erro(SKEN_ERR);
                    }
                    break;
            }
        }else{
            strLiteral[i] = c;
            i++;
        }
    }
}

void handle_id (char c, scanner *scan, token *token){
    char *id = malloc(sizeof(char*)*4);
    
    int cnt = 0;
    if (id == NULL){
        erro(INTERN_ERR);
    }
    
    if (c != '_'){
        id_size = id_size + sizeof(c)*4;
        id = realloc(id, id_size);
        id [0] = c;
        cnt = 1;
    } 
    c = (char) getc(scan->source);
    while(is_char(c) || is_num(c)){
        id_size = id_size + sizeof(c)*4;
        id = realloc(id, id_size);
        id [cnt] = c;
        cnt ++;
        c = (char) getc(scan->source);
    }
    garbCol_add(id);

    token->keyword = handle_keyword(id);
    if (token->keyword == NAK){
        token->type = ID;
        token->string_value = id;
    } else {
        token->type = KEYWORD;
    }
    ungetc(c, scan->source);

}

void handle_num (scanner *scan, token *token){
    char *num = malloc(sizeof(char));
    char *dec = malloc(sizeof(char));
    char *exp = malloc(sizeof(char));
    
    if (num == NULL || dec == NULL || exp == NULL){
        erro(INTERN_ERR);
    } 

    char c;
    int cnt = 0;
    while (is_num(c = (char) getc(scan->source))){
        num [cnt] = c;
        cnt++;
    }
    if(c != '.' && !is_num(c) && c != 'e' && c != 'E'){
        token->type = INT;
        token->number_value.i = atoi(num);
    } else if (c == '.') {
        token->number_value.d = atoi(num);
        cnt = 0;
        while (is_num(c = (char) getc(scan->source))){
            dec [cnt] = c;
            cnt++;
        }
        token->type = DBL;
        token->number_value.d = token->number_value.d + (atoi(dec) * pow(0.1, cnt));
    }
    if (c != 'e' && c != 'E'){
        ungetc(c, scan->source);
    } else {
        if (!dec[0]){
            token->number_value.d = atoi(num);
            token->type = DBL;
        }
        c = (char)getc(scan->source);
        cnt = 0;
        if (c == '-'){ 
            token->number_value.d = 1 / token->number_value.d;
        } else if (is_num(c)){
            exp [cnt] = c;
            cnt++;
        }
        while (is_num(c = (char) getc(scan->source))){
            exp [cnt] = c;
            cnt++;
        }
        ungetc(c, scan->source);
        token->number_value.d = token->number_value.d * pow(10, atoi(exp));
    }
    free(num);
    free(dec);
    free(exp);
    return;
}

void comparison(char c, scanner *scan, token *token){
    char next = (char)getc(scan->source);
    switch(c){
        case '=':
            if (next == '='){
                token->type = EQUAL;
                break;
            } else {
                ungetc(next, scan->source);
                token->type = ASSIGN;
                break;
            }
        case '>':
            if (next == '='){
                token->type = GTE;
                break;
            } else {
                ungetc(next, scan->source);
                token->type = GT;
                break;
            }
        case '<':
            if (next == '='){
                token->type = LTE;
                break;
            } else {
                ungetc(next, scan->source);
                token->type = LT;
                break;
            }
    }
}

void handle_comment(scanner *scan){
    char c = (char) getc(scan->source);
    char f;
    char bruh;
    switch(c){
        case '\n':
            return;
        case '[':
            f = (char) getc(scan->source);
            switch(f){
                case '\n':
                    return;
                case '[':
                    while ((bruh = (char)getc(scan->source)) != ']');
                    if (bruh == ']'){
                        while ((bruh = (char)getc(scan->source)) != ']');
                        return;
                    }
                default:
                    while ((c = (char)getc(scan->source)) != '\n');
                    return;
            }
            return;
        default:
            while ((c = (char)getc(scan->source)) != '\n');
            return;
    }

}

token read_next(scanner *scan){
    token token;
    token.keyword = NAK;
    token.type = NONE;
    token.number_value.d = 0;
    token.number_value.i = 0;
    token.string_value = 0;
    char n;
    while(1){
        char c = (char)getc(scan->source);
        switch(c){
            case EOF:
                token.type = END_OF_FILE;
                return token;
            case ' ': case '\n':
                break;
            case '*':
                token.type = MUL;
                return token;
            case '(':
                token.type = LBR;
                return token;
            case ')':
                token.type = RBR;
                return token;
            case ':':
                token.type = COLON;
                return token;
            case ',':
                token.type = COMMA;
                return token;
            case '+':
                token.type = ADD;
                return token;
            case '/':
                n = (char)getc(scan->source);
                if (n == '/'){
                    token.type = DIV2;
                    return token;
                } else {
                    ungetc(n, scan->source);
                    token.type = DIV;
                    return token;
                }
            case '-':
                c = (char)getc(scan->source);
                if (c != '-'){
                    ungetc(c, scan->source);
                    token.type = SUB;
                    return token;
                } else {
                    handle_comment(scan);
                    token.type = CMT;
                    return token;
                }
            case '=': case '>': case '<':
                comparison(c, scan, &token);
                return token;
            case '~':
                n = (char)getc(scan->source);
                if (n == '='){
                    token.type = NE;
                    return token;
                } else {
                    erro(SKEN_ERR);
                }
            case '"':
                get_string_literal(scan, &token);
                return token;
            default:
                if (is_char(c) || c == '_'){
                    handle_id(c, scan, &token);
                    return token;
                } else if (is_num(c)) {
                    ungetc(c, scan->source);
                    handle_num(scan, &token);
                    return token;
                } else {
                    return token;
                }

        }   

    }
}


