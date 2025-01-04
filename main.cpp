#include <stdio.h>
#include <cstdlib>
#include "compiler.hpp"
#include <errno.h>
#include <string.h>
#include <fcntl.h>

const char* loadFile(const char* filename);
FILE* openOutputFile(const char* outputFile);
int main(int agrc, char* args[])
{
    if (agrc != 2 )
    {
        printf("Incorrect number of arguments\n");
        exit(-1);
    }

    const char* file = loadFile(args[agrc -1 ]);
    FILE* stream = openOutputFile(nullptr);

    compile(file, stream);
    fclose(stream);
}

const char* loadFile(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("%s",  strerror(errno));
        exit(-1);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", filename);
        exit(-2);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize)
    {
        fprintf(stderr, "Could not read file \"%s\".\n", filename);
        exit(-3);
    }
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

FILE* openOutputFile(const char *outputFile)
{
    if(outputFile == nullptr)
    {
        outputFile = "out.s";
    }

    FILE* file = fopen(outputFile, "wb");
    if (file == NULL)
    {
        printf("%s",  strerror(errno));
        exit(-1);
    }
    return file;
}
