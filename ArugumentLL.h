#ifndef ArugmnetsLL
#define ArugmentsLL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//creates a node for each argument seperate by a whitespce
struct arguments{
    char* arg;
    struct arguments* next;
};
typedef struct arguments arguments;
//linked list with these nodes
typedef struct{
    arguments* head;
    arguments* tail;
    int size;
} arglist;
//creates the linkedlist
arglist* createarglist(){
    arglist* ret = malloc(sizeof(arglist));
    ret->head = NULL;
    ret->tail = NULL;
    ret->size = 0;
    return ret;
}
//add a argument to the list
void add_arglist(arglist* list, char* arg){
    arguments* newNode = malloc(sizeof(arguments));
    newNode->arg = malloc(sizeof(char) * (strlen(arg) + 1));
    strcpy(newNode->arg, arg);
    newNode->next = NULL;
    if(list->head == NULL){
        list->head = newNode;
        list->tail = newNode;
    }
    else{
    list->tail->next = newNode;
    list->tail = newNode;
    }
    list->size++;
}
//deletes the list and frees the memory
void del_arglist(arglist* list){
    arguments* node = list->head;
    while(list->head !=NULL){
        node = list->head;
        list->head = list->head->next;
        free(node->arg);
        free(node);
    }
    free(list);
}

#endif
