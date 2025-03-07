#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

//The JackTokenizer module:
//Constructor:

#define CHUNK 32

FILE *inputFile;
char *currentToken = NULL; //current token from the contents of the input file/files
char *inputStream = NULL; //contents of the input file/files
int inputSize = 0; //size of the current input file

void removeCharacter(int pos)
{
    for(int i = pos; i < inputSize; i++)
    {
        inputStream[i] = inputStream[i+1];
    }
    inputSize--;
    inputStream[inputSize] = '\0';
}

void Constructor(const char *inputName)
{
    //printf("COMSTRUCTOR INPUT NAME: %s\n", inputName);
    inputStream = NULL;
    inputSize = 0;
    inputFile = fopen(inputName, "r");
    if(inputFile == NULL)
    {
        fprintf(stderr, "(Constructor): error opening file\n");
        exit(EXIT_FAILURE);
    }

    char c = ' ';
    int currentSize = 0;
    inputStream = (char*)malloc(CHUNK * sizeof(char));
    currentSize = currentSize + CHUNK;
    if(inputStream == NULL)
    {
        fprintf(stderr, "(Constructor): error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    while((c = fgetc(inputFile)) != EOF)
    {
        if(inputSize == currentSize)
        {
            char *p = (char*)realloc(inputStream, (currentSize + CHUNK) * sizeof(char));
            currentSize = currentSize + CHUNK;
            if(p == NULL)
            {
                fprintf(stderr, "(Constructor): error reallocating memory\n");
                free(inputStream);
                exit(EXIT_FAILURE);
            }
            inputStream = p;
        }
        inputStream[inputSize++] = c;
    }

    for(int i = 0; i < inputSize - 1; i++)
    {
        //removing single comments
        if(inputStream[i] == '/' && inputStream[i+1] == '/')
        {
            while(inputStream[i] != '\n' && inputStream[i] != '\0')
            {
                removeCharacter(i);
            }
        }
        //removing API comments
        if(i < inputSize - 2 && inputStream[i] == '/' && inputStream[i+1] == '*' && inputStream[i+2] == '*')
        {
            removeCharacter(i);
            removeCharacter(i);
            removeCharacter(i);
            while(!(inputStream[i] == '*' && inputStream[i+1] == '/') && inputStream[i] != '\0')
            {
                removeCharacter(i);
            }
            removeCharacter(i);
            removeCharacter(i);
        }
        //removing comments until closing
        if(inputStream[i] == '/' && inputStream[i+1] == '*')
        {
            removeCharacter(i);
            removeCharacter(i);
            while(!(inputStream[i] == '*' && inputStream[i+1] == '/') && inputStream[i] != '\0')
            {
                removeCharacter(i);
            }
            removeCharacter(i);
            removeCharacter(i);
        }
    }
      
    //printf("%s", inputStream);
}

bool hasMoreTokens()
{
    //REVIEW THIS!!!
    if(currentToken == NULL)
        return false;
    return true;
}

void advance()
{
    //REVIEW THIS!!!
    currentToken = strtok(NULL, "\n\r\t\f\v");
}

char **JackTokenizer(const char *inputName)
{
    char **token = NULL;
    //printf("jack tokenizer file input: %s\n", inputName);
    Constructor(inputName);

    //STRTOK IS NOT REALLY POSSIBLE, ITERATE THROUGH INPUT STREAM
    currentToken = strtok(inputStream, "\n\r\t\f\v");
    while(hasMoreTokens())
    {
        printf("%s\n", currentToken);
        advance();
    }

    currentToken = NULL;
    free(inputStream);
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