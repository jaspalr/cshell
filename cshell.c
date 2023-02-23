#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "CommandLL.h"
#include "ArugumentLL.h"
#include "EnvVarLL.h"
#include<unistd.h>
#include<sys/wait.h> 
#include <fcntl.h>
#include<errno.h>
//pharse the arugments entered into the shell
void pharseargs(char* input,arglist* list){
    int i = 0;
    char token[512] = "\0";
    int tokenlen = 0;
    while(input[i] != '\0'){
        if(input[i] != ' ' && input[i] != '\n' && input[i] != '\r' && input[i] != '\t' ){ //check for a whitespace
            token[tokenlen] = input[i];
            tokenlen ++;
        }
        else{
            token[tokenlen] = '\0'; 
            if(tokenlen != 0){
                add_arglist(list,token);//adds the n-1 argument 
            }
            tokenlen = 0;
            token[0] = '\0';
        }
        i++;
    }
    if(tokenlen != 0){ //
        add_arglist(list,token); //adds the nth argument
    }
}
//searchs for the variable and returns it or return null if not found
EnvVar* searchvar(Variablelist* Variables, char* str){
    EnvVar* node = Variables->head;
    while(node != NULL){
        if (strcmp(node->name,str) == 0){
            return node;
        }
        node = node->next;
    }
    return NULL;

}

