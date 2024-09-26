//half of the VM part 1 implementation, this file covers the parser module
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define CHUNK 64

char **vm=NULL;
int lines=0;
int line_size=0;

typedef enum{
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL,
    null
}command_type;

int isDirectory(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode); //1 - directory, 0 - not directory
}

int isVMfile(const char *filename)
{
    if(strstr(filename, ".vm")!=NULL)
        return 1;
    return 0;
}

void removeWhitespace(char *line)
{
    //trailing whitespace
    int i=strlen(line)-1;
    while(i>=0 && isspace(line[i])) 
    {
        i--;
    }
    line[i+1]='\0';
    //printf("%s\n", line);
    //leading whitespace
    int j=0;
    while(line[j]!='\0' && isspace(line[j]))
    {
        j++;
    } 
    if(j>0)
    {
        memmove(line, line+j, strlen(line+j)+1);
    }
    //printf("%s\n", line);
}

int isBlankLine(const char *line)
{
    for(int i=0; i<(int)strlen(line); i++)
        if(!isspace(line[i]))
            return 0;
    return 1;
}

char *comment(char *line)
{
    char *aux=(char*)malloc((strlen(line)+strlen("//")+strlen("\n")+1)*sizeof(char));
    if(aux==NULL)
    {
        fprintf(stderr, "(comment) error: memory allocation\n");
        exit(EXIT_FAILURE);
    }
    strcpy(aux, "//");
    strcat(aux, line);
    strcat(aux, "\n");
    return aux;
}

command_type commandType(char *line)
{
    char check_push[5];
    strncpy(check_push, line, 4);
    check_push[4]='\0';
    char check_pop[4];
    strncpy(check_pop, line, 3);
    check_pop[3]='\0';
    char check_label[6];
    strncpy(check_label, line, 5);
    check_label[5]='\0';
    char check_goto[5];
    strncpy(check_goto, line, 4);
    check_goto[4]='\0';
    char check_if[8];
    strncpy(check_if, line, 7);
    check_if[7]='\0';
    char check_function[9];
    strncpy(check_function, line, 8);
    check_function[8]='\0';
    char check_call[5];
    strncpy(check_call, line, 4);
    check_call[4]='\0';
    char check_return[7];
    strncpy(check_return, line, 6);
    check_return[6]='\0';


    if(strcmp(line, "add")==0 ||
    strcmp(line, "sub")==0 ||
    strcmp(line, "neg")==0 ||
    strcmp(line, "eq")==0 ||
    strcmp(line, "gt")==0 ||
    strcmp(line, "lt")==0 ||
    strcmp(line, "and")==0 ||
    strcmp(line, "or")==0 ||
    strcmp(line, "not")==0)
        return C_ARITHMETIC;
    else if(strcmp(check_push, "push")==0)
        return C_PUSH;
    else if(strcmp(check_pop, "pop")==0)
        return C_POP;
    else if(strcmp(check_label, "label")==0)
        return C_LABEL;
    else if(strcmp(check_goto, "goto")==0)
        return C_GOTO;
    else if(strcmp(check_if, "if-goto")==0)
        return C_IF;
    else if(strcmp(check_function, "function")==0)
        return C_FUNCTION;
    else if(strcmp(check_call, "call")==0)
        return C_CALL;
    else if(strcmp(check_return, "return")==0)
        return C_RETURN;
    return null;
}

char *arg1(char *line)
{
    if(commandType(line)==C_ARITHMETIC)
        {
            char *aux=(char*)malloc((strlen(line)+1)*sizeof(char));
            strcpy(aux, line);
            return aux;
        }
    else if(commandType(line)!=C_RETURN) //should not be called if the current command is C_RETURN
    {
        char *aux=(char*)malloc((strlen(line)+1)*sizeof(char));
        if(aux==NULL)
        {
            fprintf(stderr, "(arg1) error: memory allocation\n");
            exit(EXIT_FAILURE);
        }
        strcpy(aux, line);
        char *p=strtok(aux, " ");
        if(p!=NULL)
        {
            p=strtok(NULL, " ");
            char *arg=(char*)malloc((strlen(p)+1)*sizeof(char));
            if(arg==NULL)
            {
                fprintf(stderr, "(arg1) error: memory allocation\n");
                free(aux);
                exit(EXIT_FAILURE);
            }
            strcpy(arg, p);
            free(aux);
            return arg;
        }
    }
    return NULL;
}

int arg2(char *line)
{
    //should be called only if current command is C_PUSH, C_POP, C_FUNCTION, or C_CALL; any exceptions from this will lead to unexpected behavior
    char *aux=(char*)malloc((strlen(line)+1)*sizeof(char));
    if(aux==NULL)
    {
        fprintf(stderr, "(arg2) error: memory allocation\n");
        exit(EXIT_FAILURE);
    }
    strcpy(aux, line);
    char *p=strtok(aux, " ");
    if(p!=NULL)
    {
        p=strtok(NULL, " ");
        if(p!=NULL)
        {
            p=strtok(NULL, " ");
            int arg=strtol(p, NULL, 10);
            free(aux);
            return arg;
        }
    }
    return NULL;
}

