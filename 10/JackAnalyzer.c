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
char *currentLine = NULL; //current line from the contents of the input file/files
char *inputStream = NULL; //contents of the input file/files
int inputSize = 0; //size of the current input file
char currentToken[32768];
int currentTokenIndex = 0;

char *symbol = "{}()[].,;+-*/&|<>=-";

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
    //printf("CONSTRUCTOR INPUT NAME: %s\n", inputName);
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
    if(currentLine == NULL)
        return false;
    return true;
}

void advance()
{
    currentLine = strtok(NULL, "\n\r\t\f\v");
}

void clearCurrentToken()
{
    memset(currentToken, 0, sizeof(currentToken));
    currentTokenIndex = 0;
}

void getIntegerConstant(int *i)
{
    while(isdigit(currentLine[*i]))
    {
        currentToken[currentTokenIndex++] = currentLine[*i];
        (*i)++;
    }
    currentToken[currentTokenIndex] = '\0';
}

void getStringConstant(int *i)
{
    (*i)++;
    while(currentLine[*i] != '"')
    {
        currentToken[currentTokenIndex++] = currentLine[*i];
        (*i)++;
    }
    currentToken[currentTokenIndex] = '\0';
}

void getSymbol(int *i)
{
    char *found = strchr(symbol, currentLine[*i]);
    currentToken[currentTokenIndex++] = found[0];
    currentToken[currentTokenIndex] = '\0';
    //printf("DEBUG: Symbol at index %d: '%c'\n", i, found[0]);
}

void getKeywordOrIdentifier(int *i)
{
    while(isalnum(currentLine[*i]) || currentLine[*i] == '_')
    {
        currentToken[currentTokenIndex++] = currentLine[*i];
        (*i)++;
    }
    (*i)--;
    currentToken[currentTokenIndex] = '\0';
}

char **JackTokenizer(const char *inputName, int *tokenSize)
{
    char **token = NULL;
    int current_size = 0;
    int size = CHUNK;
    //printf("jack tokenizer file input: %s\n", inputName);
    Constructor(inputName);
    if(!strlen(inputStream))
    {
        return token;
    }
    currentLine = strtok(inputStream, "\n\r\t\f\v");
    token = (char**)malloc(CHUNK * sizeof(char*));
    if(token == NULL)
    {
        fprintf(stderr, "(JackTokenizer): Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    if(strlen(currentLine) > 32767)
    {
        fprintf(stderr, "(JackTokenizer): line length exceeded\n");
        exit(EXIT_FAILURE);
    }
    while(hasMoreTokens())
    {
        if(current_size == size)
        {
            char **t = (char**)realloc(token, (size + CHUNK) * sizeof(char*));
            size += CHUNK;
            if(t == NULL)
            {
                fprintf(stderr, "(JackTokenizer): Memory allocation error\n");
                for(int i = 0; i < current_size; i++)
                {
                    free(token[i]);
                }
                free(token);
                exit(EXIT_FAILURE);
            }
            token = t;
        }
        //current_size has to be incremented after every strcpy, make a function for individiual token line
        //memory allocation
        token[current_size] = (char*)malloc((strlen(currentToken) + 1) * sizeof(char));
        if(token[current_size] == NULL)
        {
            fprintf(stderr, "(JackTokenizer): Memory allocation error\n");
                for(int i = 0; i < current_size; i++)
                {
                    free(token[i]);
                }
                free(token);
                exit(EXIT_FAILURE);
        }
        //printf("LINE: %s\n", currentLine);
        int i = 0;
        while(i < strlen(currentLine))
        {
            if(isspace(currentLine[i]))
            {
                i++;
            }
            if(isdigit(currentLine[i]))
            {
                getIntegerConstant(&i);
                //printf("INTEGER: %s\n", currentToken);
                strcpy(token[current_size], currentToken);
                clearCurrentToken();
            }
            if(currentLine[i] == '"')
            {
                getStringConstant(&i);
                //printf("STRING: %s\n", currentToken);
                strcpy(token[current_size], currentToken);
                clearCurrentToken();
            }
            if(strchr(symbol, currentLine[i]) != NULL)
            {
                getSymbol(&i);
                if(currentToken[0] != '\0')
                {
                    //printf("SYMBOL: %s\n", currentToken);
                    strcpy(token[current_size], currentToken);
                }
                clearCurrentToken();
            }
            if(isalpha(currentLine[i]) || currentLine[i] == '_')
            {
                getKeywordOrIdentifier(&i);
                //printf("KEYIDENT: %s\n", currentToken);
                strcpy(token[current_size], currentToken);
                clearCurrentToken();
            }
            i++;
        }
        advance();
    }

    currentLine = NULL;
    *tokenSize = current_size;
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
    int tokenSize = 0;
    if(inputType(inputName) == 0)
    {
        token = JackTokenizer(inputName, &tokenSize); //Create a JackTokenizer from the Xxx.jack file
        printf("tokenized: %s\n", inputName);
    }
    else if(inputType(inputName) == 1)
    {
        char inputFileName[4097] = "";
        strcat(inputFileName, "./");
        strcat(inputFileName, inputName);
        strcat(inputFileName, "/");
        strcat(inputFileName, fileName);
        token = JackTokenizer(inputFileName, &tokenSize);
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
    //printf("token size: %d\n", tokenSize);

    for(int i = 0; i < tokenSize; i++)
    {
        printf("%s\n", token[i]);
    }

    if(token != NULL)
    {
        for(int i = 0; i < tokenSize; i++)
        {
            free(token[i]);
        }
        free(token);
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