//deals with the built in commands 0 -> unsuccess, 1->success, 2 exit case
int Commandbluitin(arglist* list, Commandlist* Commands, Variablelist* Variables){
    if(strcmp(list->head->arg, "exit") == 0){ //exit case
        if(list->size == 1){
            printf("Bye!\n");
            return 2;
        }
        else{
            printf("Error: Too many Arguments detected\n");
            add_Commandlist(Commands, "exit", "-1");
            return 1;
        }
    }
    else if(strcmp(list->head->arg, "log") == 0){ //log case
        if(list->size == 1){
            Command* coms = Commands->head;
            while(coms != NULL){
                struct tm* tim = localtime(&coms->time);
                printf("%s",asctime(tim));
                printf(" %s %s\n", coms->name, coms->ret);
                coms = coms->next;
            }
            add_Commandlist(Commands, "log", "0");
            return 1;
        }
        else{
            add_Commandlist(Commands, "log", "-1");
            return 1;
        }
    }
    else if(strcmp(list->head->arg,"print") == 0){ //print case //fix spaces
        arguments* arg = list->head->next;
        char printstatement[512] = "\0";
        int flag = 0;
        if (list->size == 1){
            printf("Error: Too few Arguments detected\n");
            add_Commandlist(Commands, "print", "-1");
            return 1;
        }
        while(arg != NULL && flag == 0){
            if (list->size == 1){
                printf("Error: few many Arguments detected\n");
                add_Commandlist(Commands, "print", "-1");
            }
            if(arg->arg[0]== '$'){//looks for a variable
                EnvVar* var = searchvar(Variables, arg->arg);
                if(var != NULL){//variable is found
                strcat(printstatement,var->value);
                strcat(printstatement, " ");
                }
                else{//varaible is not found
                    flag = 1; // if varaible is not set flag = 1 to exit
                    printf("Error: No Environment Variable %s found\n", arg->arg);
                    add_Commandlist(Commands, "print", "-1");
                    return 1;
                }
            }
            else{//no varaible
                strcat(printstatement,arg->arg);
                strcat(printstatement, " ");
            }
            arg = arg->next;
        }
        if(flag == 0){//variable and is found case
            printf("%s\n", printstatement);
            add_Commandlist(Commands, "print", "0");
        }
        return 1;
    }
    else if(strcmp(list->head->arg, "theme") == 0){//theme case
        if(list->size > 2){//too many arugments case
            printf("Error: Too many Arguments detected\n");
            add_Commandlist(Commands, "theme", "-1");

        }
        else if(list->size == 1){ //too few arguments entered
            printf("Error: Too few Arguments detected\n");
            add_Commandlist(Commands, "theme", "-1");
        }
        else{
            if(strcmp(list->head->next->arg, "red") == 0){
                printf("\033[0;31m");
                add_Commandlist(Commands, "theme", "0");
            }
            else if(strcmp(list->head->next->arg, "green") == 0){
                printf("\033[0;32m");
                add_Commandlist(Commands, "theme", "0");
            }
            else if(strcmp(list->head->next->arg, "blue") == 0){
                printf("\033[0;34m");
                add_Commandlist(Commands, "theme", "0");
            }
            else{
                printf("unsupported theme\n");
                add_Commandlist(Commands, "theme", "-1");
            }
            return 1;
        }
    }
    return 0;//if the command is not found
}
//if a variable needs to be saves
int storevar(char* input, Variablelist* vars, arglist* args,Commandlist* Commands){
    if(input[0] == '$'){
        char str[512];
        int i = 1; 
        str[0] = '$';
        while(input[i] != '=' && input[i] != ' ' && input[i] != '\0' && input[i] != '$'){//finds the = or whitespace
            str[i] = input[i];
            i++;
        }
        if(input[i] == '=' && input[i] != '$' && input[i]!= ' ' && input[i] != '\0' && (input[i+1] == '"' || args->size == 1)){//finds the value
            str[i] = '\0';
            char* varname = malloc((strlen(str) + 1) * sizeof(char));
            strcpy(varname,str);
            if(input[i+1] == '"' && input[i+1] != '\n' && input[i+1] != '\0'){
                arguments* node = args->head;
                while(node->next != NULL){
                    node = node->next;
                }
                if(node->arg[strlen(node->arg) - 1] == '"'){//if the last value is a "
                    i = i +2;
                    int x = 1;
                    str[0] = '"';
                    while(input[i] != '"'){//copy the sting
                        str[x] = input[i];
                        x++;
                        i++;
                    }
                    str[x] = '"';
                    str[x+1] = '\0';
                }
                else{
                    printf("No name and/or value found for setting up Enviroment Variables.\n");//invaild value
                    free(varname);
                    str[512] = '\0';
                    int i = 0;
                    while (input[i] != '\0' &&  input[i] != '\n' && input[i] != '\r'){
                        str[i] = input[i];
                        i++;
                    }
                    str[i] = '\0';
                    add_Commandlist(Commands, str, "-1");
                    return 1;

                }
            }

            else{ //normal case
                i = i +1;
                int x = 0;
                int flag = i;
               
                while(input[i] != ' ' && input[i] != '\n' && input[i] != '$' && input[i] != '=' && input[i] != '\r' && input[i] != '\t' && input[i] !='\0' ){
                    str[x] = input[i];
                    x++;
                    i++;
                }
                str[x] = '\0';
                if(input[i] == ' ' || input[i] == '$' || input[i] == '=' ||input[i] == '\t' || flag == i ){
                    printf("No name and/or value found for setting up Enviroment Variables.\n");
                    str[512] = '\0';
                    int i = 0;
                    while (input[i] != '\0' &&  input[i] != '\n' && input[i] != '\r'){
                        str[i] = input[i];
                        i++;
                    }
                    str[i] = '\0';
                    add_Commandlist(Commands, str, "-1");
                    free(varname);
                    return 1;
                }
            }

            EnvVar* found = searchvar(vars,varname);
            if(found !=NULL){ //if variable is not found
                free(found->value);
                char* value = malloc((strlen(str) + 1) * sizeof(char));
                strcpy(value,str);
                found->value = value;
                str[512] = '\0';
                int i = 0;
                while (input[i] != '\0' &&  input[i] != '\n' && input[i] != '\r'){
                    str[i] = input[i];
                    i++;
                }
                str[i] = '\0';
                add_Commandlist(Commands, str, "0");
            }
            else{//variable is found
                add_Variablelist(vars,varname,str);
                    str[512] = '\0';
                    int i = 0;
                    while (input[i] != '\0' &&  input[i] != '\n' && input[i] != '\r'){
                        str[i] = input[i];
                        i++;
                    }
                    str[i] = '\0';
                add_Commandlist(Commands, str, "0");
            }
            free(varname);
            return 1;
        }
        else{ //error in the vaule
            printf("No name and/or value found for setting up Enviroment Variables.\n");
                str[512] = '\0';
                int i = 0;
                while (input[i] != '\0' &&  input[i] != '\n' && input[i] != '\r'){
                    str[i] = input[i];
                    i++;
                }
                str[i] = '\0';
            add_Commandlist(Commands, str, "-1");
            return 1;
        }
    }
    return 0; // not the correct value
}

