#include <iostream>
#include "../include/compiler.hpp"
using namespace std;

const char* loadFile(const char* filename);

int main(int argc, char* argv[])
{
    for(int i=1; i < argc; i++)
    {
        const char* sourceCode = loadFile(argv[i]);
        compile(sourceCode, "./test.s");
    }
    return 0;
}

const char* loadFile(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Could not open file \"%s\".\n", filename);
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