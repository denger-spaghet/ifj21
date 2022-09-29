/*
Author: Adam Kulla
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "skener.c"
#include "parser.c"


int main(int argc, char *argv[]) {
    scanner scan;

    FILE *f = fopen(argv[1], "r");
    if(f == NULL){
        fprintf(stderr,"File cannot be opened");
    }
    scan.source = f;
    token tk;
    int stack_size = sizeof(token);
    int cnt = 0;
    int flag = 0;
    while (flag == 0){
        tk = read_next(&scan);
        stack_size = stack_size + sizeof(tk);
        stack = realloc(stack, stack_size);
        if (stack == NULL){
            erro(INTERN_ERR);
        }
        stack[cnt] = tk;
        cnt++;
        if (tk.type == END_OF_FILE){
            flag = 1;
        }
    }
    stack[cnt] = tk;



    int syntax = check_syntax();
    return syntax;

}