void execute(arglist* list, Commandlist* Commands, Variablelist* Variables){
    int pid = fork(); //creates the child proccess
    if(pid == 0){//child case
        int file = open("myoutput.txt", O_WRONLY| O_CREAT, 0777);
        dup2(file,1);
        close(file);
        if(list->size == 1){ // if 1 argument is passed

            execlp(list->head->arg,list->head->arg,NULL);
        }
        else{ // multiple arguments
            char** arr = malloc((list->size +1)*sizeof(char*));//list of the arugments passed
            int pos = 0; //pos in the array
            arguments* arg = list->head;
            bool cont = true;
            while(arg != NULL && cont){
                if(arg->arg[0] == '$'){//if there is varaible
                    EnvVar* var = searchvar(Variables,arg->arg);
                    if(var == NULL){ // if variable is not found, leave and report 2 back to the parent
                        printf("Error: No Environment Variable %s found\n", arg->arg);
                        cont = false;
                        free(arr);
                        exit(0);
                    }
                    else{//if variable found
                        arr[pos] = var->value;
                        pos ++;
                    }
                }
                else{//if not a varible
                    arr[pos] = arg->arg;
                    pos++;
                }
                arg = arg->next;
            }
            arr[pos] = NULL;
            execvp(list->head->arg,arr);//excute the commande
            free(arr);

        }
        del_arglist(list);
        del_Commandlist(Commands);
        del_Variablelist(Variables);
        exit(0);
    }
    else{
        wait(NULL); //wait for the parent
        FILE *f = fopen("myoutput.txt", "r");
        char line[512] = "\0";
        
        if(fgets(line,512,f) == NULL){//if the file is blank
            printf("Missing keyword or command, or permission problem\n");
            add_Commandlist(Commands,list->head->arg,"-1");
        }
        else{
         
            printf("%s", line);
            if(line[0] == 'E' && line[1] == 'r' && line[2] == 'r' && line[7] == 'N'){//if no varaible found
                    add_Commandlist(Commands,list->head->arg,"-1");
            }
            else{
                add_Commandlist(Commands,list->head->arg,"0");
                while(fgets(line,512,f) != NULL){
                        printf("%s", line);
                    }
                }
        }
        fclose(f);
        remove("myoutput.txt");
    }
}




int main(int argc, char *argv[]){
    Commandlist* Comms = createCommandlist();
    Variablelist* Vars = createVariablelist();
    if(argc == 1){//termanal commandes
        int flag = -1;
        char input[512] = "\0";
        while (flag != 2){
            printf("cshell$ ");
            fgets(input,512, stdin);
            arglist* args = createarglist();
            pharseargs(input,args);
            flag = Commandbluitin(args,Comms, Vars);        
            if(flag == 0){//if not in commands
                flag = storevar(input,Vars,args,Comms);
            }
            if(flag == 0){//if not in store variables
                execute(args,Comms, Vars);
            }
           del_arglist(args);
        }
    }
    else{
        int flag = 0;
        FILE *f = fopen(argv[1], "r"); //file case
        if (f == NULL){
            printf("Unable to read script file: %s\n",argv[1]);
            flag = 2;
            return 0;
        }
        char line[512] ="\0";
     
        while(flag != 2 && fgets(line,512,f) != NULL){
          
            arglist* args = createarglist();            
            
            pharseargs(line,args);
            flag = Commandbluitin(args,Comms, Vars);
            
            if(flag == 0){//if not in commands
                flag = storevar(line,Vars,args,Comms);
            }
            if(flag == 0){
                execute(args,Comms, Vars);
            }

           del_arglist(args);
        }
        fclose(f);
        }
    del_Commandlist(Comms);
    del_Variablelist(Vars);
    
 
    
}