/*
Author: Pavlína Auerová
*/
#include <stdlib.h>
#include <stdio.h>
#include "skener.h"
#include "error.h"

int garbCol_size;
int garbCol_tksize;
int garbCol_index;
int garbCol_tkindex;
void ** garbCol_ptr;
token ** garbCol_tkptr;

void garbCol_init() {
    garbCol_size = 2048;
    garbCol_tksize = 2048;
    garbCol_index = 0;
    garbCol_tkindex = 0;
    garbCol_ptr = malloc(sizeof(void*) * garbCol_size);
    garbCol_tkptr = malloc(sizeof(token*) * garbCol_tksize);
    if(garbCol_ptr == NULL || garbCol_tkptr == NULL){
        erro(INTERN_ERR);
    }
}

void garbCol_add(void *ptr) {
    if(garbCol_index >= garbCol_size){
        garbCol_size += 1024;
        garbCol_ptr = realloc(garbCol_ptr,sizeof(void*) * garbCol_size);
    }
    garbCol_ptr[garbCol_index] = ptr;
    garbCol_index++;
}

void garbCol_addtoken(token *ptr){
    if(garbCol_tkindex >= garbCol_tksize){
        garbCol_tksize +=1024;
        garbCol_tkptr = realloc(garbCol_tkptr,sizeof(token*) * garbCol_tksize);
    }
    garbCol_tkptr[garbCol_tkindex] = ptr;
    garbCol_tkindex++;
}

void garbCol_update(void *old, void *new) {
    for(int i = 0; i < garbCol_index; i++){
        if(garbCol_ptr[i] == old){
            garbCol_ptr[i] = new;
            break;
        }
    }
}

void garbCol_tkupdate(token *old, token *new){
    for(int i = 0; i < garbCol_tkindex; i++){
        if(garbCol_tkptr[i] == old){
            garbCol_tkptr[i] = new;
            break;
        }
    }
}

void garbCol_free() {
    for(int i = 0; i < garbCol_tkindex; i++){
        free(garbCol_tkptr[i]->string_value);
        free(garbCol_tkptr[i]);
    }
    for(int i = 0; i < garbCol_index; i++){
        free(garbCol_ptr[i]);
    }
    if(garbCol_ptr != NULL){
        free(garbCol_ptr);
    }
    if(garbCol_tkptr != NULL){
        free(garbCol_tkptr);
    }
    
}