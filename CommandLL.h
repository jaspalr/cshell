#ifndef CommandLL
#define CommandLL

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
//node with the command, time and return
struct Command{
    char* name;
    time_t time;
    char* ret;
    struct Command* next;
};
typedef struct Command Command;
//linked list structure
typedef struct{
    Command* head;
    Command* tail;
} Commandlist;

//creates the linked list
Commandlist* createCommandlist(){
    Commandlist* ret = malloc(sizeof(Commandlist));
    ret->head = NULL;
    ret->tail = NULL;
    return ret;
}
//adds the command to list, and genrates the time
void add_Commandlist(Commandlist* list, char* name, char* ret){
    Command* newNode = malloc(sizeof(Command));
    newNode->name = malloc(sizeof(char) * (strlen(name) + 1));
    newNode->ret = malloc(sizeof(char) * (strlen(ret) + 1));
    strcpy(newNode->name, name);
    strcpy(newNode->ret, ret);
    newNode->time = time(NULL);
    newNode->next = NULL;
    if(list->head == NULL){
        list->head = newNode;
        list->tail = newNode;
    }
    else{
    list->tail->next = newNode;
    list->tail = newNode;
    }
}
//deletes the list and frees the memory 
void del_Commandlist(Commandlist* list){
    Command* node = list->head;
    while(list->head !=NULL){
        node = list->head;
        list->head = list->head->next;
        free(node->name);
        free(node->ret);
        free(node);
    }
    free(list);
}

#endif