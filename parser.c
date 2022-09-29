/*
Author: Filip Kružík
*/
#include <stdlib.h>
#include "skener.h"
#include "parser.h"

int flag = 0;
int i = 0;
token tk;

int is_ptr_comp(){
    return (stack[i].type == EQUAL || stack[i].type == NE || stack[i].type == LT || stack[i].type == LTE || stack[i].type == GT || stack[i].type == GTE);
}

int look_for_kw(keyword lf, token_type until){
    //i++;
    while(stack[i].type != until){
        if (stack[i].keyword == lf){
            return 0;
        }else{
            if (check_syntax() != 0){
                return 2;
            }
        }
    }
    return 2;
}

int look_for_type(token_type lf, token_type until){
    //i++;
    while(stack[i].type != until){
        if (stack[i].type == lf){
            return 0;
        }else{
            check_syntax();
        }
        i++;
    }
    return 2;
}

int check_exp(){
    int first_pass = 0;
    while (stack[i].keyword!=THEN && stack[i].type != END_OF_FILE && stack[i].keyword != DO){
        first_pass = 1;
        switch(stack[i].type){
            case LBR:
                i++;
                if (look_for_type(RBR, THEN) == 0){
                    continue;
                }
                return 2;
            case INT: case DBL: case STR: case ID:
                i++;
                if (is_ptr_comp()){
                    i++;
                    if (stack[i].type == INT || stack[i].type == DBL || stack[i].type == STR || stack[i].type == ID){
                        i++;
                        break;
                    }
                }else if (stack[i].keyword == THEN || stack[i].keyword == DO){
                    return 0;
                }
                return 2;
            default:
                return 2;
        }
    }
    if ((stack[i].keyword == THEN || stack[i].keyword == DO) && first_pass == 1){
        //i++;
        return 0;
    }
    return 2;
}

int check_syntax(){
    while((tk = stack[i]).type != END_OF_FILE){
        if (flag == 1){
            return 2;
        }
        /*if (tk.keyword == lf){
            return 0;
        }*/
        switch(tk.type){
        case KEYWORD:
            switch (tk.keyword)
            {
            case IF:
                i++;
                if (check_exp() == 0){
                    //i++;
                    if (look_for_kw(THEN, END_OF_FILE) == 0){
                        i++;
                        if (look_for_kw(ELSE, END_OF_FILE) == 0){
                            i++;
                            if (look_for_kw(END, END_OF_FILE) == 0){
                                i++;
                                return 0;
                            }
                    }
                    }
                }
                return 2;
            case WHILE:
                i++;
                if (check_exp() == 0){
                    if (look_for_kw(DO, END_OF_FILE) == 0){
                        i++;
                        if (look_for_kw(END, END_OF_FILE) == 0){
                            i++;
                            return 0;
                        }
                    }
                }
                return 2;
            default:
                return 2;
            }
            break;
        case ID:
            i++;
            if(stack[i].type == ASSIGN){
                i++;
                if (stack[i].type == INT || stack[i].type == DBL || stack[i].type == STR || stack[i].type == ID){
                    i++;
                    return 0;
                }
            }
            return 2;
        default:
            return 2;
        }
    }
    if (flag != 0){
        return 2;
    }
    return 0;
}