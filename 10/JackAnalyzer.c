#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

//The JackTokenizer module:
//Constructor:

FILE *inputFile;
void Constructor(const char *inputName)
{
    inputFile = fopen(inputName, "r");
    if(inputFile == NULL)
    {
        fprintf(stderr, "(Constructor): error opening file\n");
        exit(EXIT_FAILURE);
    }
}

bool hasMoreTokens()
{
    
}

char **JackTokenizer(const char *inputName)
{
    char **token = NULL;
    //printf("jack tokenizer file input: %s\n", inputName);
    Constructor(inputName);

    return token;
}

//CompilationEngine:

void CompilationEngine(FILE* ouputFile, char** token)
{

}

//JackAnalyzer:

int inputType(const char *inputName) //1 - directory, 0 - file
{
    if(strstr(inputName, ".jack") != NULL)
        return 0;
    return 1;
}

void analyzerLogic(char *inputName, char *fileName) //if input is file, fileName is NULL, since the inputName is the fileName already and not a directory
{
    char **token;
    if(inputType(inputName) == 0)
    {
        token = JackTokenizer(inputName); //Create a JackTokenizer from the Xxx.jack file
        printf("tokenized: %s\n", inputName);
    }
    else if(inputType(inputName) == 1)
    {
        char inputFileName[4097] = "";
        strcat(inputFileName, "./");
        strcat(inputFileName, inputName);
        strcat(inputFileName, "/");
        strcat(inputFileName, fileName);
        token = JackTokenizer(inputFileName);
        printf("tokenized: %s\n", fileName);
    }

    FILE* outputFile = NULL; 
    if(inputType(inputName) == 0) //Create an output file called Xxx.xml and prepare it for writing in the current directory
    {
        char outputName[256] = "";
        for(int i = 0; i < strlen(inputName); i++)
        {
            outputName[i] = inputName[i];
            if(inputName[i] == '.')
                break;
        }
        outputName[strlen(outputName) - 1] = '\0'; 
        strcat(outputName, ".xml");
        outputFile = fopen(outputName, "w");
        if(outputFile == NULL)
        {
        fprintf(stderr, "(analyzerLogic): error opening output file\n");
        }
        printf("created output file: %s\n", outputName);
    }
    else if(inputType(inputName) == 1) //Create an output file called Xxx.xml and prepare it for writing in the directory given by input
    {
        char outputName[256] = "";
        for(int i = 0; i < strlen(fileName); i++)
        {
            outputName[i] = fileName[i];
            if(fileName[i] == '.')
                break;
        }
        outputName[strlen(outputName) - 1] = '\0'; 
        strcat(outputName, ".xml");

        char outputPath[4097] = "";
        strcat(outputPath, "./");
        strcat(outputPath, inputName);
        strcat(outputPath, "/");
        strcat(outputPath, outputName);
        outputFile = fopen(outputPath, "w");
        //printf("current output path: %s\n", outputPath);
        printf("created output file: %s\n", outputName);
    }

    CompilationEngine(outputFile, token); //Use the CompilationEngine to compile the input JackTokenizer into the output file

    fclose(outputFile);
}

void JackAnalyzer(char *inputName)
{
    if(inputType(inputName))
    {
        printf("argument is: directory\n");
        char pathName[4097];
        strcat(pathName, "./");
        strcat(pathName, inputName);
        DIR* dir = opendir(pathName);
        if(dir == NULL)
        {
            fprintf(stderr, "(JackAnalyzer): error opening directory\n");
            exit(EXIT_FAILURE);
        }
        struct dirent* entity;
        entity = readdir(dir);
        while(entity != NULL)
        {
            //printf("%s\n", entity->d_name);
            if(strstr(entity->d_name, ".jack") != NULL)
            {
                printf(".jack file found: %s\n", entity->d_name);
                analyzerLogic(inputName, entity->d_name);
            }
            entity = readdir(dir);
        }
        closedir(dir);
    }
    else
    {
        printf("argument is: file\n");
        analyzerLogic(inputName, NULL);
    }
}

//main:
int main(int argc, char** argv)
{
    printf("%s\n", "~~~ Luca's Jack Analyzer ~~~");

    if(argc <= 1)
    {
        fprintf(stderr, "(main): not enough input arguments");
        exit(EXIT_FAILURE);
    }
    JackAnalyzer(argv[1]);

    return 0;
}