void openVMfile(const char *filename) //or path
{
    FILE *f=NULL;
    f=fopen(filename, "r");
    if(f==NULL)
    {
        fprintf(stderr, "(openVMfile) error: opening file\n");
        exit(EXIT_FAILURE);
    }

    char *line=NULL;
    size_t bufsize=0;
    while((line_size=getline(&line, &bufsize, f))!=-1)
    {
        removeWhitespace(line);
        if(isBlankLine(line)==1)
            continue;
        if(line[0]=='/' && line[1]=='/')
            continue;
        if(lines==0 || lines%CHUNK==0)
        {
            char **temp=NULL;
            temp=(char**)realloc(vm, (lines+CHUNK)*sizeof(char*));
            if(temp==NULL)
            {
                for(int i=0; i<lines; i++)
                    free(vm[i]);
                free(vm);
                fprintf(stderr, "(openVMfile) error: memory allocation\n");
                exit(EXIT_FAILURE);
            }
            vm=temp;
        }
        int trimmed_line_size=strlen(line)+1;
        vm[lines]=(char*)malloc(trimmed_line_size*sizeof(char));
        if(vm[lines]==NULL)
        {
            fprintf(stderr, "(openVMfile) error: memory allocation\n");
            exit(EXIT_FAILURE);
        }
        strcpy(vm[lines], line);
        line[0]='\0';
        lines++;
    }
    free(line);
    printf("input file read successfully\n");

    if(fclose(f)!=0)
    {
        fprintf(stderr, "(openVMfile) error: closing file\n");
        exit(EXIT_FAILURE);
    }
}

void openVM(const char *path)
{
    //this function takes place for the following routines: Constructor, hasMoreCommands, advance
    if(isDirectory(path)==1)
    {
        printf("argument is directory\n");
        DIR *directory;
        struct dirent *entry;
        directory=opendir(path);
        if(directory==NULL)
        {
            fprintf(stderr, "(openVM) error: opening directory\n");
            exit(EXIT_FAILURE);
        }
        while((entry=readdir(directory))!=NULL)
        {
            if(isVMfile(entry->d_name)==1)
            {
                char filepath[1024]="\0";
                strcat(filepath, "./");
                strcat(filepath, path);
                strcat(filepath, "/");
                printf(".vm file found: %s\n", entry->d_name);
                strcat(filepath, entry->d_name);
                openVMfile(filepath);
            }
        }
        if(closedir(directory)==-1)
        {
            fprintf(stderr, "(openVM) error: closing directory\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        if(isVMfile(path)==0)
        {
            fprintf(stderr, "(openVM) error: .vm extension is mandatory\n");
            exit(EXIT_FAILURE);
        }
        else
        {
             printf("argument is file\n");
             openVMfile(path);
        }
    }
}

int main(int argc, char **argv)
{
    printf("~~~ Luca's VM translator ~~~\n");
    openVM(argv[1]);
    //testing/debugging:
    for(int i=0; i<lines; i++)
        {
           //printf("%s\n", vm[i]);
           char *com=NULL;
           char *a1=NULL;
           int a2;

           com=comment(vm[i]);
           printf("comment: %s", com);
           free(com);

           printf("commandType: ");
           if(commandType(vm[i])==C_ARITHMETIC)
                printf("C_ARITHMETIC\n");
            else if(commandType(vm[i])==C_CALL)
                printf("C_CALL\n");
            else if(commandType(vm[i])==C_FUNCTION)
                printf("C_FUNCTION\n");
            else if(commandType(vm[i])==C_GOTO)
                printf("C_GOTO\n");
            else if(commandType(vm[i])==C_IF)
                printf("C_IF\n");
            else if(commandType(vm[i])==C_LABEL)
                printf("C_LABEL\n");
            else if(commandType(vm[i])==C_POP)
                printf("C_POP\n");
            else if(commandType(vm[i])==C_PUSH)
                printf("C_PUSH\n");
            else if(commandType(vm[i])==C_RETURN)
                printf("C_RETURN\n");

            if(commandType(vm[i])!=C_RETURN)
            {
                a1=arg1(vm[i]);
                printf("arg1: %s\n", a1);
                free(a1);
            }

            if(commandType(vm[i])==C_PUSH || commandType(vm[i])==C_POP || commandType(vm[i])==C_FUNCTION || commandType(vm[i])==C_CALL)
            {
                a2=arg2(vm[i]);
                printf("arg2: %d\n", a2);
            }

            printf("\n");
        }

    for(int i=0; i<lines; i++)
        free(vm[i]);
    free(vm);
    return 0;
}