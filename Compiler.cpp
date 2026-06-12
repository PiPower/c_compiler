#include "Compiler.hpp"
#include "frontend/Parser.hpp"
#include <string.h>
#include <assert.h>
Compiler::Compiler(int argc, char *argv[])
:
    argc(argc), argv(argv), opts(), fileManager()
{
    // standard gcc - 11 search paths

    opts.AddSearchPath("/usr/lib/gcc/x86_64-linux-gnu/11/include", 40);
    opts.AddSearchPath("/usr/local/include", 18);
    opts.AddSearchPath("/usr/include/x86_64-linux-gnu", 29);
    opts.AddSearchPath("/usr/include", 12);
    opts.ParseArgs(argc, (const char**)argv);

    assert(opts.filenames.size() == opts.filenameLens.size());
    size_t maxLen = *std::max_element(opts.filenameLens.begin(), opts.filenameLens.end()) + 1;
    char* pathBuffer = new char[maxLen];

    for(size_t i =0; i < opts.filenames.size(); i++)
    {
        memcpy(pathBuffer, opts.filenames[i], opts.filenameLens[i]);
        pathBuffer[opts.filenameLens[i]] = '\0';

        if(fileManager.TryLoadFile(pathBuffer, opts.filenameLens[i], nullptr) < 0 )
        {
            printf("File %s does not exist", pathBuffer);
            exit(-1);
        }
    }
    delete[] pathBuffer;
}

void Compiler::compile()
{
    for(size_t i =0; i < opts.filenames.size(); i++)
    {
        FILE_ID mainFile;
        SymbolTable symtab{};
        // skip err check, constructor checks for all main files
        fileManager.GetFileId( opts.filenames[i],  opts.filenameLens[i], &mainFile);
        SemanticAnalyzer analyzer(&fileManager, &symtab);
        Parser parser(mainFile, &analyzer, &fileManager, &opts);
        while (Ast::Node* ast = parser.Parse())
        {
            analyzer.Analyze(ast);
        }
        analyzer.EmitUninitializedGlobals();
        
        char* buff = (char*)alloca(opts.filenameLens[i] + 1);
        memcpy(buff, opts.filenames[i], opts.filenameLens[i]);
        size_t j;
        for(j = opts.filenameLens[i] - 1; j > 0; j--)
        {
            if(buff[j] == '.'){break;}
        }
        buff[j + 1] = 'S';
        buff[j + 2] = '\0';
        analyzer.WriteCodeToFile(buff);
    }
}
