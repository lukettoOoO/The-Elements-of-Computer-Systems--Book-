#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

#define CHUNK 32
#define MAX_HACK_SIZE 32768
#define KEY_COUNT 21

FILE *inputFile;
char *currentLine = NULL; //current line from the contents of the input file/files
char *inputStream = NULL; //contents of the input file/files
int inputSize = 0; //size of the current input file
char currentToken[MAX_HACK_SIZE];
int currentTokenIndex = 0;

char *symbol = "{}()[].,;+-*/&|<>=~";
char *keyword[] = {"class", "constructor", "function",
    "method", "field", "static", "var",
    "int", "char", "boolean", "void", "true",
    "false", "null", "this", "let", "do",
    "if", "else", "while", "return"};

typedef enum {
    KEYWORD, //0
    SYMBOL, //1
    IDENTIFIER, //2
    INT_CONST, //3
    STRING_CONST, //4
}token_type;

typedef enum {
    CLASS,
    METHOD,
    FUNCTION,
    CONSTRUCTOR,
    INT,
    BOOLEAN,
    CHAR,
    VOID,
    VAR,
    STATIC,
    FIELD,
    LET,
    DO,
    IF,
    ELSE,
    WHILE,
    RETURN,
    TRUE,
    FALSE,
    null,
    THIS
}key_type;

void removeCharacter(int pos)
{
    for(int i = pos; i < inputSize; i++)
    {
        inputStream[i] = inputStream[i+1];
    }
    inputSize--;
    inputStream[inputSize] = '\0';
}


//The JackTokenizer module:
//Constructor:

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

token_type tokenType(char *token)
{
    if(strchr(token, '"') != NULL)
        return STRING_CONST;

    int isIntConst = true;
    for(int i = 0; i < strlen(token); i++)
    {
        if(!isdigit(token[i]))
            isIntConst = false;
    }
    if(isIntConst)
        return INT_CONST;

    if(strchr(symbol, token[0]) != NULL)
        return SYMBOL;

    
    for(int i = 0; i < KEY_COUNT; i++)
    {
        if(strcmp(keyword[i], token) == 0)
        {
            return KEYWORD;
        }
    }

    return IDENTIFIER;
}

key_type keyWord(char *key)
{
    
}

//main function for JackTokenizer:
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
    currentToken[currentTokenIndex++] = currentLine[*i];
    (*i)++;
    while(currentLine[*i] != '"')
    {
        currentToken[currentTokenIndex++] = currentLine[*i];
        (*i)++;
    }
    currentToken[currentTokenIndex++] = currentLine[*i];
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

char *tokenAlloc(char **token, int tokenSize, int size)
{
    char *t = (char*)malloc(size * sizeof(char));
    if(t == NULL)
    {
        fprintf(stderr, "(tokenAlloc): Memory allocation error\n");
        for(int i = 0; i < tokenSize; i++)
        {
            if(token[i] != NULL)
                free(token[i]);
        }
        if(token != NULL)
            free(token);
        exit(EXIT_FAILURE);
    }
    return t;
}

char **JackTokenizer(const char *inputName, int *tokenSize)
{
    char **token = NULL;
    int currentSize = 0;
    //printf("jack tokenizer file input: %s\n", inputName);
    Constructor(inputName);
    if(!strlen(inputStream))
    {
        return token;
    }
    currentLine = strtok(inputStream, "\n\r\t\f\v");
    token = (char**)malloc(MAX_HACK_SIZE * sizeof(char*));
    if(token == NULL)
    {
        fprintf(stderr, "(JackTokenizer): Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    if(strlen(currentLine) > MAX_HACK_SIZE - 1)
    {
        fprintf(stderr, "(JackTokenizer): line length exceeded\n");
        exit(EXIT_FAILURE);
    }
    while(hasMoreTokens())
    {
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
                token[currentSize] = tokenAlloc(token, currentSize, strlen(currentToken));
                strcpy(token[currentSize], currentToken);
                currentSize++;
                clearCurrentToken();
            }
            if(currentLine[i] == '"')
            {
                getStringConstant(&i);
                //printf("STRING: %s\n", currentToken);
                token[currentSize] = tokenAlloc(token, currentSize, strlen(currentToken));
                strcpy(token[currentSize], currentToken);
                currentSize++;
                clearCurrentToken();
            }
            if(strchr(symbol, currentLine[i]) != NULL)
            {
                getSymbol(&i);
                if(currentToken[0] != '\0')
                {
                    //printf("SYMBOL: %s\n", currentToken);
                    token[currentSize] = tokenAlloc(token, currentSize, strlen(currentToken));
                    strcpy(token[currentSize], currentToken);
                    currentSize++;
                }
                clearCurrentToken();
            }
            if(isalpha(currentLine[i]) || currentLine[i] == '_')
            {
                getKeywordOrIdentifier(&i);
                //printf("KEYIDENT: %s\n", currentToken);
                token[currentSize] = tokenAlloc(token, currentSize, strlen(currentToken));
                strcpy(token[currentSize], currentToken);
                currentSize++;
                clearCurrentToken();
            }
            i++;
        }
        advance();
    }

    currentLine = NULL;
    *tokenSize = currentSize;
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
            exit(EXIT_FAILURE);
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
        if(outputFile == NULL)
        {
            fprintf(stderr, "(analyzerLogic): error opening output file\n");
            exit(EXIT_FAILURE);
        }
        //printf("current output path: %s\n", outputPath);
        printf("created output file: %s\n", outputName);
    }
    printf("token size: %d\n", tokenSize);

    for(int i = 0; i < tokenSize; i++)
    {
        printf("%u: %s\n", tokenType(token[i]),  token[i]);
    }

    if(token != NULL)
    {
        for(int i = 0; i < tokenSize; i++)
        {
            if(token[i] != NULL)
                free(token[i]);
        }
        if(token != NULL)
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