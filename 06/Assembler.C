#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE_ASM 655360
#define MAX_SIZE_ASM_LINE 1024
#define MAX_SIZE_HACK 65536

char asmb[MAX_SIZE_ASM][MAX_SIZE_ASM_LINE];
char hack[MAX_SIZE_HACK][MAX_SIZE_HACK];

typedef enum{
    A_COMMAND,
    C_COMMAND,
    L_COMMAND
}command_type;

bool is_empty_line(const char *line) 
{
    for (int i=0; i<(int)strlen(line); i++)
    {
        if (line[i]!=' ' && line[i]!='\t' && line[i]!='\n' && line[i]!='\r')
            return false;
    }
    return true;
}

int open_asm(const char* filename)
{
    FILE *f=NULL;
    if((f=fopen(filename, "r"))==NULL)
    {
        fprintf(stderr, "error opening .asm file\n");
        exit(EXIT_FAILURE);
    }
    
    char line[MAX_SIZE_ASM_LINE];
    int size=0;
    while(fgets(line, MAX_SIZE_ASM_LINE, f)!=NULL)
    {
        //printf("%d %s", size, line);
        if((line[0]=='/' && line[1]=='/') || is_empty_line(line))
            continue;
        strcpy(asmb[size], line);
        size++;
    }
    printf(".asm file read successfully\n");

    if(fclose(f)!=0)
    {
        fprintf(stderr, "error closing .asm file\n");
        exit(EXIT_FAILURE);
    }
    return size;
}

void symbol(char *s, char *line, command_type cmd)
{
    s[0]='\0';
    if(cmd==A_COMMAND)
    {
        char *p=strtok(line, "@");
        strcpy(s, p);
    }
    else
    {
        int k=0;
        for(int i=0; i<(int)strlen(line); i++)
        {
            if(line[i]!='(' && line[i]!=')')
            {
                s[k]=line[i];
                k++;
            }
        }
    }
}

void dest(char *m, char *line)
{
    m[0]='\0';
    if(strchr(line, '=')==NULL)
        strcpy(m, "null");
    if(line[0]=='M')
        strcpy(m, "M");
    if(line[0]=='D')
        strcpy(m, "D");
    if(line[0]=='M' && line[1]=='D')
        strcpy(m, "MD");
    if(line[0]=='A')
        strcpy(m, "A");
    if(line[0]=='A' && line[1]=='M')
        strcpy(m, "AM");
    if(line[0]=='A' && line[1]=='D')
        strcpy(m, "AD");
    if(line[0]=='A' && line[1]=='M' && line[2]=='D')
        strcpy(m, "AMD");
}

void comp(char *m, char *line)
{
    m[0]='\0';
    int ok_eq=0, ok_c=0;
    for(int i=0; i<(int)strlen(line); i++)
    {
        if(line[i]=='=')
            ok_eq=1;
        if(line[i]==';')
            ok_c=1;
    }
    char c[1024][1024];
    char line_copy[strlen(line)+1];
    strcpy(line_copy, line);
    char *p=strtok(line_copy, "=;");
    int n=0;
    while(p!=NULL)
    {
        strcpy(c[n], p);
        n++;
        p=strtok(NULL, "=;");
    }
    if(ok_eq==1 && ok_c==0)
    {
        c[1][strlen(c[1])-1]='\0';
        strcpy(m, c[1]);
    }
    if(ok_eq==0 && ok_c==1)
        strcpy(m, c[0]);
    if(ok_eq==1 && ok_c==1)
        strcpy(m, c[1]);
}

void jump(char *m, char *line)
{
    int ok=0;
    int k=0;
   for(int i=0; i<(int)strlen(line); i++)
   {
        if(ok==1)
            m[k++]=line[i];
        if(line[i]==';')
            ok=1;
   }
   if(ok==0)
    strcpy(m, "null\n");
}

command_type commandType(char *line)
{
    //A_COMMAND
    if(line[0]=='@')
        return A_COMMAND;
    //L_COMMAND
    if(line[0]=='(')
        return L_COMMAND;
    //C_COMMAND
    return C_COMMAND;
}

void write_hack(char *filename, int size)
{
    FILE *f=NULL;
    char filename_hack[100];
    char *p=strtok(filename, ".");
    strcpy(filename_hack, p);
    strcat(filename_hack, ".hack");
    
    if((f=fopen(filename_hack, "w"))==NULL)
    {
        fprintf(stderr, "error opening .hack file\n");
        exit(EXIT_FAILURE);
    }

    //copy-paste of the .asm file:
    /*for(int i=0; i<size; i++)
    {
        fprintf(f, "%s", asmb[i]);
    }*/
   
   //prompt:
   char sb[1024];
   char mnd[8];
   char mnc[32];
   char mnj[8];
    for(int i=0; i<size; i++)
    {
        if(commandType(asmb[i])==A_COMMAND)
        {
            symbol(sb, asmb[i], A_COMMAND);
            fprintf(f, "commandType: A_COMMAND, symbol: %s", sb);
        }
        else if(commandType(asmb[i])==C_COMMAND)
        {
            dest(mnd, asmb[i]);
            comp(mnc, asmb[i]);
            jump(mnj, asmb[i]);
            fprintf(f, "commandType: C_COMMAND, dest: %s, comp: %s, jump: %s", mnd, mnc, mnj);
        }
        else if(commandType(asmb[i])==L_COMMAND)
        {
            symbol(sb, asmb[i], L_COMMAND);
            fprintf(f, "commandType: L_COMMAND, symbol: %s", sb);
        }
    }

    printf(".hack file written successfully\n");

    if(fclose(f)!=0)
    {
        fprintf(stderr, "error closing .hack file\n");
        exit(EXIT_FAILURE);
    }
}

int correct_input_format(char *filename)
{
    if(strlen(filename)-1>100)
    {
        fprintf(stderr, ".asm file name too large\n");
        return 0;
    }
    int ok=0;
    for(int i=0; i<(int)strlen(filename); i++)
    {
        if(filename[i]=='.')
            ok=1;
    }
    if(ok==0)
    {
        fprintf(stderr, "invalid file format\n");
        return 0;
    }
    return 1;
}

int main(int argc, char **argv)
{
    printf("~~~Luca's Assembler for Hack~~~\n!!! Line size must be max 1024 characters\n!!! Do not include indentation\n");
    int asmb_size;
    if(correct_input_format(argv[1])==0)
    {
        exit(EXIT_FAILURE);
    }
    asmb_size=open_asm(argv[1]);
    write_hack(argv[1], asmb_size);
    return 0;
}