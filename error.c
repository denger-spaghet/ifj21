/*
Author: Pavlína Auerová
*/
#include "garbcol.c"
#include "error.h"


void erro(errore e){
    garbCol_free();
    exit(e);
}