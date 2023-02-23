#ifndef EnvVarLL
#define EnvVarLL
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//create the node
struct EnvVar{
    char *name;
   	char *value;
    struct EnvVar* next;
};
typedef struct EnvVar EnvVar;
//creates the linked list
typedef struct{
    EnvVar* head;
    EnvVar* tail;
} Variablelist;
//creates the linked list
Variablelist* createVariablelist(){
    Variablelist* ret = malloc(sizeof(Variablelist));
    ret->head = NULL;
    ret->tail = NULL;
    return ret;
}
//adds the varaible to the list
void add_Variablelist(Variablelist* list, char* name, char* value){
    EnvVar* newNode = malloc(sizeof(EnvVar));
    newNode->name = malloc(sizeof(char) * (strlen(name) + 1));
    newNode->value = malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(newNode->name, name);
    strcpy(newNode->value, value);
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
//deletes the linked list and frees the memory
void del_Variablelist(Variablelist* list){
    EnvVar* node = list->head;
    while(list->head !=NULL){
        node = list->head;
        list->head = list->head->next;
        free(node->name);
        free(node->value);
        free(node);
    }
    free(list);
}

#endif