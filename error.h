/*
Author: Pavlína Auerová
*/
#ifndef ERROR_H
#define ERROR_H


typedef enum{
    SKEN_ERR = 1,
    PARS_ERR = 2,
    INTERN_ERR = 99
} errore;

void erro(errore e);

#endif