#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE_ASM 655360
#define MAX_SIZE_HACK 65536

char asmb[MAX_SIZE_ASM][100];
char hack[MAX_SIZE_HACK][MAX_SIZE_HACK];
int asmb_size=0;

void open_asm(const char* filename)
{
    FILE *f=NULL;
    if((f=fopen(filename, "r"))==NULL)
    {
        fprintf(stderr, "error opening .asm file\n");
        exit(EXIT_FAILURE);
    }
    printf("file opened successfully\n");
    char line[MAX_SIZE_ASM];
    int asmb_size=0;
    while(fgets(line, MAX_SIZE_ASM, f)!=NULL)
    {
        if(strlen(line)>100)
            {
                fprintf(stderr, "lines of .asm file cannot exceed 100 character limit\n");
                exit(EXIT_FAILURE);
            }
        strcpy(asmb[asmb_size], line);
        asmb_size++;
    }
    printf("asmb written successfully\n");
    if(fclose(f)!=0)
    {
        fprintf(stderr, "error closing .asm file\n");
        exit(EXIT_FAILURE);
    }
    
}

void write_hack(const char *filename)
{
    FILE *f=NULL;
    char filename_hack[100];
    int i;
    for(i=0; i<strlen(filename); i++)
    {
        if(filename[i]=='.')
            break;
        else
            filename_hack[i]=filename[i];
    }
    strcat(filename_hack, ".hack");
    
    if((f=fopen(filename_hack, "w"))==NULL)
    {
        fprintf(stderr, "error opening .hack file\n");
        exit(EXIT_FAILURE);
    }
    
    //testing the file conversion
    fprintf(f, "%s", asmb);

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
    for(int i=0; i<strlen(filename); i++)
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
    if(correct_input_format(argv[1])==0)
    {
        exit(EXIT_FAILURE);
    }
    open_asm(argv[1]);
    write_hack(argv[1]);
    return 0;
}